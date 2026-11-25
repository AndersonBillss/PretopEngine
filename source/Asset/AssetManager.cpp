#include "AssetManager.hpp"

#include "AssetLoader.hpp"
#include "../Core/JobSystem.hpp"
#include "AssetBuilders/GlbAssetBuilder.hpp"
#include "AssetBuilders/ShaderModuleAssetBuilder.hpp"
#include "AssetBuilders/TextureAssetBuilder.hpp"

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

    AssetManager::AssetReference<ParsedData> AssetManager::LoadModel(
        AssetId assetId)
    {
        std::unique_ptr<GlbAssetBuilder> builder = std::make_unique<GlbAssetBuilder>();
        return castReference<ParsedData>(this->LoadAsset(assetId, std::move(builder)));
    }

    AssetManager::AssetResult AssetManager::GetGlbData(AssetReference<ParsedData> reference, ParsedData **data)
    {
        return this->GetAsset(reference, AssetType::GLB, (void **)data);
    };

    AssetManager::AssetReference<RHI::Shader> AssetManager::LoadShaderModule(AssetId assetId)
    {
        std::unique_ptr<ShaderModuleAssetBuilder> builder = std::make_unique<ShaderModuleAssetBuilder>(this->_graphicsContext);
        return castReference<RHI::Shader>(this->LoadAsset(assetId, std::move(builder)));
    }

    AssetManager::AssetResult AssetManager::GetShaderModule(AssetReference<RHI::Shader> reference, RHI::Shader **shader)
    {
        return this->GetAsset(reference, AssetType::Shader, (void **)shader);
    }

    AssetManager::AssetReference<WGPUTexture> AssetManager::LoadTexture(AssetId assetId)
    {
        std::unique_ptr<TextureAssetBuilder> builder = std::make_unique<TextureAssetBuilder>(this->_graphicsContext);
        return castReference<WGPUTexture>(this->LoadAsset(assetId, std::move(builder)));
    }

    AssetManager::AssetResult AssetManager::GetTexture(AssetReference<WGPUTexture> reference, WGPUTexture **texture)
    {
        return this->GetAsset(reference, AssetType::Texture, (void **)texture);
    }

    AssetManager::AssetResult AssetManager::GetAsset(UntypedAssetReference reference, AssetType type, void **data)
    {
        AssetEntry *entry = nullptr;
        AssetManager::AssetResult result = this->_getAsset(reference, type, &entry);
        if (data != nullptr && result == AssetResult::Success)
        {
            entry->Data->GetData();
            *data = entry->Data->GetData();
        }
        return result;
    }

    AssetManager::AssetResult AssetManager::GetResult(UntypedAssetReference reference)
    {
        return _getAsset(reference, AssetType::Undefined, nullptr);
    }

    std::string AssetManager::GetError(UntypedAssetReference reference)
    {
        AssetEntry *entry = nullptr;
        AssetResult result = _getAsset(reference, AssetType::Undefined, &entry);
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
            return entry->ErrorText;
        }
        return "";
    }

    void AssetManager::Release(UntypedAssetReference reference)
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
            _assets.erase(assetReferenceRecord->AssetId);
        }
        _records.Release(reference.handle);
    }

    AssetManager::UntypedAssetReference AssetManager::CopyReference(UntypedAssetReference reference)
    {
        if (!this->_records.IsValid(reference.handle))
        {
            // Return invalid asset reference
            return UntypedAssetReference{Core::Handle{0, 0}};
        }
        auto assetReferenceRecord = _records[reference.handle];
        auto it = _assets.find(assetReferenceRecord->AssetId);
        if (it == _assets.end())
        {
            // Return invalid asset reference
            return UntypedAssetReference{Core::Handle{0, 0}};
        }
        else
        {
            AssetEntry &existingEntry = it->second;
            existingEntry.RefCount++;
        }
        return _addReferenceRecord(assetReferenceRecord->AssetId);
    }

    struct AssetContext
    {
        AssetBuilder *Builder;
        AssetManager *Self;
        AssetManager::AssetId AssetId;
    };
    AssetManager::UntypedAssetReference AssetManager::_loadAsset(AssetId assetId, std::unique_ptr<AssetBuilder> assetBuilder)
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

        _assetLoader->ReadFile(
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
                if (status == AssetLoader::Status::Ready)
                {
                    record.Result = AssetManager::AssetResult::Success;
                    auto data = ctx->Builder->FinalizeLoadStep();
                    record.Data = std::move(data);
                }
                else if (status == AssetLoader::Status::Error)
                {
                    record.Result = AssetManager::AssetResult::Failed;
                    record.ErrorText = loader.GetError(handle);
                }
                loader.Release(handle);
                delete ctx->Builder;
            },
            ctx);

        _assets.emplace(assetId,
                        AssetEntry{
                            /* Type */ assetType,
                            /* ErrorText */ "",
                            /* Data */ nullptr,
                            /* Result */ AssetManager::AssetResult::NotReady,
                            /* RefCount */ 1,
                        });

        return _addReferenceRecord(assetId);
    }

    AssetManager::AssetResult AssetManager::_getAsset(UntypedAssetReference reference, AssetType type, AssetEntry **entry)
    {
        if (!this->_records.IsValid(reference.handle))
        {
            return AssetManager::AssetResult::InvalidReference;
        }

        AssetReferenceRecord *record = this->_records[reference.handle];
        auto it = _assets.find(record->AssetId);
        PRETOP_ASSERT(it != _assets.end(), "Asset does not exist");
        if (entry != nullptr)
        {
            *entry = &it->second;
        }
        AssetEntry &e = it->second;

        if (e.Type != type && type != AssetType::Undefined)
        {
            return AssetResult::WrongType;
        }

        if (e.Result != AssetResult::Success)
        {
            return e.Result;
        }

        return AssetResult::Success;
    }

    AssetManager::UntypedAssetReference AssetManager::_addReferenceRecord(AssetId assetId)
    {
        AssetReferenceRecord assetReferenceRecord;
        assetReferenceRecord.AssetId = assetId;
        Core::Handle assetReferenceHandle = _records.Add(assetReferenceRecord);

        UntypedAssetReference assetReference;
        assetReference.handle = assetReferenceHandle;
        return assetReference;
    };
} // namespace Pretop::Asset
