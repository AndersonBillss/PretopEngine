#pragma once
#include <webgpu/webgpu.h>
#include "appInstance.hpp"

class AppAdapter
{
public:
    AppAdapter(AppInstance instance);
    WGPUAdapter wgpuAdapter;

    void inspect();

private:
    WGPUAdapter _requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
};