#pragma once

#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
#include "AssetType.hpp"
#include "AssetBuilders/AssetBuilder.hpp"
#include "../Core/JobSystem.hpp"
#include "../Core/GraphicsContext.hpp"
#include "../Core/PagedVector.hpp"
#include "../Asset/GPUTexture.hpp"
#include "../RHI/Shader.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace Pretop::Asset
{
    class AssetManager
    {
    public:
        enum class AssetResult
        {
            Success,
            InvalidReference,
            WrongType,
            NotReady,
            Failed,
        };

        struct UntypedAssetReference
        {
            Core::Handle handle;
        };

        template <class T>
        struct AssetReference
        {
            Core::Handle handle;

            constexpr operator UntypedAssetReference() const
            {
                return {handle};
            }
        };

        template <class T>
        AssetReference<T> castReference(UntypedAssetReference assetReference)
        {
            return {assetReference.handle};
        }

        using AssetId = uint64_t;

    private:
        struct AssetReferenceRecord
        {
            AssetManager::AssetId AssetId;
        };

        struct AssetEntry
        {
            AssetType Type;
            AssetLoader::Handle LoaderHandle;
            std::unique_ptr<AssetData> Data;
            AssetManager::AssetResult Result;
            uint32_t RefCount = 0;
        };

    public:
        AssetManager(
            std::unique_ptr<AssetLoader> assetLoader,
            Core::GraphicsContext graphicsContext);
        ~AssetManager();

        AssetReference<ParsedData> LoadModel(AssetId assetId);
        AssetResult GetGlbData(AssetReference<ParsedData> reference, ParsedData **data);

        AssetReference<RHI::Shader> LoadShaderModule(AssetId assetId);
        AssetResult GetShaderModule(AssetReference<RHI::Shader> reference, RHI::Shader **shader);

        AssetReference<WGPUTexture> LoadTexture(AssetId assetId);
        AssetResult GetTexture(AssetReference<WGPUTexture> reference, WGPUTexture **texture);

        UntypedAssetReference LoadAsset(AssetId assetId, std::unique_ptr<AssetBuilder> assetBuilder)
        {
            return _loadAsset(assetId, std::move(assetBuilder));
        };
        AssetResult GetAsset(UntypedAssetReference reference, AssetType type, void **data)
        {
            return _getAsset(reference, type, data);
        };

        AssetResult GetResult(UntypedAssetReference reference);
        std::string GetError(UntypedAssetReference reference);
        void Release(UntypedAssetReference reference);

        template <class T>
        AssetReference<T> copyReference(AssetReference<T> reference)
        {
            return {copyReference(reinterpret_cast<UntypedAssetReference>(reference)).handle};
        };
        UntypedAssetReference copyReference(UntypedAssetReference reference);

    private:
        UntypedAssetReference _loadAsset(AssetId assetId, std::unique_ptr<AssetBuilder> assetBuilder);
        AssetResult _getAsset(UntypedAssetReference reference, AssetType type, void **data);

        UntypedAssetReference _addReferenceRecord(AssetId assetId);
        UntypedAssetReference _removeReference(AssetId assetId);

        AssetResult _getData(UntypedAssetReference reference, AssetType *type, void **data);
        AssetResult _getLoaderHandle(UntypedAssetReference reference, AssetType *type, AssetLoader::Handle *handle);

        std::unique_ptr<AssetLoader> _assetLoader;
        Core::GraphicsContext _graphicsContext;
        Core::RecordTable<AssetReferenceRecord> _records;
        std::unordered_map<AssetId, AssetEntry> _assets;
    };
} // namespace Pretop::Asset
