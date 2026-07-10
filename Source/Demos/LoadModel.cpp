#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>
#include "../PrintStringView.hpp"

#include "../Window/WindowFactory.hpp"

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

#include "../Asset/LoadGlb.hpp"
#include "../Asset/ModelParseError.hpp"
#include "../Asset/AssetLoaderFactory.hpp"

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
    void Start(Application &application)
    {
        std::unique_ptr<AssetLoader> assetLoader = AssetLoaderFactory::CreateAssetLoader();

        ParsedData model;
        try
        {
            model = LoadGlb(assetLoader.get(), "assets/models/woolly-mammoth-100k-4096_std.glb");
            std::cout << "Success!" << std::endl;
        }
        catch (ModelParseError &e)
        {
            std::cout << e.what() << std::endl;
            exit(1);
        }

        application.LogQueueCommands();
        application.SetWindow(WindowFactory::CreateWindow("My Window"));

        Shader shader = Shader::Pipeline(application.Device.get(), application.Instance.get(), assetLoader.get(), "shaders/shader.wgsl");

        Buffer vertices(application.Device.get(), model.Vertices.size() * sizeof(Vertex), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
        application.WriteVec(vertices, model.Vertices);

        uint32_t indicesSize = 0;
        bool is32bitIndexBuffer = std::holds_alternative<std::vector<uint32_t>>(model.Indices);
        if (is32bitIndexBuffer)
        {
            std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.Indices);
            indicesSize = indicesVec.size() * sizeof(uint32_t);
        }
        else
        {
            std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.Indices);
            indicesSize = indicesVec.size() * sizeof(uint16_t);
        }
        Buffer indices(application.Device.get(), indicesSize, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index);
        if (is32bitIndexBuffer)
        {
            std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.Indices);
            application.WriteVec(indices, indicesVec);
        }
        else
        {
            std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.Indices);
            application.WriteVec(indices, indicesVec);
        }
        VertexLayout vertexLayout = {{LayoutType::Float32x3, LayoutType::Float32x3}};

        WGPUBindGroupLayoutEntry bindingLayoutEntry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
        bindingLayoutEntry.binding = 0;
        bindingLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
        bindingLayoutEntry.buffer.minBindingSize = sizeof(MyUniforms);
        bindingLayoutEntry.buffer.hasDynamicOffset = true;
        bindingLayoutEntry.visibility = WGPUShaderStage_Vertex;
        BindingLayout bindingLayout(application.Device.get(), {{bindingLayoutEntry}});

        Pipeline pipeline(application.Device.get(), shader, application.WindowFormat, vertexLayout, bindingLayout);
        Buffer myUniformBuffer(application.Device.get(), sizeof(MyUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

        std::vector<WGPUBindGroupEntry> bindings = {WGPU_BIND_GROUP_ENTRY_INIT};
        bindings[0].binding = 0;
        bindings[0].buffer = myUniformBuffer.WgpuBuffer;
        bindings[0].size = sizeof(MyUniforms);
        BindGroup bindGroup(application.Device.get(), bindingLayout.WgpuBindGroupLayouts[0], bindings);
        application.WriteBufZero(myUniformBuffer);

        float seconds = 0;
        application.Run([&](
                            double dt,
                            WGPUTextureView targetView)
                        {
                        seconds += dt;
                        MyUniforms u;
                        u.Color = (sin(seconds * 2.32325) + 1) / 2;
                        u.Time = seconds;

                        Mat4x4 r1 = (Euler{90.0f * (float)Deg2Rad, 0, seconds}).ToMatrix();
                        Mat4x4 s = Mat4x4::Scale(ModelScale);
                        u.ModelMatrix = r1 * s;

                        Mat4x4 r2 = (Euler{-45.0f * (float)Deg2Rad, 0, 0}).ToMatrix();
                        Mat4x4 t2 = Mat4x4::Transform(0.0f, 0.0f, -4.0f);
                        u.ViewMatrix = t2 * r2;

                        float near = 0.01f;
                        float far = 100.0f;
                        u.ProjectionMatrix = Mat4x4::Perspective(near, far, 60.0f * Deg2Rad, 640.0 / 480.0);
                        application.WriteBuf(myUniformBuffer, u);

                        CommandBuffer commandBuffer(application.Device.get());
                        std::cout << "DELTATIME: " << dt << std::endl;
                        RenderPassCommand command(application.Device.get(), targetView, pipeline.WgpuDepthStencilAttachment);
                        std::vector<Buffer *> bufs = {&vertices};

                        auto cmd = commandBuffer.AddCommand(command);
                        cmd->SetPipeline(pipeline)
                        .SetVertexBuffers(bufs)
                        .SetBindGroup(&bindGroup, 0, {0});
                        if (is32bitIndexBuffer)
                        {
                            std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.Indices);
                            cmd->DrawIndexed(indices, indicesVec.size(), WGPUIndexFormat_Uint32);
                        }
                        else
                        {
                            std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.Indices);
                            cmd->DrawIndexed(indices, indicesVec.size(), WGPUIndexFormat_Uint16);
                        }
                        cmd->Finish();

                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.Finish();
                        application.SubmitCommandBuffer(commandBuffer);

                        return commandBuffer; });
    }

    int LoadModel()
    {
        std::cout << "Hello, WebGPU!!" << std::endl;
        Application::Create(Start);
        return 0;
    }
} // namespace Pretop::Demos
