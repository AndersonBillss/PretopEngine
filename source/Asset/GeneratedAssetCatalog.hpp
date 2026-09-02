#pragma once
#include "AssetCatalog.hpp"
#include <memory>

namespace Pretop::Asset
{
    class GeneratedAssetCatalog : public AssetCatalog
    {
    public:
        GeneratedAssetCatalog(const std::unordered_map<uint64_t, Pretop::Gen::FileMetadata> &metadata);
        const Gen::FileMetadata *Find(std::string_view assetSource) const override;

    private:
        std::unordered_map<uint64_t, Pretop::Gen::FileMetadata> _metadata;
    };

    inline std::unique_ptr<GeneratedAssetCatalog> CreateGeneratedAssetCatalog()
    {
        return std::make_unique<GeneratedAssetCatalog>(Gen::AssetMetadata);
    }
}