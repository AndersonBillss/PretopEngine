#pragma once
#include <memory>
#include <string>
#include "AssetLoader.hpp"

namespace Pretop::Asset
{
    namespace AssetLoaderFactory
    {
        std::unique_ptr<AssetLoader> CreateAssetLoader();
    }
} // namespace Pretop::Asset
