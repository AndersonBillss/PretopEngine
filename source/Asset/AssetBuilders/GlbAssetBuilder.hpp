#pragma once
#include "AssetBuilder.hpp"
#include "GlbAssetData.hpp"

namespace Pretop::Asset
{
    class GlbAssetBuilder : AssetBuilder
    {
    private:
        std::unique_ptr<GlbAssetData> _assetData;

    public:
        void ThreadLoadStep(const AssetBytes &bytes) override;
        std::unique_ptr<AssetData> FinalizeLoadStep() override;
    };
}
