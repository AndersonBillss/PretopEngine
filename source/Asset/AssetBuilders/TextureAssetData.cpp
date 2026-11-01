#include "TextureAssetData.hpp"

namespace Pretop::Asset
{
    TextureAssetData::~TextureAssetData()
    {
        wgpuTextureDestroy(this->data);
    };
}
