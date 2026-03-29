#pragma once
#include <webgpu/webgpu.h>

class AppAdapter
{
public:
    AppAdapter(WGPUInstance instance);
    WGPUAdapter wgpuAdapter;

private:
    WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options);
};