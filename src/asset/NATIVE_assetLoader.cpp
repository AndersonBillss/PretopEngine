#include "NATIVE_assetLoader.hpp"

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <future>

namespace
{
    AssetBytes readFileBytes(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);

        if (!file)
        {
            throw std::runtime_error("Failed to open file: " + path);
        }

        std::streamsize size = file.tellg();
        if (size < 0)
        {
            throw std::runtime_error("Failed to get file size: " + path);
        }

        AssetBytes bytes(static_cast<std::size_t>(size));

        file.seekg(0, std::ios::beg);

        if (!file.read(reinterpret_cast<char *>(bytes.data()), size))
        {
            throw std::runtime_error("Failed to read file: " + path);
        }

        return bytes;
    }

    AssetText readFileText(const std::string &path)
    {
        AssetBytes bytes = readFileBytes(path);
        return AssetText(
            reinterpret_cast<const char *>(bytes.data()),
            bytes.size());
    }

    template <typename T, typename LoaderFn>
    AssetHandle<T> makeAsyncHandle(const std::string &path, AssetKind kind, LoaderFn &&loader)
    {
        using Result = AssetResult<T>;

        auto future = std::async(std::launch::async, [path, loader = std::forward<LoaderFn>(loader)]() mutable -> Result
                                 {
                try
                {
                    auto data = std::make_shared<const T>(loader(path));
                    return Result{std::move(data), {}};
                }
                catch (const std::exception& e)
                {
                    return Result{nullptr, e.what()};
                }
                catch (...)
                {
                    return Result{nullptr, "Unknown error while loading asset: " + path};
                } })
                          .share();

        return AssetHandle<T>(path, kind, std::move(future));
    }
}

AssetHandle<AssetBytes> NativeAssetLoader::loadBinaryAsync(std::string_view path)
{
    return makeAsyncHandle<AssetBytes>(std::string(path), AssetKind::Binary, readFileBytes);
}

AssetHandle<AssetText> NativeAssetLoader::loadTextAsync(std::string_view path)
{
    return makeAsyncHandle<AssetText>(std::string(path), AssetKind::Text, readFileText);
}