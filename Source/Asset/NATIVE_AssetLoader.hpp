#pragma once

#include "AssetLoader.hpp"

class NativeAssetLoader final : public AssetLoader
{
public:
    AssetHandle<AssetBytes> loadBinaryAsync(std::string_view path) override;
    AssetHandle<AssetText> loadTextAsync(std::string_view path) override;
};