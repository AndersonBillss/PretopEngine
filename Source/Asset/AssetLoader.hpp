#pragma once
#include "AssetHandle.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace Pretop::Asset
{
    class AssetLoader
    {
    public:
        using BinaryLoadCallback = std::function<void(AssetResult<AssetBytes>)>;

        virtual ~AssetLoader() = default;

        AssetHandle<AssetBytes> LoadBinaryAsync(std::string_view path)
        {
            TaskCompletion<AssetResult<AssetBytes>> completion;
            auto task = completion.CreateTask();
            std::string id(path);

            ReadBinaryAsync(
                path,
                [completion = std::move(completion)](AssetResult<AssetBytes> result) mutable
                {
                    completion.SetResult(std::move(result));
                });

            return AssetHandle<AssetBytes>{std::move(id), AssetKind::Binary, std::move(task)};
        }

        virtual void ReadBinaryAsync(
            std::string_view path,
            BinaryLoadCallback callback) = 0;
        virtual AssetHandle<AssetText> LoadTextAsync(std::string_view path) = 0;
    };
} // namespace Pretop::Asset
