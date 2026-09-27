#pragma once
#include "AssetType.hpp"

namespace Pretop::Asset
{
    struct AssetData
    {
        virtual AssetType GetAssetType() = 0;
        virtual ~AssetData() = default;
    };
}
