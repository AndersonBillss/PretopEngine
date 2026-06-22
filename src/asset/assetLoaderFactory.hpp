#pragma once
#include <memory>
#include <string>
#include "assetLoader.hpp"

namespace AssetLoaderFactory
{
    std::unique_ptr<AssetLoader> createAssetLoader();
}