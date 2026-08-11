#include <array>

#include "Texture.hpp"
#include "../RHI/Application.hpp"
#include "../Asset/AssetManagerFactory.hpp"
#include "../Asset/AssetLoaderFactory.hpp"
#include "../Window/WindowFactory.hpp"
#include "../Math/Linalg/Mat4x4.hpp"
#include "../Math/Euler.hpp"
#include "../Math/Constants.hpp"

WGPUStringView wgpuStr(const char *data)
{
    size_t len = strlen(data);
    return {
        /*.data=*/data,
        /*.length=*/len,
    };
}

struct MyUniforms
{
    Pretop::Math::Mat4x4 ProjectionMatrix;
    Pretop::Math::Mat4x4 ViewMatrix;
    Pretop::Math::Mat4x4 ModelMatrix;
    float Color;
    float Time;
    float Pad[2];
};

struct TextureDemoData
{
    Pretop::RHI::Application *app;
    Pretop::Core::JobSystem jobs;
    std::unique_ptr<Pretop::Asset::AssetManager> Assets;

    Pretop::Asset::AssetManager::Handle ShaderHandle;
    std::unique_ptr<Pretop::RHI::Shader> Shader;
    WGPUBindGroupLayout bindGroupLayout;
    WGPURenderPipeline pipeline;
    bool ShaderLoaded = false;
};

void CreateImage(std::vector<uint8_t> &pixels, WGPUExtent3D &size)
{
    pixels.resize(4 * size.width * size.height);
    for (uint32_t i = 0; i < size.width; ++i)
    {
        for (uint32_t j = 0; j < size.height; ++j)
        {
            uint8_t *p = &pixels[4 * (j * size.width + i)];
            p[0] = (i / 16) % 2 == (j / 16) % 2 ? 255 : 0; // r
            p[1] = ((i - j) / 16) % 2 == 0 ? 255 : 0;      // g
            p[2] = ((i + j) / 16) % 2 == 0 ? 255 : 0;      // b
            p[3] = 255;                                    // a
        }
    }
}

