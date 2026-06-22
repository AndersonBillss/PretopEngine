#include "assetLoaderFactory.hpp"
#ifdef __EMSCRIPTEN__
#include "WEB_assetLoader.hpp"
#else
#include "NATIVE_assetLoader.hpp"
#endif

std::unique_ptr<AssetLoader> AssetLoaderFactory::createAssetLoader()
{
#ifdef __EMSCRIPTEN__
    return std::make_unique<WebAssetLoader>();
#else
    return std::make_unique<NativeAssetLoader>();
#endif
}