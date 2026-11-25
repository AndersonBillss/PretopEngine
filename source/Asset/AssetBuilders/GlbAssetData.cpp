#include "GlbAssetData.hpp"

namespace Pretop::Asset
{
    void *GlbAssetData::GetData()
    {
        return &this->data;
    }
    GlbAssetData::~GlbAssetData() = default;
}
