#pragma once

#include "AssetManager.hpp"

#include <memory>

namespace Pretop::Core
{
    class JobSystem;
}

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(Core::JobSystem &jobSystem);
} // namespace Pretop::Asset::AssetManagerFactory
