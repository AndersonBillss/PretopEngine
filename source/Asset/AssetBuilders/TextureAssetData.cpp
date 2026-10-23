#include "TextureAssetData.hpp"

namespace Pretop::Asset
{
    AssetType Pretop::Asset::TextureAssetData::GetAssetType()
    {
        return AssetType::Texture;
    }

    TextureAssetData::~TextureAssetData()
    {
        wgpuTextureDestroy(this->data);
    };
}
