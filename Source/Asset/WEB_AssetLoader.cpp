#include "WEB_AssetLoader.hpp"
#include <cstring>
#include <fstream>
#include <iterator>
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

namespace Pretop::Asset
{
    template <typename T>
    struct FetchContext
    {
        TaskCompletion<AssetResult<T>> Completion;
    };

    inline std::string MakeHttpError(emscripten_fetch_t *fetch)
    {
        std::string message = "Fetch failed";
        if (fetch)
        {
            message += ", HTTP status ";
            message += std::to_string(fetch->status);
            message += " (";
            message += fetch->statusText;
            message += ")";
        }
        return message;
    }

    template <typename T>
    AssetResult<T> BuildResultFromFetch(emscripten_fetch_t *fetch)
    {
        AssetResult<T> result;

        if constexpr (std::is_same_v<T, AssetBytes>)
        {
            result.Data.resize(static_cast<std::size_t>(fetch->numBytes));
            if (!result.Data.empty())
            {
                std::memcpy(result.Data.data(), fetch->data, result.Data.size());
            }
        }
        else if constexpr (std::is_same_v<T, AssetText>)
        {
            result.Data.assign(
                static_cast<const char *>(fetch->data),
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
    void OnFetchSuccess(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<T> *>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        if (fetch->status < 200 || fetch->status >= 300)
        {
            ctx->Completion.SetResult(AssetResult<T>{T{}, MakeHttpError(fetch)});
        }
        else
        {
            ctx->Completion.SetResult(BuildResultFromFetch<T>(fetch));
        }

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    template <typename T>
    void OnFetchError(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext<T> *>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        ctx->Completion.SetResult(AssetResult<T>{T{}, MakeHttpError(fetch)});

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    template <typename T>
    AssetHandle<T> LoadImpl(std::string_view path, AssetKind kind)
    {
        auto *ctx = new FetchContext<T>();
        auto task = ctx->Completion.CreateTask();

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.userData = ctx;
        attr.onsuccess = &OnFetchSuccess<T>;
        attr.onerror = &OnFetchError<T>;

        std::string url(path);
        emscripten_fetch(&attr, url.c_str());

        return AssetHandle<T>{std::move(url), kind, std::move(task)};
    }

    AssetResult<AssetText> ReadTextFromVfs(std::string_view path)
    {
        AssetResult<AssetText> result;

        std::ifstream file(std::string(path), std::ios::binary);
        if (!file)
        {
            result.Error = "Failed to open file from Emscripten virtual file system: " + std::string(path);
            return result;
        }

        file.seekg(0, std::ios::end);
        const std::streamoff size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size < 0)
        {
            result.Error = "Failed to read file size from Emscripten virtual file system: " + std::string(path);
            return result;
        }

        result.Data.resize(static_cast<std::size_t>(size));
        if (!result.Data.empty())
        {
            file.read(result.Data.data(), static_cast<std::streamsize>(result.Data.size()));
            if (!file)
            {
                result.Error = "Failed to read file from Emscripten virtual file system: " + std::string(path);
                result.Data.clear();
            }
        }

        return result;
    }

    AssetHandle<AssetBytes>
    WebAssetLoader::LoadBinaryAsync(std::string_view path)
    {
        return LoadImpl<AssetBytes>(path, AssetKind::Binary);
    }

    AssetHandle<AssetText>
    WebAssetLoader::LoadTextAsync(std::string_view path)
    {
        TaskCompletion<AssetResult<AssetText>> completion;
        auto task = completion.CreateTask();

        completion.SetResult(ReadTextFromVfs(path));

        return AssetHandle<AssetText>{std::string(path), AssetKind::Text, std::move(task)};
    }
} // namespace Pretop::Asset
