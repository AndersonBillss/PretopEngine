#pragma once
#include <webgpu/webgpu.h>
#include "appInstance.hpp"

class AppAdapter
{
public:
    AppAdapter(AppInstance instance);
    WGPUAdapter wgpuAdapter;

private:
    WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
};