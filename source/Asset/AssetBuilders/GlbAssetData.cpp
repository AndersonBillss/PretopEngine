#include "GlbAssetData.hpp"

namespace Pretop::Asset
{
    AssetType Pretop::Asset::GlbAssetData::GetAssetType()
    {
        return AssetType::GLB;
    }

    GlbAssetData::~GlbAssetData() = default;
}
