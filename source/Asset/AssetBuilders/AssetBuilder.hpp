#pragma once
#include <memory>
#include "../AssetBytes.hpp"
#include "../AssetData.hpp"

namespace Pretop::Asset
{
    class AssetBuilder
    {
    public:
        virtual void ThreadLoadStep(const AssetBytes &bytes) = 0;
        virtual std::unique_ptr<AssetData> FinalizeLoadStep() = 0;
    };
}
