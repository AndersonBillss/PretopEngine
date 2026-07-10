#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include <vector>
#include <memory>
#include "Device.hpp"
#include "BindGroup.hpp"
#include "Buffer.hpp"

namespace Pretop::RHI
{
    class BindingLayout
    {
    public:
        BindingLayout(Device *device, std::initializer_list<std::initializer_list<WGPUBindGroupLayoutEntry>> layoutEntries);
        ~BindingLayout();

        WGPUPipelineLayout WgpuLayout;
        std::vector<WGPUBindGroupLayout> WgpuBindGroupLayouts;
    };
} // namespace Pretop::RHI
