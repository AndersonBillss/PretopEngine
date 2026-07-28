#include "AssetManagerFactory.hpp"
#include "AssetLoaderFactory.hpp"

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(std::unique_ptr<AssetLoader> assetLoader,
                                                     RHI::Device *device)
    {
        return std::make_unique<AssetManager>(std::move(assetLoader), device);
    }
} // namespace Pretop::Asset::AssetManagerFactory
