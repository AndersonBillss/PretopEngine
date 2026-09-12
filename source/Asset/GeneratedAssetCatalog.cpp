#include "GeneratedAssetCatalog.hpp"
#include "../SHARED_/GetAssetId.hpp"
#include <iostream>

namespace Pretop::Asset
{
    GeneratedAssetCatalog::GeneratedAssetCatalog(
        const std::unordered_map<uint64_t, FileMetadata> &metadata) : _metadata(metadata)
    {
    }

    const FileMetadata *GeneratedAssetCatalog::Find(std::string_view assetSource) const
    {
        uint64_t key = Utils::GetAssetId(assetSource);
        return Find(key);
    }

    const FileMetadata *GeneratedAssetCatalog::Find(uint64_t assetId) const
    {

        auto it = _metadata.find(assetId);
        if (it == _metadata.end())
        {
            return nullptr;
        }
        return &it->second;
    };
}
