#pragma once
#include "../AssetData.hpp"
#include "../LoadGlb.hpp"

namespace Pretop::Asset
{
    struct GlbAssetData : AssetData
    {
        ParsedData data;
        void *GetData() override;
        ~GlbAssetData() override;
    };
}
