#include "AssetManager.hpp"

#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"
#include "AssetBuilders/GlbAssetBuilder.hpp"
#include "AssetBuilders/ShaderModuleAssetBuilder.hpp"
#include "AssetBuilders/TextureAssetBuilder.hpp"

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

    AssetManager::OldAssetReference AssetManager::LoadModel(
        AssetId assetId)
    {
        std::unique_ptr<GlbAssetBuilder> builder = std::make_unique<GlbAssetBuilder>();
        return this->LoadAsset(assetId, std::move(builder));
    }

    AssetManager::AssetResult AssetManager::GetGlbData(OldAssetReference reference, ParsedData **data)
    {
        return this->GetAsset(reference, AssetType::GLB, (void **)data);
    };

    AssetManager::OldAssetReference AssetManager::LoadShaderModule(AssetId assetId)
    {
        std::unique_ptr<ShaderModuleAssetBuilder> builder = std::make_unique<ShaderModuleAssetBuilder>(this->_graphicsContext);
        return this->LoadAsset(assetId, std::move(builder));
    }

    AssetManager::AssetResult AssetManager::GetShaderModule(OldAssetReference reference, RHI::Shader **shader)
    {
        return this->GetAsset(reference, AssetType::Shader, (void **)shader);
    }

    AssetManager::OldAssetReference AssetManager::LoadTexture(AssetId assetId)
    {
        std::unique_ptr<TextureAssetBuilder> builder = std::make_unique<TextureAssetBuilder>(this->_graphicsContext);
        return this->LoadAsset(assetId, std::move(builder));
    }

    AssetManager::AssetResult AssetManager::GetTexture(OldAssetReference reference, WGPUTexture **texture)
    {
        return this->GetAsset(reference, AssetType::Texture, (void **)texture);
    }

    AssetManager::AssetResult AssetManager::GetResult(OldAssetReference reference)
    {
        return _getData(reference, nullptr, nullptr);
    }

    std::string AssetManager::GetError(OldAssetReference reference)
    {
        AssetResult result = GetResult(reference);
        if (result == AssetResult::WrongType)
        {
            return "Incorrect asset type";
        }
        if (result == AssetResult::InvalidReference)
        {
            return "Asset reference is invalid";
        }
        if (result == AssetResult::NotReady)
        {
            return "Asset is not ready";
        }
        if (result == AssetResult::Failed)
        {
            AssetLoader::Handle loaderHandle;
            _getLoaderHandle(reference, nullptr, &loaderHandle);
            return _assetLoader->GetError(loaderHandle);
        }
        return "";
    }

    void AssetManager::Release(OldAssetReference reference)
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

    struct AssetContext
    {
        AssetBuilder *Builder;
        AssetManager *Self;
        AssetManager::AssetId AssetId;
    };
    AssetManager::OldAssetReference AssetManager::_loadAsset(AssetId assetId, std::unique_ptr<AssetBuilder> assetBuilder)
    {
        auto it = _assets.find(assetId);
        if (it != _assets.end())
        {
            AssetEntry &existingEntry = it->second;
            existingEntry.RefCount++;

            return _addReferenceRecord(assetId);
        }

        AssetType assetType = assetBuilder->GetAssetType();

        AssetContext *ctx = new AssetContext{assetBuilder.release(), this, assetId};

        AssetLoader::Handle loaderHandle = _assetLoader->ReadFile(
            assetId,
            [](const AssetBytes &bytes, void *userData)
            {
                AssetContext *ctx = reinterpret_cast<AssetContext *>(userData);
                ctx->Builder->ThreadLoadStep(bytes);
            },
            [](AssetLoader &loader, AssetLoader::Handle handle)
            {
                AssetContext *ctx = reinterpret_cast<AssetContext *>(loader.GetRawData(handle));
                AssetLoader::Status status = loader.GetStatus(handle);
                auto it = ctx->Self->_assets.find(ctx->AssetId);
                PRETOP_ASSERT(it != ctx->Self->_assets.end(), "Asset should exist");
                AssetEntry &record = it->second;
                record.LoaderHandle = handle;
                if (status == AssetLoader::Status::Ready)
                {
                    record.Result = AssetManager::AssetResult::Success;
                    auto data = ctx->Builder->FinalizeLoadStep();
                    record.Data = std::move(data);
                }
                else if (status == AssetLoader::Status::Error)
                {
                    record.Result = AssetManager::AssetResult::Failed;
                }
                loader.Release(handle);
                delete ctx->Builder;
            },
            ctx);

        _assets.emplace(assetId,
                        AssetEntry{
                            /* Type */ assetType,
                            /* LoaderHandle */ loaderHandle,
                            /* Data */ nullptr,
                            /* Result */ AssetManager::AssetResult::NotReady,
                            /* RefCount */ 1,
                        });

        return _addReferenceRecord(assetId);
    }

    AssetManager::AssetResult AssetManager::_getAsset(OldAssetReference reference, AssetType type, void **data)
    {
        if (!this->_records.IsValid(reference.handle))
        {
            return AssetManager::AssetResult::InvalidReference;
        }

        AssetReferenceRecord *record = this->_records[reference.handle];
        auto it = _assets.find(record->AssetId);
        PRETOP_ASSERT(it != _assets.end(), "Asset does not exist");
        AssetEntry &entry = it->second;

        if (entry.Type != type)
        {
            return AssetResult::WrongType;
        }

        if (entry.Result != AssetResult::Success)
        {
            if (entry.Result == AssetResult::NotReady)
            {
                std::cout << "Not Ready" << std::endl;
            }
            if (entry.Result == AssetResult::Failed)
            {
                std::cout << "Failed" << std::endl;
            }
            return entry.Result;
        }

        void *rawData = entry.Data->GetData();
        if (data != nullptr)
        {
            *data = rawData;
        }

        return AssetResult::Success;
    }

    AssetManager::OldAssetReference AssetManager::_addReferenceRecord(AssetId assetId)
    {
        AssetReferenceRecord assetReferenceRecord;
        assetReferenceRecord.AssetId = assetId;
        Core::Handle assetReferenceHandle = _records.Add(assetReferenceRecord);

        OldAssetReference assetReference;
        assetReference.handle = assetReferenceHandle;
        return assetReference;
    };

    AssetManager::AssetResult AssetManager::_getData(OldAssetReference reference, AssetType *type, void **data)
    {
        AssetLoader::Handle loaderHandle;
        AssetManager::AssetResult result = _getLoaderHandle(reference, type, &loaderHandle);

        if (result != AssetResult::Success)
        {
            return result;
        }

        void *rawData = this->_assetLoader->GetRawData(loaderHandle);
        if (data != nullptr)
        {
            *data = rawData;
        }

        return AssetResult::Success;
    }

    AssetManager::AssetResult AssetManager::_getLoaderHandle(
        OldAssetReference reference, AssetType *type, AssetLoader::Handle *handle)
    {
        if (!_records.IsValid(reference.handle))
        {
            return AssetResult::InvalidReference;
        }
        AssetReferenceRecord *assetReferenceRecord = _records[reference.handle];

        auto it = _assets.find(assetReferenceRecord->AssetId);
        if (it == _assets.end())
        {
            return AssetResult::InvalidReference;
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
