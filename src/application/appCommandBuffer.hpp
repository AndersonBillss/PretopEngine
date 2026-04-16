#pragma once
#include <webgpu/webgpu.h>
#include "./appDevice.hpp"
#include "./appRenderPassCommand.hpp"
#include "./appPipeline.hpp"
#include "appVertexBuffer.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice &device);
    ~AppCommandBuffer();
    void addCommand(AppRenderPassCommand &command, AppPipeline &pipeline, AppVertexBuffer<float> &vertexBuffer);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
