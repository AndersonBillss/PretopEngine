#include "AssetLoaderFactory.hpp"
#ifdef __EMSCRIPTEN__
#include "WEB_AssetLoader.hpp"
#else
#include "NATIVE_AssetLoader.hpp"
#endif

namespace Pretop::Asset
{
    std::unique_ptr<AssetLoader> AssetLoaderFactory::CreateAssetLoader()
    {
#ifdef __EMSCRIPTEN__
        return std::make_unique<WebAssetLoader>();
#else
        return std::make_unique<NativeAssetLoader>();
#endif
    }
} // namespace Pretop::Asset