void LoadShader(TextureDemoData *state)
{
    Pretop::Asset::AssetManager::Status shaderLoadStatus = state->Assets->GetState(state->ShaderHandle);
    if (shaderLoadStatus == Pretop::Asset::AssetManager::Status::InProgress)
    {
        return;
    }
    if (shaderLoadStatus == Pretop::Asset::AssetManager::Status::Error)
    {
        std::cout << state->Assets->GetError(state->ShaderHandle) << std::endl;
        exit(1);
    }
    state->Shader = std::move(state->Assets->GetShaderModule(state->ShaderHandle));
    std::string pipelineLayoutLabel = "Pipeline layout";
    WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/{
            /*.data=*/pipelineLayoutLabel.data(),
            /*.length=*/pipelineLayoutLabel.size(),
        },
        /*.bindGroupLayoutCount=*/1,
        /*.bindGroupLayouts=*/&state->bindGroupLayout,
        /*.immediateSize=*/0};
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(state->app->Device->WgpuDevice, &pipelineLayoutDesc);
    std::string vsEntryPoint = "vs_main";
    WGPUVertexAttribute vertexAttributes[2];
    vertexAttributes[0] = {
        /*.nextInChain=*/nullptr,
        /*.format=*/WGPUVertexFormat_Float32x3,
        /*.offset=*/0,
        /*.shaderLocation=*/0,
    };
    vertexAttributes[1] = {
        /*.nextInChain=*/nullptr,
        /*.format=*/WGPUVertexFormat_Float32x2,
        /*.offset=*/sizeof(float) * 3,
        /*.shaderLocation=*/1,
    };
    WGPUVertexBufferLayout vertexBufferLayout = {
        /*.nextInChain=*/nullptr,
        /*.stepMode=*/WGPUVertexStepMode_Vertex,
        /*.arrayStride=*/sizeof(float) * 5,
        /*.attributeCount=*/2,
        /*.attributes=*/vertexAttributes,
    };
    std::string fsEntryPoint = "fs_main";
    WGPUColorTargetState colorTargetState = {
        /*.nextInChain=*/nullptr,
        /*.format=*/state->app->WindowFormat,
        /*.blend=*/nullptr,
        /*.writeMask=*/WGPUColorWriteMask_All,
    };
    WGPUFragmentState fragmentState = {
        /*.nextInChain=*/nullptr,
        /*.module=*/state->Shader->WgpuShader,
        /*.entryPoint=*/{
            /*.data=*/fsEntryPoint.data(),
            /*.length=*/fsEntryPoint.size(),
        },
        /*.constantCount=*/0,
        /*.constants=*/nullptr,
        /*.targetCount=*/1,
        /*.targets=*/&colorTargetState,
    };
    WGPURenderPipelineDescriptor pipelineDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/WGPU_STRING_VIEW_INIT,
        /*.layout=*/pipelineLayout,
        /*.vertex=*/{
            /*.nextInChain=*/nullptr,
            /*.module=*/state->Shader->WgpuShader,
            /*.entryPoint=*/{
                /*.data=*/vsEntryPoint.data(),
                /*.length=*/vsEntryPoint.size(),
            },
            /*.constantCount=*/0,
            /*.constants=*/nullptr,
            /*.bufferCount=*/1,
            /*.buffers=*/&vertexBufferLayout,
        },
        /*.primitive=*/{
            /*.nextInChain=*/nullptr,
            /*.topology=*/WGPUPrimitiveTopology_TriangleList,
            /*.stripIndexFormat=*/WGPUIndexFormat_Undefined,
            /*.frontFace=*/WGPUFrontFace_CCW,
            /*.cullMode=*/WGPUCullMode_Back,
            /*.unclippedDepth=*/false,
        },
        /*.depthStencil=*/nullptr,
        /*.multisample=*/{
            /*.nextInChain=*/nullptr,
            /*.count=*/1,
            /*.mask=*/0xFFFFFFFF,
            /*.alphaToCoverageEnabled=*/false,
        },
        /*.fragment=*/&fragmentState,
    };
    state->pipeline = wgpuDeviceCreateRenderPipeline(state->app->Device->WgpuDevice, &pipelineDesc);
    std::cout << "Succesfully loaded ShaderModule" << std::endl;
    state->ShaderLoaded = true;
}

