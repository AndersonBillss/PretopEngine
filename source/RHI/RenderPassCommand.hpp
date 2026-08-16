#pragma once
#include <webgpu/webgpu.h>
#include "Device.hpp"
#include "Pipeline.hpp"

namespace Pretop::RHI
{
    class RenderPassCommand
    {
    public:
        RenderPassCommand(
            Device *device,
            WGPUTextureView targetView,
            WGPURenderPassDepthStencilAttachment &depthStencilAttachment);
        WGPURenderPassDescriptor WgpuRenderPassDescriptor;
        WGPURenderPassColorAttachment WgpuRenderPassColorAttachment;
    };
} // namespace Pretop::RHI
