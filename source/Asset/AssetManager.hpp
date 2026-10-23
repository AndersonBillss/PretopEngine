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

        struct OldAssetReference
        {
            Core::Handle handle;
        };

        using Status = Core::Status;
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
            uint32_t RefCount = 0;
        };

    public:
        AssetManager(
            std::unique_ptr<AssetLoader> assetLoader,
            Core::GraphicsContext graphicsContext);
        ~AssetManager();

        OldAssetReference LoadModel(AssetId assetId);
        AssetResult GetGlbData(OldAssetReference reference, ParsedData **data);

        OldAssetReference LoadShaderModule(AssetId assetId);
        AssetResult GetShaderModule(OldAssetReference reference, RHI::Shader **shader);

        OldAssetReference LoadTexture(AssetId assetId);
        AssetResult GetTexture(OldAssetReference reference, WGPUTexture **texture);

        // AssetReference LoadAsset<T>(AssetId assetId, std::unique_ptr<AssetBuilder> assetBuilder);
        // AssetResult GetAsset<T>(AssetReference reference, T **texture);

        Status GetState(OldAssetReference reference);
        AssetResult GetResult(OldAssetReference reference);
        std::string GetError(OldAssetReference reference);
        void Release(OldAssetReference reference);

    private:
        OldAssetReference _addReferenceRecord(AssetId assetId);
        OldAssetReference _removeReference(AssetId assetId);

        AssetResult _getData(OldAssetReference reference, AssetType *type, void **data);
        AssetResult _getLoaderHandle(OldAssetReference reference, AssetType *type, AssetLoader::Handle *handle);

        std::unique_ptr<AssetLoader> _assetLoader;
        Core::GraphicsContext _graphicsContext;
        Core::RecordTable<AssetReferenceRecord> _records;
        std::unordered_map<AssetId, AssetEntry> _assets;
    };
} // namespace Pretop::Asset
