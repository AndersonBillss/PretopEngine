#pragma once
#include "../AssetBytes.hpp"
#include "../AssetData.hpp"
#include "../AssetType.hpp"

namespace Pretop::Asset
{
    class AssetBuilder
    {
        virtual AssetType GetAssetType() = 0;
        virtual void ThreadLoadStep(const AssetBytes &bytes) = 0;
        virtual AssetData FinalizeLoadStep() = 0;
    };
}
