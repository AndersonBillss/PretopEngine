#include <stb_image.h>
#include "TextureAssetBuilder.hpp"

namespace Pretop::Asset
{
    void TextureAssetBuilder::ThreadLoadStep(const AssetBytes &bytes)
    {
        this->pixelData = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(bytes.data()),
            bytes.size(),
            &this->width,
            &this->height,
            &this->channels,
            4);
    }

    std::unique_ptr<AssetData> TextureAssetBuilder::FinalizeLoadStep()
    {
        WGPUTextureDescriptor textureDesc = WGPU_TEXTURE_DESCRIPTOR_INIT;
        textureDesc.nextInChain = nullptr;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;
        textureDesc.size = {(unsigned int)this->width, (unsigned int)this->height, 1};
        textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
        textureDesc.viewFormatCount = 0;
        textureDesc.viewFormats = nullptr;
        WGPUTexture texture = wgpuDeviceCreateTexture(this->_context.Device, &textureDesc);

        WGPUTexelCopyTextureInfo destination = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
        destination.texture = texture;
        destination.mipLevel = 0;
        destination.origin = {/*.width=*/0, /*.height=*/0, /*.depthOrArrayLayers=*/0};
        destination.aspect = WGPUTextureAspect_All;

        WGPUTexelCopyBufferLayout dataLayout = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
        dataLayout.offset = 0;
        dataLayout.bytesPerRow = 4 * this->width;
        dataLayout.rowsPerImage = this->height;

        WGPUExtent3D size{/*.width=*/static_cast<uint32_t>(this->width),
                          /*.height=*/static_cast<uint32_t>(this->height),
                          /*.depthOrArrayLayers=*/1};

        wgpuQueueWriteTexture(
            this->_context.Queue,
            &destination,
            this->pixelData,
            4 * this->width * this->height,
            &dataLayout,
            &size);

        stbi_image_free(this->pixelData);

        this->_assetData->data = texture;
        return std::move(this->_assetData);
    }
}