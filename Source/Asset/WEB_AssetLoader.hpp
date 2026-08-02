#pragma once
#include "AssetLoader.hpp"

#include <memory>

struct emscripten_fetch_t;

namespace Pretop::Asset
{
    class WebAssetLoader final : public AssetLoader
    {
    public:
        WebAssetLoader(Core::JobSystem *js);
        ~WebAssetLoader() override;

        Handle ReadFile(std::string_view path) override;
        Handle ReadFile(std::string_view path, FinishCb finishCb, void *userData) override;
        Handle ReadFile(std::string_view path, RawBytesCb rawBytesCb, FinishCb finishCb, void *userData) override;

        Status GetStatus(Handle handle) const override;
        const AssetBytes &GetBytes(Handle handle) const override;
        void *GetRawData(Handle handle) override;

        std::string GetError(Handle handle) const override;
        void Release(Handle handle) override;

    private:
        struct Impl;

        static void OnFetchSuccess(emscripten_fetch_t *fetch);
        static void OnFetchError(emscripten_fetch_t *fetch);

        void QueueFetchResult(emscripten_fetch_t *fetch, bool succeeded);

        Core::JobSystem *_js;
        std::unique_ptr<Impl> _impl;
    };

} // namespace Pretop::Asset
