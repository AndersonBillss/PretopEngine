#pragma once
#include "../Gen/AssetMetadata.hpp"
#include <string_view>

namespace Pretop::Asset
{
    class AssetCatalog
    {
    public:
        virtual ~AssetCatalog() = default;
        virtual const Gen::FileMetadata *Find(std::string_view assetSource) const = 0;
    };
}
