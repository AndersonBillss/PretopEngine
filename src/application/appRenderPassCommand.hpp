#pragma once
#include <webgpu/webgpu.h>
#include "appDevice.hpp"
#include "appPipeline.hpp"

class AppRenderPassCommand
{
public:
    AppRenderPassCommand(
        AppDevice &device,
        WGPUTextureView targetView,
        WGPURenderPassDepthStencilAttachment &depthStencilAttachment);
    WGPURenderPassDescriptor wgpuRenderPassDescriptor;
    WGPURenderPassColorAttachment wgpuRenderPassColorAttachment;
};