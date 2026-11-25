#pragma once
#include "AssetType.hpp"

namespace Pretop::Asset
{
    struct AssetData
    {
        virtual void *GetData() = 0;
        virtual ~AssetData() = default;
    };
}
