#include "AssetLoaderFactory.hpp"
#ifdef __EMSCRIPTEN__
#include "WEB_AssetLoader.hpp"
#else
#include "NATIVE_AssetLoader.hpp"
#endif

namespace Pretop::Asset
{
    std::unique_ptr<AssetLoader> AssetLoaderFactory::CreateAssetLoader(Core::JobSystem *js)
    {
#ifdef __EMSCRIPTEN__
        return std::make_unique<WebAssetLoader>();
#else
        return std::make_unique<NativeAssetLoader>(js);
#endif
    }
} // namespace Pretop::Asset
