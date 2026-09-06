#pragma once

#include "AssetManager.hpp"

#include <memory>

namespace Pretop::Core
{
    class JobSystem;
}

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(std::unique_ptr<AssetLoader> assetLoader,
                                                     Core::GraphicsContext graphicsContext);
} // namespace Pretop::Asset::AssetManagerFactory
