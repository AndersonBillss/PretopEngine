#pragma once
#include <webgpu/webgpu.h>

namespace Pretop::Asset
{
    struct GPUTexture
    {
        WGPUTexture texture;
        GPUTexture(WGPUTexture texture)
        {
            this->texture = texture;
        }
        ~GPUTexture()
        {
            wgpuTextureDestroy(texture);
        }
    };
}
