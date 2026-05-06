#pragma once
#include <vector>
#include <webgpu/webgpu.h>
#include <memory>
#include "./appDevice.hpp"
#include "./appRenderPassCommand.hpp"
#include "./appPipeline.hpp"
#include "appBuffer.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice &device);
    ~AppCommandBuffer();
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer *> &vertexBuffers,
        WGPUBindGroup &bindGroup);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer *> &vertexBuffers,
        AppBuffer &indexBuffer,
        WGPUBindGroup &bindGroup);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
