#pragma once
#include <memory>
#include <string>
#include "AssetLoader.hpp"
#include "AssetCatalog.hpp"
#include "../Core/JobSystem.hpp"

namespace Pretop::Asset
{
    namespace AssetLoaderFactory
    {
        std::unique_ptr<AssetLoader> CreateAssetLoader(Core::JobSystem *js, std::unique_ptr<AssetCatalog> catalog);
    }
} // namespace Pretop::Asset
