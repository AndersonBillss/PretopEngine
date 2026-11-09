#include "ShaderModuleAssetBuilder.hpp"
#include <iostream>

namespace Pretop::Asset
{
    ShaderModuleAssetBuilder::ShaderModuleAssetBuilder(Core::GraphicsContext context)
    {
        this->_context = context;
    }

    void ShaderModuleAssetBuilder::ThreadLoadStep(const AssetBytes &bytes)
    {
        this->_bytes = &bytes;
    }

    std::unique_ptr<AssetData> ShaderModuleAssetBuilder::FinalizeLoadStep()
    {
        this->_assetData = std::make_unique<ShaderModuleAssetData>();
        this->_assetData->data = RHI::Shader::Pipeline(this->_context, this->_bytes->data(), this->_bytes->size());
        return std::move(this->_assetData);
    }
    AssetType ShaderModuleAssetBuilder::GetAssetType()
    {
        return AssetType::Shader;
    }
}