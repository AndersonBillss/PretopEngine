#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include "../PrintStringView.hpp"

#include "../Window/WindowFactory.hpp"
#include "../SHARED_/GetAssetId.hpp"

#include "../RHI/Application.hpp"
#include "../RHI/Device.hpp"
#include "../RHI/Shader.hpp"
#include "../RHI/Pipeline.hpp"
#include "../RHI/CommandBuffer.hpp"
#include "../RHI/RenderPassCommand.hpp"
#include "../RHI/VertexLayout.hpp"
#include "../RHI/BindingLayout.hpp"
#include "../RHI/Buffer.hpp"
#include "../RHI/BindGroup.hpp"

#include "../Math/CeilToBufferOffset.hpp"
#include "../Math/Linalg/Mat4x4.hpp"
#include "../Math/Constants.hpp"
#include "../Math/Euler.hpp"

#include "../Asset/GeneratedAssetCatalog.hpp"
#include "../Asset/AssetLoaderFactory.hpp"
#include "../Asset/AssetManagerFactory.hpp"
#include "../Core/JobSystem.hpp"

#include "../Rendering/Renderer.hpp"

namespace Pretop::Demos
{
    using namespace Pretop::Asset;
    using namespace Pretop::Math;
    using namespace Pretop::RHI;
    using namespace Pretop::Window;

    struct MyUniforms
    {
        Mat4x4 ProjectionMatrix;
        Mat4x4 ViewMatrix;
        Mat4x4 ModelMatrix;
        float Color;
        float Time;
        float Pad[2];
    };

    const float ModelScale = 0.5f;

    struct DemoState
    {
        std::unique_ptr<Core::JobSystem> Jobs;
        std::unique_ptr<AssetManager> Assets;
        AssetManager::AssetReference<ParsedData> ModelRef;
        ParsedData *Model;

        std::unique_ptr<Buffer> Vertices;
        std::unique_ptr<Buffer> Indices;
        WGPUIndexFormat IndexFormat = WGPUIndexFormat_Undefined;

        std::unique_ptr<Rendering::Renderer> Renderer;

        bool ModelReady = false;

        float Seconds;
    };

    void InitializeModel(Application &application, DemoState &state)
    {
        std::cout << "Initializing Model..." << std::endl;
        AssetManager::AssetResult modelLoadStatus = state.Assets->GetGlbData(state.ModelRef, &state.Model);
        if (modelLoadStatus != AssetManager::AssetResult::Success)
            return;

        state.Vertices = std::make_unique<Buffer>(
            application.Device.get(),
            state.Model->Vertices.size() * sizeof(Vertex),
            WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
        application.WriteVec(*state.Vertices, state.Model->Vertices);

        size_t indicesSize = 0;
        if (std::holds_alternative<std::vector<uint32_t>>(state.Model->Indices))
        {
            auto &indices = std::get<std::vector<uint32_t>>(state.Model->Indices);
            indicesSize = indices.size() * sizeof(uint32_t);
            state.IndexFormat = WGPUIndexFormat_Uint32;
        }
        else
        {
            auto &indices = std::get<std::vector<uint16_t>>(state.Model->Indices);
            indicesSize = indices.size() * sizeof(uint16_t);
            state.IndexFormat = WGPUIndexFormat_Uint16;
        }

        state.Indices = std::make_unique<Buffer>(
            application.Device.get(),
            indicesSize,
            WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index);
        if (state.IndexFormat == WGPUIndexFormat_Uint32)
        {
            auto &indices = std::get<std::vector<uint32_t>>(state.Model->Indices);
            application.WriteVec(*state.Indices, indices);
        }
        else
        {
            auto &indices = std::get<std::vector<uint16_t>>(state.Model->Indices);
            application.WriteVec(*state.Indices, indices);
        }

        Rendering::Renderer::GpuMesh mesh{
            /* WGPUBuffer */ state.Vertices->WgpuBuffer,
            /* WGPUBuffer */ state.Indices->WgpuBuffer,
            /* WGPUIndexFormat */ state.IndexFormat,
        };
        state.Renderer->AddObject(mesh);

        state.ModelReady = true;
        std::cout << "Model initialized!" << std::endl;
    }

    void Start(Application &application)
    {
        auto state = std::make_shared<DemoState>();
        state->Jobs = std::make_unique<Core::JobSystem>();
        state->Assets = AssetManagerFactory::CreateAssetManager(
            AssetLoaderFactory::CreateAssetLoader(
                state->Jobs.get(),
                std::move(Pretop::Asset::CreateGeneratedAssetCatalog())),
            application.GetGraphicsContext());

        state->ModelRef = state->Assets->LoadModel(
            Pretop::Utils::GetAssetId("models/woolly-mammoth-100k-4096_std.glb"));

        application.LogQueueCommands();
        application.SetWindow(WindowFactory::CreateWindow("My Window"));

        state->Renderer = std::make_unique<Rendering::Renderer>(state->Assets.get(), application.GetGraphicsContext(), application.WindowFormat);

        application.Run(
            [state, &application](
                double dt,
                WGPUTextureView targetView)
            {
                state->Jobs->PumpMainThreadCompletions();
                if (!state->ModelReady)
                {
                    InitializeModel(application, *state);
                }
                if (!state->Renderer->PollReady() || !state->ModelReady)
                {
                    return;
                }

                state->Seconds += dt;

                Mat4x4 r1 = (Euler{90.0f * (float)Deg2Rad, 0, state->Seconds}).ToMatrix();
                Mat4x4 s = Mat4x4::Scale(ModelScale);
                Mat4x4 modelMatrix = r1 * s;

                Mat4x4 r2 = (Euler{-45.0f * (float)Deg2Rad, 0, 0}).ToMatrix();
                Mat4x4 t2 = Mat4x4::Transform(0.0f, 0.0f, -4.0f);
                Mat4x4 viewMatrix = t2 * r2;

                float near = 0.01f;
                float far = 100.0f;
                Mat4x4 projectionMatrix = Mat4x4::Perspective(near, far, 60.0f * Deg2Rad, 640.0 / 480.0);

                state->Renderer->UpdateMVP(modelMatrix, viewMatrix, projectionMatrix);
                state->Renderer->Render(targetView);
            });
    }

    int LoadModel()
    {
        std::cout << "Hello, WebGPU!!" << std::endl;
        Application::Create(Start);
        return 0;
    }
} // namespace Pretop::Demos
