#pragma once

#include "IAssetLoader.hpp"

class WebAssetLoader final : public IAssetLoader
{
public:
    AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) override;
    AssetHandle<AssetText> loadTextAsync(std::string_view path) override;
};
