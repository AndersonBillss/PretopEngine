#pragma once

#include "assetLoader.hpp"

class WebAssetLoader final : public AssetLoader
{
public:
    AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) override;
    AssetHandle<AssetText> loadTextAsync(std::string_view path) override;
};
