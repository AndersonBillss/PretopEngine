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
        std::vector<AppBuffer<float> *> &vertexBuffers);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer<float> *> &vertexBuffers,
        AppBuffer<uint16_t> &indexBuffer);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer<float> *> &vertexBuffers,
        AppBuffer<uint32_t> &indexBuffer);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
