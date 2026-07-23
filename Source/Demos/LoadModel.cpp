// #include <iostream>
// #include <unordered_map>
// #include <vector>
// #include <memory>
// #include <chrono>
// #define _USE_MATH_DEFINES
// #include <cmath>
// #include "../PrintStringView.hpp"

// #include "../Window/WindowFactory.hpp"

// #include "../RHI/Application.hpp"
// #include "../RHI/Device.hpp"
// #include "../RHI/Shader.hpp"
// #include "../RHI/Pipeline.hpp"
// #include "../RHI/CommandBuffer.hpp"
// #include "../RHI/RenderPassCommand.hpp"
// #include "../RHI/VertexLayout.hpp"
// #include "../RHI/BindingLayout.hpp"
// #include "../RHI/Buffer.hpp"
// #include "../RHI/BindGroup.hpp"

// #include "../Math/CeilToBufferOffset.hpp"
// #include "../Math/Linalg/Mat4x4.hpp"
// #include "../Math/Constants.hpp"
// #include "../Math/Euler.hpp"

// #include "../Asset/AssetLoaderFactory.hpp"
// #include "../Asset/AssetManagerFactory.hpp"
// #include "../Core/JobSystem.hpp"

namespace Pretop::Demos
{
    // using namespace Pretop::Asset;
    // using namespace Pretop::Math;
    // using namespace Pretop::RHI;
    // using namespace Pretop::Window;

    // struct MyUniforms
    // {
    //     Mat4x4 ProjectionMatrix;
    //     Mat4x4 ViewMatrix;
    //     Mat4x4 ModelMatrix;
    //     float Color;
    //     float Time;
    //     float Pad[2];
    // };

    // const float ModelScale = 0.5f;

    // struct DemoState
    // {
    //     Core::JobSystem Jobs;
    //     std::unique_ptr<AssetManager> Assets;
    //     std::unique_ptr<AssetLoader> ShaderLoader;
    //     AssetHandle<ParsedData> ModelHandle;
    //     ParsedData Model;

    //     std::unique_ptr<Buffer> Vertices;
    //     std::unique_ptr<Buffer> Indices;
    //     std::unique_ptr<BindingLayout> ModelBindingLayout;
    //     std::unique_ptr<Pipeline> ModelPipeline;
    //     std::unique_ptr<Buffer> UniformBuffer;
    //     std::unique_ptr<BindGroup> ModelBindGroup;

    //     uint32_t IndexCount = 0;
    //     WGPUIndexFormat IndexFormat = WGPUIndexFormat_Undefined;
    //     float Seconds = 0.0f;
    //     bool ModelLoadHandled = false;
    //     bool Ready = false;
    // };

    // void InitializeModel(Application &application, DemoState &state)
    // {
    //     state.ModelLoadHandled = true;
    //     auto result = state.ModelHandle.Get();
    //     if (!result)
    //     {
    //         std::cerr << "Failed to load model: " << result.Error << std::endl;
    //         return;
    //     }

    //     state.Model = std::move(result.Data);

    //     try
    //     {
    //         Shader shader = Shader::Pipeline(
    //             application.Device.get(),
    //             application.Instance.get(),
    //             state.ShaderLoader.get(),
    //             "shaders/shader.wgsl");

    //         state.Vertices = std::make_unique<Buffer>(
    //             application.Device.get(),
    //             state.Model.Vertices.size() * sizeof(Vertex),
    //             WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    //         application.WriteVec(*state.Vertices, state.Model.Vertices);

    //         size_t indicesSize = 0;
    //         if (std::holds_alternative<std::vector<uint32_t>>(state.Model.Indices))
    //         {
    //             auto &indices = std::get<std::vector<uint32_t>>(state.Model.Indices);
    //             indicesSize = indices.size() * sizeof(uint32_t);
    //             state.IndexCount = static_cast<uint32_t>(indices.size());
    //             state.IndexFormat = WGPUIndexFormat_Uint32;
    //         }
    //         else
    //         {
    //             auto &indices = std::get<std::vector<uint16_t>>(state.Model.Indices);
    //             indicesSize = indices.size() * sizeof(uint16_t);
    //             state.IndexCount = static_cast<uint32_t>(indices.size());
    //             state.IndexFormat = WGPUIndexFormat_Uint16;
    //         }

    //         state.Indices = std::make_unique<Buffer>(
    //             application.Device.get(),
    //             indicesSize,
    //             WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index);
    //         if (state.IndexFormat == WGPUIndexFormat_Uint32)
    //         {
    //             auto &indices = std::get<std::vector<uint32_t>>(state.Model.Indices);
    //             application.WriteVec(*state.Indices, indices);
    //         }
    //         else
    //         {
    //             auto &indices = std::get<std::vector<uint16_t>>(state.Model.Indices);
    //             application.WriteVec(*state.Indices, indices);
    //         }

