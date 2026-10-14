#include "ShaderModuleAssetData.hpp"

namespace Pretop::Asset
{
    AssetType Pretop::Asset::ShaderModuleAssetData::GetAssetType()
    {
        return AssetType::GLB;
    }

    ShaderModuleAssetData::~ShaderModuleAssetData() = default;
}