#pragma once
#include "../AssetData.hpp"
#include "../../RHI/Shader.hpp"

namespace Pretop::Asset
{
    struct TextureAssetData : AssetData
    {
        WGPUTexture data;
        void *GetData() override;
        ~TextureAssetData() override;
    };
}