const float ModelScale = 1.3f;
void Start(Pretop::RHI::Application &application)
{
    TextureDemoData state;
    state.app = &application;
    state.Assets = std::move(
        Pretop::Asset::AssetManagerFactory::CreateAssetManager(
            std::move(Pretop::Asset::AssetLoaderFactory::CreateAssetLoader(&state.jobs)),
            application.Device.get()));

    std::unique_ptr<Pretop::Window::Window> window = Pretop::Window::WindowFactory::CreateWindow("Texture");
    application.SetWindow(std::move(window));

    state.ShaderHandle = state.Assets->LoadShaderModule("assets/shaders/textureDemoShader.wgsl");

    std::vector<float> vertices = {
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    WGPUBufferDescriptor vertexBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/wgpuStr("Vertex buffer"),
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        /*.size=*/vertices.size() * sizeof(float),
        /*.mappedAtCreation=*/false,
    };
    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &vertexBufferDesc);
    wgpuQueueWriteBuffer(application.WgpuQueue, vertexBuffer, 0, vertices.data(), vertices.size() * sizeof(float));

    std::vector<uint16_t> indices = {
        0, 2, 1,
        1, 2, 3,

        2, 0, 6,
        0, 4, 6,

        2, 6, 7,
        3, 2, 7,

        3, 5, 1,
        3, 7, 5,

        0, 5, 4,
        0, 1, 5,

        4, 5, 6,
        5, 7, 6};

    WGPUBufferDescriptor indexBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/wgpuStr("Index buffer"),
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        /*.size=*/indices.size() * sizeof(uint16_t),
        /*.mappedAtCreation=*/false,
    };
    WGPUBuffer indexBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &indexBufferDesc);
    wgpuQueueWriteBuffer(application.WgpuQueue, indexBuffer, 0, indices.data(), indices.size() * sizeof(uint16_t));

    WGPUBufferDescriptor uniformBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/wgpuStr("Uniform Buffer"),
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
        /*.size=*/sizeof(MyUniforms),
        /*.mappedAtCreation=*/false,
    };

    WGPUBuffer uniformBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &uniformBufferDesc);
    MyUniforms initialUniforms;
    initialUniforms.ModelMatrix = Pretop::Math::Mat4x4::Identity();
    initialUniforms.ViewMatrix = Pretop::Math::Mat4x4::Identity();
    initialUniforms.ProjectionMatrix = Pretop::Math::Mat4x4::Identity();
    initialUniforms.Color = 0.0f;
    initialUniforms.Time = 0.0f;
    wgpuQueueWriteBuffer(application.WgpuQueue, uniformBuffer, 0, &initialUniforms, sizeof(MyUniforms));

    std::vector<uint8_t> pixels;
    WGPUExtent3D size{/*.width=*/256, /*.height=*/256, /*.depthOrArrayLayers=*/1};
    CreateImage(pixels, size);

    WGPUTextureDescriptor textureDesc = WGPU_TEXTURE_DESCRIPTOR_INIT;
    textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    textureDesc.dimension = WGPUTextureDimension_2D;
    textureDesc.size = size;
    textureDesc.mipLevelCount = 1;
    textureDesc.sampleCount = 1;
    textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
    textureDesc.viewFormatCount = 0;
    textureDesc.viewFormats = nullptr;

    WGPUTexture texture = wgpuDeviceCreateTexture(application.Device->WgpuDevice, &textureDesc);
    WGPUTexelCopyTextureInfo destination = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = {/*.width=*/0, /*.height=*/0, /*.depthOrArrayLayers=*/0};
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout dataLayout = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
    dataLayout.offset = 0;
    dataLayout.bytesPerRow = 4 * size.width;
    dataLayout.rowsPerImage = size.height;

    wgpuQueueWriteTexture(application.WgpuQueue, &destination, pixels.data(), pixels.size(), &dataLayout, &size);

    std::array<WGPUBindGroupLayoutEntry, 2> bindingLayoutEntries = {WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT};
    WGPUBindGroupLayoutEntry &bindingLayout = bindingLayoutEntries[0];
    bindingLayout.binding = 0;
    bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = sizeof(MyUniforms);

    WGPUBindGroupLayoutEntry &textureBindingLayout = bindingLayoutEntries[1];
    textureBindingLayout.binding = 1;
    textureBindingLayout.visibility = WGPUShaderStage_Fragment;
    textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
    textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = (uint32_t)bindingLayoutEntries.size();
    bindGroupLayoutDesc.entries = bindingLayoutEntries.data();
    state.bindGroupLayout = wgpuDeviceCreateBindGroupLayout(application.Device->WgpuDevice, &bindGroupLayoutDesc);

    std::array<WGPUBindGroupEntry, 2> bindings = {WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT};
    bindings[0].binding = 0;
    bindings[0].buffer = uniformBuffer;
    bindings[0].offset = 0;
    bindings[0].size = sizeof(MyUniforms);

    WGPUTextureViewDescriptor textureViewDesc = WGPU_TEXTURE_VIEW_DESCRIPTOR_INIT;
    textureViewDesc.aspect = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc.format = textureDesc.format;
    WGPUTextureView textureView = wgpuTextureCreateView(texture, &textureViewDesc);

    bindings[1].binding = 1;
    bindings[1].textureView = textureView;

    WGPUBindGroupDescriptor bindGroupDesc{};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = state.bindGroupLayout;
    bindGroupDesc.entryCount = (uint32_t)bindings.size();
    bindGroupDesc.entries = bindings.data();
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(application.Device->WgpuDevice, &bindGroupDesc);

    MyUniforms uniforms;
    uniforms.ModelMatrix = Pretop::Math::Mat4x4::Identity();
    uniforms.ViewMatrix = Pretop::Math::Mat4x4::Identity();
    uniforms.ProjectionMatrix = Pretop::Math::Mat4x4::Identity();
    uniforms.Color = 0.0f;
    uniforms.Time = 0.0f;

    float seconds = 0.0f;

    application.Run(
        [&](
            double dt,
            WGPUTextureView targetView)
        {
            state.jobs.PumpMainThreadCompletions();
            if (!state.ShaderLoaded)
            {
                LoadShader(&state);
            }

            seconds += dt;
            MyUniforms u;
            u.Color = (sin(seconds * 2.32325) + 1) / 2;
            u.Time = seconds;

            Pretop::Math::Mat4x4 r1 = (Pretop::Math::Euler{0, 0, seconds}).ToMatrix();
            Pretop::Math::Mat4x4 s = Pretop::Math::Mat4x4::Scale(ModelScale);
            u.ModelMatrix = r1 * s;

            Pretop::Math::Mat4x4 r2 = (Pretop::Math::Euler{-45.0f * (float)Pretop::Math::Deg2Rad, 0, 0}).ToMatrix();
            Pretop::Math::Mat4x4 t2 = Pretop::Math::Mat4x4::Transform(0.0f, 0.0f, -4.0f);
            u.ViewMatrix = t2 * r2;

            float near = 0.01f;
            float far = 100.0f;
            u.ProjectionMatrix = Pretop::Math::Mat4x4::Perspective(near, far, 60.0f * Pretop::Math::Deg2Rad, 640.0 / 480.0);
            wgpuQueueWriteBuffer(
                application.WgpuQueue,
                uniformBuffer,
                0,
                &u,
                sizeof(MyUniforms));

            WGPUCommandEncoderDescriptor encoderDescriptor = {
                /*.nextInChain=*/nullptr,
                /*.label=*/wgpuStr("Texture command encoder"),
            };
            WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(application.Device->WgpuDevice, &encoderDescriptor);

            WGPURenderPassColorAttachment renderPassColorAttachment = {
                /*.nextInChain=*/nullptr,
                /*.view=*/targetView,
                /*.depthSlice=*/WGPU_DEPTH_SLICE_UNDEFINED,
                /*.resolveTarget=*/nullptr,
                /*.loadOp=*/WGPULoadOp_Clear,
                /*.storeOp=*/WGPUStoreOp_Store,
                /*.clearValue=*/WGPUColor{0.2, 0.2, 0.2, 1.0},
            };
            WGPURenderPassDescriptor renderPassDescriptor = {
                /*.nextInChain=*/nullptr,
                /*.label=*/wgpuStr("Texture render pass"),
                /*.colorAttachmentCount=*/1,
                /*.colorAttachments=*/&renderPassColorAttachment,
                /*.depthStencilAttachment=*/nullptr,
                /*.occlusionQuerySet=*/nullptr,
                /*.timestampWrites=*/nullptr,
            };
            WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);

            wgpuRenderPassEncoderSetPipeline(renderPass, state.pipeline);
            wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, wgpuBufferGetSize(vertexBuffer));
            wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(indexBuffer));
            wgpuRenderPassEncoderSetBindGroup(renderPass, 0, bindGroup, 0, nullptr);
            wgpuRenderPassEncoderDrawIndexed(renderPass, indices.size(), 1, 0, 0, 0);
            wgpuRenderPassEncoderEnd(renderPass);
            wgpuRenderPassEncoderRelease(renderPass);

            WGPUCommandBufferDescriptor cmdBufferDescriptor = {
                /*.nextInChain=*/nullptr,
                /*.label=*/wgpuStr("Texture command buffer"),
            };
            WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);

            wgpuQueueSubmit(application.WgpuQueue, 1, &commandBuffer);

            wgpuCommandEncoderRelease(encoder);
        });

    wgpuBufferDestroy(uniformBuffer);
    wgpuBufferRelease(uniformBuffer);

    wgpuTextureDestroy(texture);
    wgpuTextureRelease(texture);
}

namespace Pretop::Demos
{
    int Texture()
    {
        Pretop::RHI::Application::Create(Start);
        return 0;
    }
}