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

    AssetManager::AssetReference AssetManager::LoadModel(
        AssetId assetId)
    {
        auto it = _assets.find(assetId);
        if (it != _assets.end())
        {
            AssetEntry &existingEntry = it->second;
            existingEntry.RefCount++;

            return _addReferenceRecord(assetId);
        }

        LoadModelData *loadModelData = new LoadModelData;
        loadModelData->data = nullptr;
        AssetLoader::Handle loaderHandle = _assetLoader->ReadFile(
            assetId,
            [](const AssetLoader::AssetBytes &bytes, void *userData)
            {
                LoadModelData *data = reinterpret_cast<LoadModelData *>(userData);
                data->data = std::make_unique<ParsedData>(LoadGlb(bytes));
            },
            [](AssetLoader &loader, AssetLoader::Handle handle) {},
            loadModelData);

        _assets.emplace(assetId,
                        AssetEntry{
                            /* Type */ AssetType::GLB,
                            /* LoaderHandle */ loaderHandle,
                            /* RefCount */ 0,
                        });

        return _addReferenceRecord(assetId);
    }

    AssetManager::AssetResult AssetManager::GetGlbData(AssetReference reference, ParsedData **data)
    {
        void *rawDataPtr = nullptr;
        AssetType type = AssetType::Undefined;
        AssetResult result = this->_getData(reference, &type, &rawDataPtr);
        LoadModelData *loadModelData = reinterpret_cast<LoadModelData *>(rawDataPtr);
        if (type != AssetType::GLB)
        {
            return AssetResult::WrongType;
        }
        if (result != AssetResult::Success)
        {
            return result;
        }

        *data = loadModelData->data.get();
        return AssetResult::Success;
    };

    struct LoadShaderModuleData : AssetManagerData
    {
        std::unique_ptr<RHI::Shader> data;
        Core::GraphicsContext context;
    };
    AssetManager::AssetReference AssetManager::LoadShaderModule(AssetId assetId)
    {
        auto it = _assets.find(assetId);
        if (it != _assets.end())
        {
            AssetEntry &existingEntry = it->second;
            existingEntry.RefCount++;

            return _addReferenceRecord(assetId);
        }

        LoadShaderModuleData *loadShaderModuleData = new LoadShaderModuleData;
        loadShaderModuleData->data = nullptr;
        loadShaderModuleData->context = this->_graphicsContext;
        AssetLoader::Handle loaderHandle = _assetLoader->ReadFile(
            assetId,
            [](AssetLoader &loader, AssetLoader::Handle handle)
            {
                AssetLoader::AssetBytes bytes = loader.GetBytes(handle);
                LoadShaderModuleData *data = reinterpret_cast<LoadShaderModuleData *>(loader.GetRawData(handle));
                data->data = std::make_unique<RHI::Shader>(
                    RHI::Shader::Pipeline(data->context, bytes.data(), bytes.size()));
            },
            loadShaderModuleData);

        _assets.emplace(assetId,
                        AssetEntry{
                            /* Type */ AssetType::Shader,
                            /* LoaderHandle */ loaderHandle,
                            /* RefCount */ 0,
                        });

        return _addReferenceRecord(assetId);
    }

    AssetManager::AssetResult AssetManager::GetShaderModule(AssetReference reference, RHI::Shader **shader)
    {
        void *rawDataPtr = nullptr;
        AssetType type = AssetType::Undefined;
        AssetResult result = this->_getData(reference, &type, &rawDataPtr);
        LoadShaderModuleData *loadShaderModuleData = reinterpret_cast<LoadShaderModuleData *>(rawDataPtr);
        if (type != AssetType::Shader)
        {
            return AssetResult::WrongType;
        }
        if (result != AssetResult::Success)
        {
            return result;
        }

        *shader = loadShaderModuleData->data.get();
        return AssetResult::Success;
    }

    struct LoadTextureData
    {
        int width;
        int height;
        int channels;
        unsigned char *pixelData;
        WGPUTexture texture;
        Core::GraphicsContext graphicsContext;
    };
    AssetManager::AssetReference AssetManager::LoadTexture(AssetId assetId)
    {
        auto it = _assets.find(assetId);
        if (it != _assets.end())
        {
            AssetEntry &existingEntry = it->second;
            existingEntry.RefCount++;

            return _addReferenceRecord(assetId);
        }

        LoadTextureData *loadTextureData = new LoadTextureData;
        loadTextureData->graphicsContext = this->_graphicsContext;
        AssetLoader::Handle loaderHandle = _assetLoader->ReadFile(
            assetId,
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

        _assets.emplace(assetId,
                        AssetEntry{
                            /* Type */ AssetType::Texture,
                            /* LoaderHandle */ loaderHandle,
                            /* RefCount */ 0,
                        });

        return _addReferenceRecord(assetId);
    }

    AssetManager::AssetResult AssetManager::GetTexture(AssetReference reference, WGPUTexture **texture)
    {
        void *rawDataPtr = nullptr;
        AssetType type = AssetType::Undefined;
        AssetResult result = this->_getData(reference, &type, &rawDataPtr);
        LoadTextureData *loadTextureData = reinterpret_cast<LoadTextureData *>(rawDataPtr);
        if (type != AssetType::Texture)
        {
            return AssetResult::WrongType;
        }
        if (result != AssetResult::Success)
        {
            return result;
        }

        *texture = &loadTextureData->texture;
        return AssetResult::Success;
    }

    AssetManager::AssetResult AssetManager::GetResult(AssetReference reference)
    {
        return _getData(reference, nullptr, nullptr);
    }

    std::string AssetManager::GetError(AssetReference reference)
    {
        // AssetManager::Asset return _assetLoader->GetError(handle);
        return "TEST STRING FOR NOW"; // Todo: implement this
    }

    void AssetManager::Release(AssetReference reference)
    {
        if (!_records.IsValid(reference.handle))
        {
            return;
        }
        AssetReferenceRecord *assetReferenceRecord = _records[reference.handle];

        auto it = _assets.find(assetReferenceRecord->AssetId);
        if (it == _assets.end())
        {
            return;
        }

        AssetEntry &assetEntry = it->second;
        if (assetEntry.RefCount == 0)
        {
            return;
        }

        assetEntry.RefCount--;
        if (assetEntry.RefCount == 0)
        {
            _assetLoader->Release(assetEntry.LoaderHandle);
        }
        return _records.Release(reference.handle);
    }

    AssetManager::AssetReference AssetManager::_addReferenceRecord(AssetId assetId)
    {
        AssetReferenceRecord assetReferenceRecord;
        assetReferenceRecord.AssetId = assetId;
        Core::Handle assetReferenceHandle = _records.Add(assetReferenceRecord);

        AssetReference assetReference;
        assetReference.handle = assetReferenceHandle;
        return assetReference;
    };

    AssetManager::AssetResult AssetManager::_getData(AssetReference reference, AssetType *type, void **data)
    {
        AssetLoader::Handle loaderHandle;
        AssetManager::AssetResult result = _getLoaderHandle(reference, type, &loaderHandle);

        if (result != AssetResult::Success)
        {
            return result;
        }

        // void *rawData = this->_assetLoader->GetRawData(loaderHandle);
        void *rawData = this->_assetLoader->GetRawData(loaderHandle);
        if (data != nullptr)
        {
            *data = rawData;
        }

        return AssetResult::Success;
    }

    AssetManager::AssetResult AssetManager::_getLoaderHandle(
        AssetReference reference, AssetType *type, AssetLoader::Handle *handle)
    {
        if (!_records.IsValid(reference.handle))
        {
            return AssetResult::InvalidHandle;
        }
        AssetReferenceRecord *assetReferenceRecord = _records[reference.handle];

        auto it = _assets.find(assetReferenceRecord->AssetId);
        if (it == _assets.end())
        {
            return AssetResult::InvalidHandle;
        }

        AssetEntry &assetEntry = it->second;
        if (type != nullptr)
        {
            *type = assetEntry.Type;
        }
        *handle = assetEntry.LoaderHandle;

        AssetLoader::Status assetLoadStatus =
            this->_assetLoader->GetStatus(assetEntry.LoaderHandle);

        if (assetLoadStatus == AssetLoader::Status::Error)
        {
            return AssetResult::Failed;
        }
        if (assetLoadStatus == AssetLoader::Status::InProgress)
        {
            return AssetResult::NotReady;
        }

        return AssetResult::Success;
    }
} // namespace Pretop::Asset
