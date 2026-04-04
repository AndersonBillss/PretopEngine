#pragma once
#include <webgpu/webgpu.h>
#include "appDevice.hpp"
#include "appPipeline.hpp"

class AppCommand
{
public:
    AppCommand(AppDevice &device, AppPipeline &pipeline, WGPUTextureView targetView);
    ~AppCommand();
    WGPUCommandEncoder wgpuEncoder;
    WGPUCommandBuffer wgpuCommand;
};