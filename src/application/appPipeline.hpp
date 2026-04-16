#pragma once
#include <string>
#include "appDevice.hpp"
#include "appShader.hpp"

class AppPipeline
{
public:
    AppPipeline(AppDevice device, AppShader shader, WGPUTextureFormat format, WGPUBuffer buf);
    ~AppPipeline();

    WGPURenderPipeline wgpuPipeline;
};
