#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include "appDevice.hpp"
#include "appBuffer.hpp"

class AppBindGroup
{
public:
    AppBindGroup(AppDevice &device, size_t startBinding, WGPUBindGroupLayout &layout, std::initializer_list<AppBuffer *> bufs);
    ~AppBindGroup();
    WGPUBindGroup wgpuBindGroup;
};