#include "GlbAssetBuilder.hpp"

namespace Pretop::Asset
{
    void GlbAssetBuilder::ThreadLoadStep(const AssetBytes &bytes)
    {
        this->_assetData = std::make_unique<GlbAssetData>();
        this->_assetData->data = LoadGlb(bytes);
    }

    std::unique_ptr<AssetData> GlbAssetBuilder::FinalizeLoadStep()
    {
        return std::move(this->_assetData);
    }
}