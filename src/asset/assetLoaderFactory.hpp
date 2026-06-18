#pragma once
#include <memory>
#include <string>
#include "IAssetLoader.hpp"

namespace AssetLoaderFactory
{
    std::unique_ptr<IAssetLoader> createAssetLoader();
}