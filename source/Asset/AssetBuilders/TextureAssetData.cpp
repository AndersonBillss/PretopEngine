#include "TextureAssetData.hpp"

namespace Pretop::Asset
{
    void *TextureAssetData::GetData()
    {
        return &this->data;
    }

    TextureAssetData::~TextureAssetData()
    {
        wgpuTextureDestroy(this->data);
    };
}
