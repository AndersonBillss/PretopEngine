#pragma once

#include "LoadGlb.hpp"
#include "AssetLoader.hpp"
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
        enum class AssetType
        {
            Undefined,
            Shader,
            GLB,
            Texture,
        };

        enum class AssetResult
        {
            Success,
            InvalidHandle,
            WrongType,
            NotReady,
            Failed,
        };

        struct AssetReference
        {
            Core::Handle handle;
        };

        // using Handle = Core::Handle;
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

        AssetReference LoadModel(AssetId assetId);
        AssetResult GetGlbData(AssetReference reference, ParsedData **data);

        AssetReference LoadShaderModule(AssetId assetId);
        AssetResult GetShaderModule(AssetReference reference, RHI::Shader **shader);

        AssetReference LoadTexture(AssetId assetId);
        AssetResult GetTexture(AssetReference reference, WGPUTexture **texture);

        Status GetState(AssetReference reference);
        AssetResult GetResult(AssetReference reference);
        std::string GetError(AssetReference reference);
        void Release(AssetReference reference);

    private:
        AssetReference _addReferenceRecord(AssetId assetId);
        AssetReference _removeReference(AssetId assetId);

        AssetResult _getData(AssetReference reference, AssetType *type, void **data);
        AssetResult _getLoaderHandle(AssetReference reference, AssetType *type, AssetLoader::Handle *handle);

        std::unique_ptr<AssetLoader> _assetLoader;
        Core::GraphicsContext _graphicsContext;
        Core::RecordTable<AssetReferenceRecord> _records;
        std::unordered_map<AssetId, AssetEntry> _assets;
    };
} // namespace Pretop::Asset
