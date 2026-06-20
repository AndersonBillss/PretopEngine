#include "WEB_assetLoader.hpp"

#include <cstring>
#include <new>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#if defined(__EMSCRIPTEN__)
    #include <emscripten/fetch.h>
#else
    #error "WebAssetLoader should only be compiled for Emscripten builds."
#endif

namespace
{
    template <typename T>
    struct FetchContext
    {
        TaskCompletion<AssetResult<T>> completion;
    };

    inline std::string makeHttpError(emscripten_fetch_t* fetch)
    {
        std::string message = "Fetch failed";
        if (fetch)
        {
            message += ", HTTP status ";
            message += std::to_string(fetch->status);
            if (fetch->statusText)
            {
                message += " (";
                message += fetch->statusText;
                message += ")";
            }
        }
        return message;
    }

    template <typename T>
    AssetResult<T> buildResultFromFetch(emscripten_fetch_t* fetch)
    {
        AssetResult<T> result;

        if constexpr (std::is_same_v<T, AssetBytes>)
        {
            result.data.resize(static_cast<std::size_t>(fetch->numBytes));
            if (!result.data.empty())
            {
                std::memcpy(result.data.data(), fetch->data, result.data.size());
            }
        }
        else if constexpr (std::is_same_v<T, AssetText>)
        {
            result.data.assign(
                static_cast<const char*>(fetch->data),
                static_cast<std::size_t>(fetch->numBytes));
        }
        else
        {
            static_assert(std::is_same_v<T, AssetBytes> || std::is_same_v<T, AssetText>,
                          "Unsupported asset type for WebAssetLoader.");
        }

        return result;
    }

    template <typename T>
    void onFetchSuccess(emscripten_fetch_t* fetch)
    {
        auto* ctx = static_cast<FetchContext<T>*>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        if (fetch->status < 200 || fetch->status >= 300)
        {
            ctx->completion.set_result(AssetResult<T>{T{}, makeHttpError(fetch)});
        }
        else
        {
            ctx->completion.set_result(buildResultFromFetch<T>(fetch));
        }

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    template <typename T>
    void onFetchError(emscripten_fetch_t* fetch)
    {
        auto* ctx = static_cast<FetchContext<T>*>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        ctx->completion.set_result(AssetResult<T>{T{}, makeHttpError(fetch)});

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    template <typename T>
    AssetHandle<T> loadImpl(std::string_view path, AssetKind kind)
    {
        auto* ctx = new FetchContext<T>();
        auto task = ctx->completion.task();

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.userData = ctx;
        attr.onsuccess = &onFetchSuccess<T>;
        attr.onerror = &onFetchError<T>;

        std::string url(path);
        emscripten_fetch(&attr, url.c_str());

        return AssetHandle<T>{std::move(url), kind, std::move(task)};
    }
}

AssetHandle<AssetBytes>
WebAssetLoader::loadBinaryAsync(std::string_view path)
{
    return loadImpl<AssetBytes>(path, AssetKind::Binary);
}

AssetHandle<AssetText>
WebAssetLoader::loadTextAsync(std::string_view path)
{
    return loadImpl<AssetText>(path, AssetKind::Text);
}
