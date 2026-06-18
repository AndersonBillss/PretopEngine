#ifdef __EMSCRIPTEN__

#include "Web_assetLoader.hpp"

#include <emscripten/fetch.h>

#include <cstring>
#include <future>
#include <memory>
#include <string>
#include <utility>

namespace
{
    template <typename T>
    struct FetchContext
    {
        std::string path;
        std::shared_ptr<std::promise<AssetResult<T>>> promise;
    };

    static void finishBinarySuccess(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<AssetBytes> *>(fetch->userData);

        try
        {
            AssetBytes bytes(static_cast<std::size_t>(fetch->numBytes));
            if (fetch->numBytes > 0 && fetch->data)
            {
                std::memcpy(bytes.data(), fetch->data, static_cast<std::size_t>(fetch->numBytes));
            }

            AssetResult<AssetBytes> result;
            result.data = std::make_shared<const AssetBytes>(std::move(bytes));
            ctx->promise->set_value(std::move(result));
        }
        catch (const std::exception &e)
        {
            ctx->promise->set_value(AssetResult<AssetBytes>{nullptr, e.what()});
        }
        catch (...)
        {
            ctx->promise->set_value(AssetResult<AssetBytes>{nullptr, "Unknown error while loading binary asset."});
        }

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    static void finishBinaryError(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<AssetBytes> *>(fetch->userData);

        std::string message = "Failed to fetch binary asset: " + ctx->path;
        if (fetch->statusText)
        {
            message += " (" + std::string(fetch->statusText) + ")";
        }

        ctx->promise->set_value(AssetResult<AssetBytes>{nullptr, message});

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    static void finishTextSuccess(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<AssetText> *>(fetch->userData);

        try
        {
            AssetText text;
            text.assign(fetch->data, fetch->data + fetch->numBytes);

            AssetResult<AssetText> result;
            result.data = std::make_shared<const AssetText>(std::move(text));
            ctx->promise->set_value(std::move(result));
        }
        catch (const std::exception &e)
        {
            ctx->promise->set_value(AssetResult<AssetText>{nullptr, e.what()});
        }
        catch (...)
        {
            ctx->promise->set_value(AssetResult<AssetText>{nullptr, "Unknown error while loading text asset."});
        }

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    static void finishTextError(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<AssetText> *>(fetch->userData);

        std::string message = "Failed to fetch text asset: " + ctx->path;
        if (fetch->statusText)
        {
            message += " (" + std::string(fetch->statusText) + ")";
        }

        ctx->promise->set_value(AssetResult<AssetText>{nullptr, message});

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    template <typename T>
    AssetHandle<T> makeHandle(const std::string &path, AssetKind kind, std::shared_ptr<std::promise<AssetResult<T>>> promise)
    {
        auto future = promise->get_future().share();
        return AssetHandle<T>(path, kind, std::move(future));
    }

    AssetHandle<AssetBytes> startBinaryFetch(const std::string &path)
    {
        auto *ctx = new FetchContext<AssetBytes>{
            path,
            std::make_shared<std::promise<AssetResult<AssetBytes>>>()};

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = finishBinarySuccess;
        attr.onerror = finishBinaryError;
        attr.userData = ctx;

        auto handle = makeHandle<AssetBytes>(ctx->path, AssetKind::Binary, ctx->promise);
        emscripten_fetch(&attr, ctx->path.c_str());
        return handle;
    }

    AssetHandle<AssetText> startTextFetch(const std::string &path)
    {
        auto *ctx = new FetchContext<AssetText>{
            path,
            std::make_shared<std::promise<AssetResult<AssetText>>>()};

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = finishTextSuccess;
        attr.onerror = finishTextError;
        attr.userData = ctx;

        auto handle = makeHandle<AssetText>(ctx->path, AssetKind::Text, ctx->promise);
        emscripten_fetch(&attr, ctx->path.c_str());
        return handle;
    }
}

AssetHandle<AssetBytes> WebAssetLoader::loadBinaryAsync(std::string_view path)
{
    return startBinaryFetch(std::string(path));
}

AssetHandle<AssetText> WebAssetLoader::loadTextAsync(std::string_view path)
{
    return startTextFetch(std::string(path));
}

#else
#endif