#include "../RHI/Application.hpp"
#include "../Asset/AssetManagerFactory.hpp"
#include "../Asset/AssetLoaderFactory.hpp"
#include "../Window/WindowFactory.hpp"

struct TextureDemoData
{
    Pretop::Core::JobSystem jobs;
    std::unique_ptr<Pretop::Asset::AssetManager> Assets;

    Pretop::Asset::AssetManager::Handle ShaderHandle;
    std::unique_ptr<Pretop::RHI::Shader> Shader;
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
    std::cout << "Succesfully loaded ShaderModule" << std::endl;
    state->ShaderLoaded = true;
}

void Start(Pretop::RHI::Application &application)
{
    TextureDemoData state;
    state.Assets = std::move(
        Pretop::Asset::AssetManagerFactory::CreateAssetManager(
            std::move(Pretop::Asset::AssetLoaderFactory::CreateAssetLoader(&state.jobs)),
            application.Device.get()));

    std::unique_ptr<Pretop::Window::Window> window = Pretop::Window::WindowFactory::CreateWindow("Texture");
    application.SetWindow(std::move(window));

    state.ShaderHandle = state.Assets->LoadShaderModule("Does-not-exist");

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