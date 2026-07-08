#pragma once
#include <webgpu/webgpu.h>
#include "Device.hpp"
#include "Pipeline.hpp"

namespace Pretop::RHI
{
    class AppRenderPassCommand
    {
    public:
        AppRenderPassCommand(
            AppDevice *device,
            WGPUTextureView targetView,
            WGPURenderPassDepthStencilAttachment &depthStencilAttachment);
        WGPURenderPassDescriptor WgpuRenderPassDescriptor;
        WGPURenderPassColorAttachment WgpuRenderPassColorAttachment;
    };
} // namespace Pretop::RHI
