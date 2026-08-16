#include "AssetManagerFactory.hpp"
#include "AssetLoaderFactory.hpp"

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(std::unique_ptr<AssetLoader> assetLoader,
                                                     RHI::Application *app)
    {
        return std::make_unique<AssetManager>(std::move(assetLoader), app);
    }
} // namespace Pretop::Asset::AssetManagerFactory
