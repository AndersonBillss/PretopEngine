#include "ShaderModuleAssetBuilder.hpp"

namespace Pretop::Asset
{
    void ShaderModuleAssetBuilder::ThreadLoadStep(const AssetBytes &bytes)
    {
        this->_assetData->data = RHI::Shader::Pipeline(this->_context, bytes.data(), bytes.size());
    }

    std::unique_ptr<AssetData> ShaderModuleAssetBuilder::FinalizeLoadStep()
    {
        return std::move(this->_assetData);
    }
}