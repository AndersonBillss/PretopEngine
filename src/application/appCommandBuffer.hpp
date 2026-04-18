#pragma once
#include <vector>
#include <webgpu/webgpu.h>
#include <memory>
#include "./appDevice.hpp"
#include "./appRenderPassCommand.hpp"
#include "./appPipeline.hpp"
#include "appVertexBuffer.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice &device);
    ~AppCommandBuffer();
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppVertexBuffer<float> *> &vertexBuffers);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
