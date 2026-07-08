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
    class AppBindingLayout
    {
    public:
        AppBindingLayout(AppDevice *device, std::initializer_list<std::initializer_list<WGPUBindGroupLayoutEntry>> layoutEntries);
        ~AppBindingLayout();

        WGPUPipelineLayout WgpuLayout;
        std::vector<WGPUBindGroupLayout> WgpuBindGroupLayouts;
    };
} // namespace Pretop::RHI
