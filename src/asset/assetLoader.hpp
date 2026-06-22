#pragma once

#include "assetHandle.hpp"

#include <string_view>
#include <memory>

class AssetLoader
{
public:
    virtual ~AssetLoader() = default;

    virtual AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) = 0;
    virtual AssetHandle<AssetText> loadTextAsync(std::string_view path) = 0;
};