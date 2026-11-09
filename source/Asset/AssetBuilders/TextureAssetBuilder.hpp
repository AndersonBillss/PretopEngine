#pragma once
#include "AssetBuilder.hpp"
#include "TextureAssetData.hpp"

namespace Pretop::Asset
{
    class TextureAssetBuilder : public AssetBuilder
    {
    private:
        std::unique_ptr<TextureAssetData> _assetData;

        int width;
        int height;
        int channels;
        unsigned char *pixelData;
        Core::GraphicsContext _context;

    public:
        TextureAssetBuilder::TextureAssetBuilder(Core::GraphicsContext context);
        void ThreadLoadStep(const AssetBytes &bytes) override;
        std::unique_ptr<AssetData> FinalizeLoadStep() override;
        AssetType GetAssetType() override;
    };
}
