#include "AssetLoaderFactory.hpp"
#include "../Core/Platform.hpp"

#ifdef PRETOP_PLATFORM_WEB
#include "WEB_AssetLoader.hpp"
#else
#include "NATIVE_AssetLoader.hpp"
#endif

namespace Pretop::Asset
{
    std::unique_ptr<AssetLoader> AssetLoaderFactory::CreateAssetLoader(Core::JobSystem *js)
    {
#ifdef PRETOP_PLATFORM_WEB
        return std::make_unique<WebAssetLoader>(js);
#else
        return std::make_unique<NativeAssetLoader>(js);
#endif
    }
} // namespace Pretop::Asset
