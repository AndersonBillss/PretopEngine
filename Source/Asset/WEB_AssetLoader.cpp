#include "WEB_AssetLoader.hpp"

#include "../Core/Assert.hpp"
#include "../Core/RecordTable.hpp"
#include "../Core/Platform.hpp"

#include <atomic>
#include <cstring>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

#include <emscripten/fetch.h>

namespace Pretop::Asset
{
    namespace
    {
        constexpr std::string_view ShaderDirectory = "assets/shaders/";

        enum class ReadSource
        {
            Http,
            VirtualFileSystem,
        };

        struct WebReadFileData
        {
            std::atomic<AssetLoader::Status> State{AssetLoader::Status::InProgress};
            AssetLoader::RawBytesCb RawBytesCb = nullptr;
            AssetLoader::FinishCb FinishCb = nullptr;
            std::string Path;
            std::string ErrorText;
            AssetLoader::AssetBytes Result;
            void *UserData = nullptr;
            WebAssetLoader *Self = nullptr;
            AssetLoader::Handle AssetHandle{};
            Core::Handle ProcessingJob{};
            emscripten_fetch_t *Fetch = nullptr;
            ReadSource Source = ReadSource::Http;
            bool FetchSucceeded = false;
            bool HasProcessingJob = false;
        };

        std::string_view NormalizeAssetPath(std::string_view path)
        {
            if (!path.empty() && path.front() == '/')
            {
                path.remove_prefix(1);
            }
            return path;
        }

        bool IsShaderPath(std::string_view path)
        {
            path = NormalizeAssetPath(path);
            return path.size() >= ShaderDirectory.size() &&
                   path.compare(0, ShaderDirectory.size(), ShaderDirectory) == 0;
        }

        std::string MakeHttpError(emscripten_fetch_t *fetch)
        {
            std::string message = "Failed to fetch asset";
            if (fetch != nullptr)
            {
                message += " '";
                message += fetch->url != nullptr ? fetch->url : "";
                message += "': HTTP status ";
                message += std::to_string(fetch->status);
                if (fetch->statusText[0] != '\0')
                {
                    message += " (";
                    message += fetch->statusText;
                    message += ")";
                }
            }
            return message;
        }

        AssetLoader::AssetBytes ReadBinaryFileFromVfs(const std::string &path)
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
            {
                throw std::runtime_error(
                    "Failed to open asset from Emscripten virtual file system: " + path);
            }

            const std::streampos end = file.tellg();
            if (end < 0)
            {
                throw std::runtime_error(
                    "Failed to determine VFS asset size: " + path);
            }

            AssetLoader::AssetBytes bytes(static_cast<std::size_t>(end));
            file.seekg(0, std::ios::beg);
            if (!bytes.empty())
            {
                file.read(
                    reinterpret_cast<char *>(bytes.data()),
                    static_cast<std::streamsize>(bytes.size()));
            }

            if (!file)
            {
                throw std::runtime_error(
                    "Failed to read asset from Emscripten virtual file system: " + path);
            }
            return bytes;
        }

        void ProcessReadFile(void *userData)
        {
            auto *data = static_cast<WebReadFileData *>(userData);
            try
            {
                if (!data->ErrorText.empty())
                {
                    throw std::runtime_error(data->ErrorText);
                }

                if (data->Source == ReadSource::VirtualFileSystem)
                {
                    data->Result = ReadBinaryFileFromVfs(data->Path);
                }
                else
                {
                    if (!data->FetchSucceeded || data->Fetch == nullptr)
                    {
                        throw std::runtime_error("Fetch failed without an error response: " + data->Path);
                    }
                    if (data->Fetch->numBytes > std::numeric_limits<std::size_t>::max())
                    {
                        throw std::runtime_error("Fetched asset is too large: " + data->Path);
                    }
                    if (data->Fetch->numBytes > 0 && data->Fetch->data == nullptr)
                    {
                        throw std::runtime_error("Fetch returned no bytes for asset: " + data->Path);
                    }

                    data->Result.resize(static_cast<std::size_t>(data->Fetch->numBytes));
                    if (!data->Result.empty())
                    {
                        std::memcpy(
                            data->Result.data(),
                            data->Fetch->data,
                            data->Result.size());
                    }
                }

                if (data->RawBytesCb != nullptr)
                {
                    data->RawBytesCb(data->Result, data->UserData);
                }
            }
            catch (const std::exception &error)
            {
                data->ErrorText = error.what();
                throw;
            }
            catch (...)
            {
                data->ErrorText = "Unknown error while processing asset: " + data->Path;
                throw;
            }
        }

