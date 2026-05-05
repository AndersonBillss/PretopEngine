#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include "appDevice.hpp"

class AppBindingLayout
{
public:
    AppBindingLayout(AppDevice &device, std::initializer_list<std::initializer_list<uint32_t>> sizes);
    ~AppBindingLayout();
    WGPUPipelineLayout wgpuLayout;
    std::vector<WGPUBindGroupLayout> wgpuBindGroupLayouts;
};