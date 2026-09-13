#pragma once
#include "AssetLoader.hpp"
#include "AssetCatalog.hpp"

namespace Pretop::Asset
{
    class NativeAssetLoader final : public AssetLoader
    {
    public:
        NativeAssetLoader(Core::JobSystem *js, std::unique_ptr<AssetCatalog> catalog);

        Handle ReadFile(std::string_view path) override;
        Handle ReadFile(std::string_view path, FinishCb finishCb, void *userData) override;
        Handle ReadFile(std::string_view path, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) override;

        Handle ReadFile(uint64_t assetId) override;
        Handle ReadFile(uint64_t assetId, FinishCb finishCb, void *userData) override;
        Handle ReadFile(uint64_t assetId, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) override;

        Status GetStatus(Handle handle) const override;
        const AssetBytes &GetBytes(Handle handle) const override;
        void *GetRawData(Handle handle) override;

        std::string GetError(Handle handle) const override;
        void Release(Handle handle) override;

    private:
        Core::JobSystem *_js;
        std::unique_ptr<AssetCatalog> _catalog;
    };

} // namespace Pretop::Asset
