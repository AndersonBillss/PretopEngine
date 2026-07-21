#include "AssetManagerFactory.hpp"
#include "AssetLoaderFactory.hpp"

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(Core::JobSystem &jobSystem)
    {
        return std::make_unique<AssetManager>(
            jobSystem,
            AssetLoaderFactory::CreateAssetLoader());
    }
} // namespace Pretop::Asset::AssetManagerFactory
