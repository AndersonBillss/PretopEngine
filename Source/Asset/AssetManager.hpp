#pragma once

#include "AssetHandle.hpp"
#include "LoadGlb.hpp"

#include <memory>
#include <string_view>

namespace Pretop::Core
{
    class JobSystem;
}

namespace Pretop::Asset
{
    class AssetLoader;

    class AssetManager
    {
    public:
        AssetManager(
            Core::JobSystem &jobSystem,
            std::unique_ptr<AssetLoader> assetLoader);
        ~AssetManager();

        AssetHandle<ParsedData> LoadModelAsync(std::string_view path);

    private:
        Core::JobSystem &_jobSystem;
        std::unique_ptr<AssetLoader> _assetLoader;
    };
} // namespace Pretop::Asset
