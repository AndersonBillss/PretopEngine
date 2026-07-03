#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include "appDevice.hpp"
#include "appBuffer.hpp"

class AppBindGroup
{
public:
    AppBindGroup(AppDevice *device,
                 WGPUBindGroupLayout &layout,
                 std::vector<WGPUBindGroupEntry> &bindGroupEntries);
    ~AppBindGroup();
    WGPUBindGroup wgpuBindGroup;
};