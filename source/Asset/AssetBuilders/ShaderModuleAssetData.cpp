#include "ShaderModuleAssetData.hpp"

namespace Pretop::Asset
{
    void *ShaderModuleAssetData::GetData()
    {
        return &this->data;
    }

    ShaderModuleAssetData::~ShaderModuleAssetData() = default;
}