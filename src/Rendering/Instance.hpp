#pragma once
#include <webgpu/webgpu.h>

class AppInstance
{
public:
    AppInstance();
    WGPUInstance wgpuInstance;

private:
    WGPUInstance createInstance();
};