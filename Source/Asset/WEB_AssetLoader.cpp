#include "WEB_AssetLoader.hpp"
#include <cstring>
#include <fstream>
#include <iterator>
#include <functional>
#include <string>
#include <utility>

#if defined(__EMSCRIPTEN__)
#include <emscripten/fetch.h>
#else
#error "WebAssetLoader should only be compiled for Emscripten builds."
#endif

namespace Pretop::Asset
{
    struct FetchContext
    {
        AssetLoader::BinaryLoadCallback Callback;
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

    AssetResult<AssetBytes> BuildResultFromFetch(emscripten_fetch_t *fetch)
    {
        AssetResult<AssetBytes> result;
        result.Data.resize(static_cast<std::size_t>(fetch->numBytes));
        if (!result.Data.empty())
        {
            std::memcpy(result.Data.data(), fetch->data, result.Data.size());
        }

        return result;
    }

    void OnFetchSuccess(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext *>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        if (fetch->status < 200 || fetch->status >= 300)
        {
            ctx->Callback(AssetResult<AssetBytes>{AssetBytes{}, MakeHttpError(fetch)});
        }
        else
        {
            ctx->Callback(BuildResultFromFetch(fetch));
        }

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    void OnFetchError(emscripten_fetch_t *fetch)
    {
        auto *ctx = static_cast<FetchContext *>(fetch->userData);
        if (!ctx)
        {
            emscripten_fetch_close(fetch);
            return;
        }

        ctx->Callback(AssetResult<AssetBytes>{AssetBytes{}, MakeHttpError(fetch)});

        emscripten_fetch_close(fetch);
        delete ctx;
    }

    void WebAssetLoader::ReadBinaryAsync(
        std::string_view path,
        BinaryLoadCallback callback)
    {
        auto *ctx = new FetchContext{std::move(callback)};

        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.userData = ctx;
        attr.onsuccess = &OnFetchSuccess;
        attr.onerror = &OnFetchError;

        std::string url(path);
        emscripten_fetch(&attr, url.c_str());
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

    AssetHandle<AssetText>
    WebAssetLoader::LoadTextAsync(std::string_view path)
    {
        TaskCompletion<AssetResult<AssetText>> completion;
        auto task = completion.CreateTask();

        completion.SetResult(ReadTextFromVfs(path));

        return AssetHandle<AssetText>{std::string(path), AssetKind::Text, std::move(task)};
    }
} // namespace Pretop::Asset
