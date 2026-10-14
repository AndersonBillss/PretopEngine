#pragma once
#include "AssetBuilder.hpp"
#include "ShaderModuleAssetData.hpp"

namespace Pretop::Asset
{
    class ShaderModuleAssetBuilder : AssetBuilder
    {
    private:
        std::unique_ptr<ShaderModuleAssetData> _assetData;
        std::unique_ptr<RHI::Shader> _data;
        Core::GraphicsContext _context;

    public:
        void ThreadLoadStep(const AssetBytes &bytes) override;
        std::unique_ptr<AssetData> FinalizeLoadStep() override;
    };
}
