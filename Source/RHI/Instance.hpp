#pragma once
#include <webgpu/webgpu.h>

class AppInstance
{
public:
    AppInstance();
    WGPUInstance WgpuInstance;

private:
    WGPUInstance CreateInstance();
};