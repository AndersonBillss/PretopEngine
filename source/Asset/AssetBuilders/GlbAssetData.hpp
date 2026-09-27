#pragma once
#include "../AssetData.hpp"
#include "../LoadGlb.hpp"

namespace Pretop::Asset
{
    struct GlbAssetData : AssetData
    {
        ParsedData data;
        AssetType GetAssetType() override;
        ~GlbAssetData() override;
    };
}
