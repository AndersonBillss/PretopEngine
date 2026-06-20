#pragma once

#include "assetHandle.hpp"

#include <string_view>
#include <memory>

class IAssetLoader
{
public:
    virtual ~IAssetLoader() = default;

    virtual AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) = 0;
    virtual AssetHandle<AssetText> loadTextAsync(std::string_view path) = 0;
};