#pragma once
#include "../AssetData.hpp"
#include "../../RHI/Shader.hpp"

namespace Pretop::Asset
{
    struct ShaderModuleAssetData : AssetData
    {
        RHI::Shader data;
        void *GetData() override;
        ~ShaderModuleAssetData() override;
    };
}