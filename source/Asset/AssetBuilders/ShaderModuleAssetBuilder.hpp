#pragma once
#include "AssetBuilder.hpp"
#include "ShaderModuleAssetData.hpp"

namespace Pretop::Asset
{
    class ShaderModuleAssetBuilder : public AssetBuilder
    {
    private:
        const AssetBytes *_bytes;
        std::unique_ptr<ShaderModuleAssetData> _assetData;
        std::unique_ptr<RHI::Shader> _data;
        Core::GraphicsContext _context;

    public:
        ShaderModuleAssetBuilder::ShaderModuleAssetBuilder(Core::GraphicsContext context);
        void ThreadLoadStep(const AssetBytes &bytes) override;
        std::unique_ptr<AssetData> FinalizeLoadStep() override;
        AssetType GetAssetType() override;
    };
}
