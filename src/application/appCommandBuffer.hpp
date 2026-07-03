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
#include "appRenderPassEncoder.hpp"

class AppCommandBuffer
{
public:
    AppCommandBuffer(AppDevice *device);
    ~AppCommandBuffer();
    std::unique_ptr<AppRenderPassEncoder> addCommand(AppRenderPassCommand &command);

    void finish();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuBuffer;
};
