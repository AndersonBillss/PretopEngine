#pragma once
#include "AssetLoader.hpp"

namespace Pretop::Asset
{
    class WebAssetLoader final : public AssetLoader
    {
    public:
        AssetHandle<AssetBytes> LoadBinaryAsync(std::string_view path) override;
        AssetHandle<AssetText> LoadTextAsync(std::string_view path) override;
    };
} // namespace Pretop::Asset
