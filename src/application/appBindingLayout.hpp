#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include <vector>
#include <memory>
#include "appDevice.hpp"
#include "appBindGroup.hpp"
#include "appBuffer.hpp"

class AppBindingLayout
{
public:
    AppBindingLayout(AppDevice &device, std::initializer_list<std::initializer_list<WGPUBindGroupLayoutEntry>> layoutEntries);
    ~AppBindingLayout();

    WGPUPipelineLayout wgpuLayout;
    std::vector<WGPUBindGroupLayout> wgpuBindGroupLayouts;
};