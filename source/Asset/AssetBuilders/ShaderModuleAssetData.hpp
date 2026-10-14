#pragma once
#include "../AssetData.hpp"
#include "../../RHI/Shader.hpp"

namespace Pretop::Asset
{
    struct ShaderModuleAssetData : AssetData
    {
        RHI::Shader data;
        AssetType GetAssetType() override;
        ~ShaderModuleAssetData() override;
    };
}