        void CompleteReadFile(Core::JobSystem &jobSystem, Core::Handle jobHandle)
        {
            auto *data = static_cast<WebReadFileData *>(jobSystem.GetData(jobHandle));
            const AssetLoader::Status finalState = jobSystem.GetState(jobHandle);

            if (data->Fetch != nullptr)
            {
                emscripten_fetch_close(data->Fetch);
                data->Fetch = nullptr;
            }

            data->State.store(finalState);
            if (data->FinishCb != nullptr)
            {
                data->FinishCb(*data->Self, data->AssetHandle);
            }
        }
    }

    struct WebAssetLoader::Impl
    {
        Core::RecordTable<WebReadFileData *> Records;
    };

    WebAssetLoader::WebAssetLoader(Core::JobSystem *js)
        : _js(js), _impl(std::make_unique<Impl>())
    {
        PRETOP_ASSERT(js != nullptr, "WebAssetLoader requires a JobSystem");
    }

    WebAssetLoader::~WebAssetLoader() = default;

    AssetLoader::Handle WebAssetLoader::ReadFile(std::string_view path)
    {
        return ReadFile(path, nullptr, nullptr, nullptr);
    }

    AssetLoader::Handle WebAssetLoader::ReadFile(
        std::string_view path,
        AssetLoader::FinishCb finishCb,
        void *userData)
    {
        return ReadFile(path, nullptr, finishCb, userData);
    }

    AssetLoader::Handle WebAssetLoader::ReadFile(
        std::string_view path,
        AssetLoader::RawBytesCb rawBytesCb,
        AssetLoader::FinishCb finishCb,
        void *userData)
    {
        auto *data = new WebReadFileData;
        data->RawBytesCb = rawBytesCb;
        data->FinishCb = finishCb;
        data->Path = std::string(path);
        data->UserData = userData;
        data->Self = this;
        data->Source = IsShaderPath(path)
                           ? ReadSource::VirtualFileSystem
                           : ReadSource::Http;
        data->AssetHandle = _impl->Records.Add(data);

        if (data->Source == ReadSource::VirtualFileSystem)
        {
            data->ProcessingJob = _js->Submit(
                {ProcessReadFile, data},
                {CompleteReadFile});
            data->HasProcessingJob = true;
            return data->AssetHandle;
        }

        emscripten_fetch_attr_t attributes;
        emscripten_fetch_attr_init(&attributes);
        std::strcpy(attributes.requestMethod, "GET");
        attributes.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attributes.userData = data;
        attributes.onsuccess = &WebAssetLoader::OnFetchSuccess;
        attributes.onerror = &WebAssetLoader::OnFetchError;

        emscripten_fetch_t *fetch = emscripten_fetch(&attributes, data->Path.c_str());
        if (fetch == nullptr)
        {
            data->ErrorText = "Failed to start fetch for asset: " + data->Path;
            data->ProcessingJob = _js->Submit(
                {ProcessReadFile, data},
                {CompleteReadFile});
            data->HasProcessingJob = true;
        }

        return data->AssetHandle;
    }

    void WebAssetLoader::OnFetchSuccess(emscripten_fetch_t *fetch)
    {
        auto *data = static_cast<WebReadFileData *>(fetch->userData);
        data->Self->QueueFetchResult(fetch, fetch->status >= 200 && fetch->status < 300);
    }

    void WebAssetLoader::OnFetchError(emscripten_fetch_t *fetch)
    {
        auto *data = static_cast<WebReadFileData *>(fetch->userData);
        data->Self->QueueFetchResult(fetch, false);
    }

    void WebAssetLoader::QueueFetchResult(emscripten_fetch_t *fetch, bool succeeded)
    {
        auto *data = static_cast<WebReadFileData *>(fetch->userData);
        data->Fetch = fetch;
        data->FetchSucceeded = succeeded;
        if (!succeeded)
        {
            data->ErrorText = MakeHttpError(fetch);
        }
        data->ProcessingJob = _js->Submit(
            {ProcessReadFile, data},
            {CompleteReadFile});
        data->HasProcessingJob = true;
    }

    AssetLoader::Status WebAssetLoader::GetStatus(Handle handle) const
    {
        return (*_impl->Records[handle])->State.load();
    }

    const AssetLoader::AssetBytes &WebAssetLoader::GetBytes(Handle handle) const
    {
        return (*_impl->Records[handle])->Result;
    }

    void *WebAssetLoader::GetRawData(Handle handle)
    {
        return (*_impl->Records[handle])->UserData;
    }

    std::string WebAssetLoader::GetError(Handle handle) const
    {
        return (*_impl->Records[handle])->ErrorText;
    }

    void WebAssetLoader::Release(Handle handle)
    {
        WebReadFileData *data = *_impl->Records[handle];
        PRETOP_ASSERT(
            data->State.load() != Status::InProgress,
            "Cannot release an asset while it is still loading");
        PRETOP_ASSERT(data->HasProcessingJob, "Asset has no processing job");

        _js->Release(data->ProcessingJob);
        delete data;
        _impl->Records.Release(handle);
    }
} // namespace Pretop::Asset
