#pragma once
#include "AssetLoader.hpp"

namespace Pretop::Asset
{
    class WebAssetLoader final : public AssetLoader
    {
    public:
        void ReadBinaryAsync(
            std::string_view path,
            BinaryLoadCallback callback,
            Core::JobSystem *js) override;
        AssetHandle<AssetText> LoadTextAsync(std::string_view path) override;
    };
} // namespace Pretop::Asset