    //         VertexLayout vertexLayout = {{LayoutType::Float32x3, LayoutType::Float32x3}};

    //         WGPUBindGroupLayoutEntry bindingLayoutEntry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    //         bindingLayoutEntry.binding = 0;
    //         bindingLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
    //         bindingLayoutEntry.buffer.minBindingSize = sizeof(MyUniforms);
    //         bindingLayoutEntry.buffer.hasDynamicOffset = true;
    //         bindingLayoutEntry.visibility = WGPUShaderStage_Vertex;

    //         state.ModelBindingLayout = std::make_unique<BindingLayout>(
    //             application.Device.get(),
    //             std::initializer_list<std::initializer_list<WGPUBindGroupLayoutEntry>>{
    //                 {bindingLayoutEntry}});
    //         state.ModelPipeline = std::make_unique<Pipeline>(
    //             application.Device.get(),
    //             shader,
    //             application.WindowFormat,
    //             vertexLayout,
    //             *state.ModelBindingLayout);
    //         state.UniformBuffer = std::make_unique<Buffer>(
    //             application.Device.get(),
    //             sizeof(MyUniforms),
    //             WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

    //         std::vector<WGPUBindGroupEntry> bindings = {WGPU_BIND_GROUP_ENTRY_INIT};
    //         bindings[0].binding = 0;
    //         bindings[0].buffer = state.UniformBuffer->WgpuBuffer;
    //         bindings[0].size = sizeof(MyUniforms);
    //         state.ModelBindGroup = std::make_unique<BindGroup>(
    //             application.Device.get(),
    //             state.ModelBindingLayout->WgpuBindGroupLayouts[0],
    //             bindings);
    //         application.WriteBufZero(*state.UniformBuffer);

    //         state.Ready = true;
    //         std::cout << "Model loaded successfully." << std::endl;
    //     }
    //     catch (const std::exception &error)
    //     {
    //         std::cerr << "Failed to initialize model rendering: " << error.what() << std::endl;
    //     }
    // }

    // void Start(Application &application)
    // {
    //     auto state = std::make_shared<DemoState>();
    //     state->Assets = AssetManagerFactory::CreateAssetManager(state->Jobs);
    //     state->ShaderLoader = AssetLoaderFactory::CreateAssetLoader();
    //     state->ModelHandle = state->Assets->LoadModelAsync(
    //         "assets/models/woolly-mammoth-100k-4096_std.glb");

    //     application.LogQueueCommands();
    //     application.SetWindow(WindowFactory::CreateWindow("My Window"));

    //     application.Run([state, &application](
    //                         double dt,
    //                         WGPUTextureView targetView)
    //                     {
    //                     if (!state->ModelLoadHandled && state->ModelHandle.Ready())
    //                     {
    //                         InitializeModel(application, *state);
    //                     }

    //                     if (!state->Ready)
    //                     {
    //                         return;
    //                     }

    //                     state->Seconds += dt;
    //                     MyUniforms u;
    //                     u.Color = (sin(state->Seconds * 2.32325) + 1) / 2;
    //                     u.Time = state->Seconds;

    //                     Mat4x4 r1 = (Euler{90.0f * (float)Deg2Rad, 0, state->Seconds}).ToMatrix();
    //                     Mat4x4 s = Mat4x4::Scale(ModelScale);
    //                     u.ModelMatrix = r1 * s;

    //                     Mat4x4 r2 = (Euler{-45.0f * (float)Deg2Rad, 0, 0}).ToMatrix();
    //                     Mat4x4 t2 = Mat4x4::Transform(0.0f, 0.0f, -4.0f);
    //                     u.ViewMatrix = t2 * r2;

    //                     float near = 0.01f;
    //                     float far = 100.0f;
    //                     u.ProjectionMatrix = Mat4x4::Perspective(near, far, 60.0f * Deg2Rad, 640.0 / 480.0);
    //                     application.WriteBuf(*state->UniformBuffer, u);

    //                     CommandBuffer commandBuffer(application.Device.get());
    //                     RenderPassCommand command(
    //                         application.Device.get(),
    //                         targetView,
    //                         state->ModelPipeline->WgpuDepthStencilAttachment);
    //                     std::vector<Buffer *> bufs = {state->Vertices.get()};

    //                     auto cmd = commandBuffer.AddCommand(command);
    //                     cmd->SetPipeline(*state->ModelPipeline)
    //                     .SetVertexBuffers(bufs)
    //                     .SetBindGroup(state->ModelBindGroup.get(), 0, {0})
    //                     .DrawIndexed(*state->Indices, state->IndexCount, state->IndexFormat);
    //                     cmd->Finish();

    //                     commandBuffer.Finish();
    //                     application.SubmitCommandBuffer(commandBuffer);
    //                     });
    // }

    int LoadModel()
    {
        // std::cout << "Hello, WebGPU!!" << std::endl;
        // Application::Create(Start);
        return 0;
    }
} // namespace Pretop::Demos
