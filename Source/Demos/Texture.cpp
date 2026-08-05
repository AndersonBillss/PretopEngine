#include <array>

#include "Texture.hpp"
#include "../RHI/Application.hpp"
#include "../Asset/AssetManagerFactory.hpp"
#include "../Asset/AssetLoaderFactory.hpp"
#include "../Window/WindowFactory.hpp"
#include "../Math/Linalg/Mat4x4.hpp"

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
            p[0] = (uint8_t)i; // r
            p[1] = (uint8_t)j; // g
            p[2] = 128;        // b
            p[3] = 255;        // a
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

    std::cout << "Succesfully loaded ShaderModule" << std::endl;
    state->ShaderLoaded = true;
}

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

    state.ShaderHandle = state.Assets->LoadShaderModule("Does-not-exist");

    std::vector<float> vertices = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f};

    std::string vertexBufferLabel = "Vertex buffer";
    WGPUBufferDescriptor vertexBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/{
            /*.data=*/vertexBufferLabel.data(),
            /*.length=*/vertexBufferLabel.size(),
        },
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        /*.size=*/vertices.size() * sizeof(float),
        /*.mappedAtCreation=*/false,
    };
    WGPUBuffer vertexBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &vertexBufferDesc);
    wgpuQueueWriteBuffer(application.WgpuQueue, vertexBuffer, 0, vertices.data(), vertices.size() * sizeof(float));

    std::vector<uint16_t> indices = {
        0, 1, 2,
        1, 3, 2};

    std::string indexBufferLabel = "Index buffer";
    WGPUBufferDescriptor indexBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/{
            /*.data=*/indexBufferLabel.data(),
            /*.length=*/indexBufferLabel.size(),
        },
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index,
        /*.size=*/indices.size() * sizeof(uint16_t),
        /*.mappedAtCreation=*/false,
    };
    WGPUBuffer indexBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &indexBufferDesc);
    wgpuQueueWriteBuffer(application.WgpuQueue, indexBuffer, 0, indices.data(), indices.size() * sizeof(uint16_t));

    std::string uniformBufferLabel = "Uniform buffer";
    WGPUBufferDescriptor uniformBufferDesc = {
        /*.nextInChain=*/nullptr,
        /*.label=*/{
            /*.data=*/uniformBufferLabel.data(),
            /*.length=*/uniformBufferLabel.size(),
        },
        /*.usage=*/WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform,
        /*.size=*/sizeof(MyUniforms),
        /*.mappedAtCreation=*/false,
    };

    WGPUBuffer uniformBuffer = wgpuDeviceCreateBuffer(application.Device->WgpuDevice, &uniformBufferDesc);
    MyUniforms initialUniforms = {0};
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

    std::array<WGPUBindGroupLayoutEntry, 2> bindingLayoutEntries;
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

    std::array<WGPUBindGroupEntry, 2> bindings;
    bindings[0].binding = 0;
    bindings[0].buffer = uniformBuffer;
    bindings[0].offset = 0;
    bindings[0].size = sizeof(MyUniforms);

    WGPUTextureViewDescriptor textureViewDesc;
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

    application.Run(
        [&state, &application](
            double dt,
            WGPUTextureView targetView)
        {
            state.jobs.PumpMainThreadCompletions();
            if (!state.ShaderLoaded)
            {
                LoadShader(&state);
            }
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