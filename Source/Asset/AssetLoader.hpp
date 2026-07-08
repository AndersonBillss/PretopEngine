#pragma once
#include "AssetHandle.hpp"

#include <string_view>
#include <memory>

namespace Pretop::Asset
{
    class AssetLoader
    {
    public:
        virtual ~AssetLoader() = default;

        virtual AssetHandle<AssetBytes> LoadBinaryAsync(std::string_view path) = 0;
        virtual AssetHandle<AssetText> LoadTextAsync(std::string_view path) = 0;
    };
} // namespace Pretop::Asset
