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

    AssetText ReadTextFile(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Failed to open text asset: " + path);
        }

        return AssetText(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    }

    void NativeAssetLoader::ReadBinaryAsync(
        std::string_view path,
        BinaryLoadCallback callback)
    {
        std::string pathCopy(path);

        std::thread(
            [callback = std::move(callback), pathCopy = std::move(pathCopy)]() mutable
            {
                try
                {
                    AssetBytes bytes = ReadBinaryFile(pathCopy);
                    callback(AssetResult<AssetBytes>{std::move(bytes), {}});
                }
                catch (const std::exception &e)
                {
                    callback(AssetResult<AssetBytes>{AssetBytes{}, e.what()});
                }
                catch (...)
                {
                    callback(
                        AssetResult<AssetBytes>{AssetBytes{}, "Unknown error while loading binary asset."});
                }
            })
            .detach();
    }

    AssetHandle<AssetText>
    NativeAssetLoader::LoadTextAsync(std::string_view path)
    {
        TaskCompletion<AssetResult<AssetText>> completion;
        auto task = completion.CreateTask();
        std::string pathCopy(path);

        std::thread(
            [completion = std::move(completion), pathCopy = std::move(pathCopy)]() mutable
            {
                try
                {
                    AssetText text = ReadTextFile(pathCopy);
                    completion.SetResult(AssetResult<AssetText>{std::move(text), {}});
                }
                catch (const std::exception &e)
                {
                    completion.SetResult(AssetResult<AssetText>{AssetText{}, e.what()});
                }
                catch (...)
                {
                    completion.SetResult(
                        AssetResult<AssetText>{AssetText{}, "Unknown error while loading text asset."});
                }
            })
            .detach();

        return AssetHandle<AssetText>{std::move(pathCopy), AssetKind::Text, std::move(task)};
    }
} // namespace Pretop::Asset
