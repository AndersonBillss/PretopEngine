#include "NATIVE_AssetLoader.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace Pretop::Asset
{
    AssetBytes ReadBinaryFile(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
        {
            throw std::runtime_error("Failed to open binary asset: " + path);
        }

        const std::streampos end = file.tellg();
        if (end < 0)
        {
            throw std::runtime_error("Failed to determine binary asset size: " + path);
        }

        AssetBytes bytes(static_cast<std::size_t>(end));
        file.seekg(0, std::ios::beg);

        if (!bytes.empty())
        {
            file.read(reinterpret_cast<char *>(bytes.data()),
                      static_cast<std::streamsize>(bytes.size()));
        }

        if (!file)
        {
            throw std::runtime_error("Failed to read binary asset: " + path);
        }

        return bytes;
    }

    NativeAssetLoader::NativeAssetLoader(Core::JobSystem *js)
    {
        this->_js = js;
    }

    AssetLoader::Handle NativeAssetLoader::ReadFile(std::string_view path)
    {
        return ReadFile(path, nullptr, nullptr, nullptr);
    }

    AssetLoader::Handle NativeAssetLoader::ReadFile(std::string_view path, AssetLoader::FinishCb finishCb, void *userData)
    {
        return ReadFile(path, nullptr, finishCb, nullptr);
    }

    struct ReadFileJobData
    {
        AssetLoader::RawBytesCb RawBytesCb;
        AssetLoader::FinishCb FinishCb;
        std::string Path;
        std::string ErrorText;
        AssetBytes Result;
        void *userData;
        NativeAssetLoader *self;
    };
    AssetLoader::Handle NativeAssetLoader::ReadFile(std::string_view path, AssetLoader::RawBytesCb rawBytesCb, AssetLoader::FinishCb finishCb, void *userData)
    {
        ReadFileJobData *data = new ReadFileJobData;
        data->RawBytesCb = rawBytesCb;
        data->FinishCb = finishCb;
        data->Path = std::string(path);
        data->ErrorText = "";
        data->Result = {};
        data->userData = userData;
        data->self = this;

        return this->_js->Submit(
            {[](void *userData)
             {
                 ReadFileJobData *readFileData = reinterpret_cast<ReadFileJobData *>(userData);
                 try
                 {
                     readFileData->Result = ReadBinaryFile(readFileData->Path);
                     if (readFileData->RawBytesCb != nullptr)
                     {
                         readFileData->RawBytesCb(readFileData->Result, readFileData->userData);
                     }
                 }
                 catch (const std::exception &e)
                 {
                     readFileData->ErrorText = e.what();
                     throw;
                 }
                 catch (...)
                 {
                     readFileData->ErrorText = "Unknown error while loading binary asset.";
                     throw;
                 }
             },
             data},
            {[](Core::JobSystem &js, Handle handle)
             {
                 ReadFileJobData *readFileData =
                     reinterpret_cast<ReadFileJobData *>(js.GetData(handle));
                 readFileData->FinishCb(*readFileData->self, handle);
             }});
    };

    AssetLoader::Status NativeAssetLoader::GetStatus(Handle handle) const
    {
        return _js->GetState(handle);
    }

    const AssetBytes &NativeAssetLoader::GetBytes(Handle handle) const
    {
        return std::move(reinterpret_cast<ReadFileJobData *>(_js->GetData(handle))->Result);
    }

    void *NativeAssetLoader::GetRawData(Handle handle)
    {
        return std::move(reinterpret_cast<ReadFileJobData *>(_js->GetData(handle))->userData);
    }

    std::string NativeAssetLoader::GetError(Handle handle) const
    {
        return reinterpret_cast<ReadFileJobData *>(_js->GetData(handle))->ErrorText;
    }

    void NativeAssetLoader::Release(Handle handle)
    {
        return _js->Release(handle);
    }
}
