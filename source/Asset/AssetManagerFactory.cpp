#include "AssetManagerFactory.hpp"
#include "AssetLoaderFactory.hpp"

namespace Pretop::Asset::AssetManagerFactory
{
    std::unique_ptr<AssetManager> CreateAssetManager(std::unique_ptr<AssetLoader> assetLoader,
                                                     Core::GraphicsContext graphicsContext)
    {
        return std::make_unique<AssetManager>(std::move(assetLoader), graphicsContext);
    }
} // namespace Pretop::Asset::AssetManagerFactory
