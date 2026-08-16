#pragma once
#include "AssetLoader.hpp"

namespace Pretop::Asset
{
    class NativeAssetLoader final : public AssetLoader
    {
    public:
        NativeAssetLoader(Core::JobSystem *js);

        Handle ReadFile(std::string_view path) override;
        Handle ReadFile(std::string_view path, FinishCb finishCb, void *userData) override;
        Handle ReadFile(std::string_view path, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) override;

        Status GetStatus(Handle handle) const override;
        const AssetBytes &GetBytes(Handle handle) const override;
        void *GetRawData(Handle handle) override;

        std::string GetError(Handle handle) const override;
        void Release(Handle handle) override;

    private:
        Core::JobSystem *_js;
    };

} // namespace Pretop::Asset
