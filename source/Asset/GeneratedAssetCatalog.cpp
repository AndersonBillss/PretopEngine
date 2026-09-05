#include "GeneratedAssetCatalog.hpp"
#include "../SHARED_/GetAssetId.hpp"
#include <iostream>

namespace Pretop::Asset
{
    GeneratedAssetCatalog::GeneratedAssetCatalog(
        const std::unordered_map<uint64_t, FileMetadata> &metadata) : _metadata(metadata)
    {
    }

    const FileMetadata *Pretop::Asset::GeneratedAssetCatalog::Find(std::string_view assetSource) const
    {
        uint64_t key = Utils::GetAssetId(assetSource);
        auto it = _metadata.find(key);
        if (it == _metadata.end())
        {
            return nullptr;
        }
        return &it->second;
    }
}
