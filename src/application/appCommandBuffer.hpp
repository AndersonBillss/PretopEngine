#pragma once
#include <vector>
#include <webgpu/webgpu.h>
#include <memory>
#include "appDevice.hpp"
#include "appRenderPassCommand.hpp"
#include "appPipeline.hpp"
#include "appBindGroup.hpp"
#include "appBuffer.hpp"
#include "appBindGroup.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice &device);
    ~AppCommandBuffer();
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer *> &vertexBuffers,
        std::vector<std::unique_ptr<AppBindGroup>> &bindGroups);
    void addCommand(
        AppRenderPassCommand &command,
        AppPipeline &pipeline,
        std::vector<AppBuffer *> &vertexBuffers,
        AppBuffer &indexBuffer,
        std::vector<std::unique_ptr<AppBindGroup>> &bindGroups);
    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
