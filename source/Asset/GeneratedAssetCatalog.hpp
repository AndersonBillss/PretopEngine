#pragma once
#include "AssetCatalog.hpp"
#include "../Gen/AssetMetadata.hpp"
#include <memory>
#include <unordered_map>

namespace Pretop::Asset
{
    class GeneratedAssetCatalog : public AssetCatalog
    {
    public:
        GeneratedAssetCatalog(const std::unordered_map<uint64_t, FileMetadata> &metadata);
        const FileMetadata *Find(std::string_view assetSource) const override;

    private:
        std::unordered_map<uint64_t, FileMetadata> _metadata;
    };

    inline std::unique_ptr<GeneratedAssetCatalog> CreateGeneratedAssetCatalog()
    {
        return std::make_unique<GeneratedAssetCatalog>(Gen::AssetMetadata);
    }
}