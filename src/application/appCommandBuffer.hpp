#pragma once
#include <vector>
#include <webgpu/webgpu.h>
#include <memory>
#include "./appDevice.hpp"
#include "./appRenderPassCommand.hpp"
#include "./appPipeline.hpp"
#include "appVertexBuffer.hpp"
#include "appIndexBuffer.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice &device);
    ~AppCommandBuffer();
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppVertexBuffer<float> *> &vertexBuffers);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppVertexBuffer<float> *> &vertexBuffers,
        AppIndexBuffer<uint16_t> &indexBuffer);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppVertexBuffer<float> *> &vertexBuffers,
        AppIndexBuffer<uint32_t> &indexBuffer);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
