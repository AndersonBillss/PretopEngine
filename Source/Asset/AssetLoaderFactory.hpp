#pragma once
#include <memory>
#include <string>
#include "AssetLoader.hpp"

namespace AssetLoaderFactory
{
    std::unique_ptr<AssetLoader> CreateAssetLoader();
}