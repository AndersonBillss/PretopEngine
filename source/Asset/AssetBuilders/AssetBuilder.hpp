#pragma once
#include "../AssetBytes.hpp"
#include "../AssetData.hpp"

namespace Pretop::Asset
{
    class AssetBuilder
    {
        virtual void ThreadLoadStep(const AssetBytes &bytes) = 0;
        virtual AssetData FinalizeLoadStep() = 0;
    };
}
