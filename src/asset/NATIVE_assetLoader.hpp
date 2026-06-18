#pragma once

#include "IAssetLoader.hpp"

class NativeAssetLoader final : public IAssetLoader
{
public:
    AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) override;
    AssetHandle<AssetText> loadTextAsync(std::string_view path) override;
};