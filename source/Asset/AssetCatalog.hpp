#pragma once
#include "FileMetadata.hpp"
#include <string_view>

namespace Pretop::Asset
{
    class AssetCatalog
    {
    public:
        virtual ~AssetCatalog() = default;
        virtual const FileMetadata *Find(std::string_view assetSource) const = 0;
        virtual const FileMetadata *Find(uint64_t assetId) const = 0;
    };
}
