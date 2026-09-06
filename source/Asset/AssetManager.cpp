#include "AssetManager.hpp"

#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"

#include <stb_image.h>

#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace Pretop::Asset
{
    struct AssetManagerData
    {
        virtual ~AssetManagerData() = default;
    };

    AssetManager::AssetManager(
        std::unique_ptr<AssetLoader> assetLoader,
        Core::GraphicsContext graphicsContext)
        : _assetLoader(std::move(assetLoader)), _graphicsContext(graphicsContext)
    {
    }

    AssetManager::~AssetManager() = default;

    struct LoadModelData : AssetManagerData
    {
        std::unique_ptr<ParsedData> data;
    };
    AssetManager::Handle AssetManager::LoadModel(std::string_view path)
    {
        LoadModelData *loadModelData = new LoadModelData;
        loadModelData->data = nullptr;
        return _assetLoader->ReadFile(
            path,
            [](const AssetLoader::AssetBytes &bytes, void *userData)
            {
                LoadModelData *data = reinterpret_cast<LoadModelData *>(userData);
                data->data = std::make_unique<ParsedData>(LoadGlb(bytes));
            },
            [](AssetLoader &loader, AssetLoader::Handle handle) {},
            loadModelData);
    }

    AssetManager::Status AssetManager::GetState(Handle handle)
    {
        return _assetLoader->GetStatus(handle);
    }

    std::unique_ptr<ParsedData> AssetManager::GetGlbData(Handle handle)
    {
        LoadModelData *loadModelData = reinterpret_cast<LoadModelData *>(_assetLoader->GetRawData(handle));
        return std::move(loadModelData->data);
    }

    struct LoadShaderModuleData : AssetManagerData
    {
        std::unique_ptr<RHI::Shader> data;
        Core::GraphicsContext context;
    };
    AssetManager::Handle AssetManager::LoadShaderModule(std::string_view path)
    {
        LoadShaderModuleData *loadShaderModuleData = new LoadShaderModuleData;
        loadShaderModuleData->data = nullptr;
        loadShaderModuleData->context = this->_graphicsContext;
        return _assetLoader->ReadFile(
            path,
            [](AssetLoader &loader, AssetManager::Handle handle)
            {
                AssetLoader::AssetBytes bytes = loader.GetBytes(handle);
                LoadShaderModuleData *data = reinterpret_cast<LoadShaderModuleData *>(loader.GetRawData(handle));
                data->data = std::make_unique<RHI::Shader>(
                    RHI::Shader::Pipeline(data->context, bytes.data(), bytes.size()));
            },
            loadShaderModuleData);
    }

    std::unique_ptr<RHI::Shader> AssetManager::GetShaderModule(Handle handle)
    {
        LoadShaderModuleData *loadShaderModuleData = reinterpret_cast<LoadShaderModuleData *>(_assetLoader->GetRawData(handle));
        return std::move(loadShaderModuleData->data);
    }

    struct LoadTextureData
    {
        int width;
        int height;
        int channels;
        unsigned char *pixelData;
        WGPUTexture texture;
        Core::GraphicsContext graphicsContext;
        // RHI::Application *application;
    };
    AssetManager::Handle AssetManager::LoadTexture(std::string_view path)
    {
        LoadTextureData *loadTextureData = new LoadTextureData;
        loadTextureData->graphicsContext = this->_graphicsContext;
        // loadTextureData->application = this->_application;
        return _assetLoader->ReadFile(
            path,
            [](const AssetLoader::AssetBytes &bytes, void *userData)
            {
                LoadTextureData *data = reinterpret_cast<LoadTextureData *>(userData);
                data->pixelData = stbi_load_from_memory(
                    reinterpret_cast<const unsigned char *>(bytes.data()),
                    bytes.size(),
                    &data->width,
                    &data->height,
                    &data->channels,
                    4);
            },
            [](AssetLoader &loader, AssetLoader::Handle handle)
            {
                LoadTextureData *data = reinterpret_cast<LoadTextureData *>(loader.GetRawData(handle));

                WGPUTextureDescriptor textureDesc = WGPU_TEXTURE_DESCRIPTOR_INIT;
                textureDesc.nextInChain = nullptr;
                textureDesc.dimension = WGPUTextureDimension_2D;
                textureDesc.format = WGPUTextureFormat_RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
                textureDesc.mipLevelCount = 1;
                textureDesc.sampleCount = 1;
                textureDesc.size = {(unsigned int)data->width, (unsigned int)data->height, 1};
                textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
                textureDesc.viewFormatCount = 0;
                textureDesc.viewFormats = nullptr;
                WGPUTexture texture = wgpuDeviceCreateTexture(data->graphicsContext.Device, &textureDesc);

                WGPUTexelCopyTextureInfo destination = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
                destination.texture = texture;
                destination.mipLevel = 0;
                destination.origin = {/*.width=*/0, /*.height=*/0, /*.depthOrArrayLayers=*/0};
                destination.aspect = WGPUTextureAspect_All;

                WGPUTexelCopyBufferLayout dataLayout = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
                dataLayout.offset = 0;
                dataLayout.bytesPerRow = 4 * data->width;
                dataLayout.rowsPerImage = data->height;

                WGPUExtent3D size{/*.width=*/static_cast<uint32_t>(data->width),
                                  /*.height=*/static_cast<uint32_t>(data->height),
                                  /*.depthOrArrayLayers=*/1};

                wgpuQueueWriteTexture(
                    data->graphicsContext.Queue,
                    &destination,
                    data->pixelData,
                    4 * data->width * data->height,
                    &dataLayout,
                    &size);

                stbi_image_free(data->pixelData);

                data->texture = texture;
            },
            loadTextureData);
    }

    std::unique_ptr<GPUTexture> AssetManager::GetTexture(Handle handle)
    {
        LoadTextureData *data = reinterpret_cast<LoadTextureData *>(this->_assetLoader->GetRawData(handle));
        std::unique_ptr<GPUTexture> texture = std::make_unique<GPUTexture>(data->texture);
        return std::move(texture);
    }

    std::string AssetManager::GetError(Handle handle)
    {
        return _assetLoader->GetError(handle);
    }
    void AssetManager::Release(Handle handle)
    {
        delete static_cast<AssetManagerData *>(_assetLoader->GetRawData(handle));
        return _assetLoader->Release(handle);
    }
} // namespace Pretop::Asset
