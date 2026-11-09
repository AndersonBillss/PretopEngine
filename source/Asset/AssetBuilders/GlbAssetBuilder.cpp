#include "GlbAssetBuilder.hpp"
#include <iostream>

namespace Pretop::Asset
{
    void GlbAssetBuilder::ThreadLoadStep(const AssetBytes &bytes)
    {
        this->_assetData = std::make_unique<GlbAssetData>();
        this->_assetData->data = LoadGlb(bytes);
    }

    std::unique_ptr<AssetData> GlbAssetBuilder::FinalizeLoadStep()
    {
        std::cout << "GlbAssetBuilder::FinalizeLoadStep()" << std::endl;
        return std::move(this->_assetData);
    }
    AssetType GlbAssetBuilder::GetAssetType()
    {
        return AssetType::GLB;
    }
}