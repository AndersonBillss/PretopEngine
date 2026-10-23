#pragma once
#include "../AssetData.hpp"
#include "../../RHI/Shader.hpp"

namespace Pretop::Asset
{
    struct TextureAssetData : AssetData
    {
        WGPUTexture data;
        AssetType GetAssetType() override;
        ~TextureAssetData() override;
    };
}
