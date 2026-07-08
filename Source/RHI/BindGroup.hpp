#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include "Device.hpp"
#include "Buffer.hpp"

namespace Pretop::RHI
{
    class AppBindGroup
    {
    public:
        AppBindGroup(AppDevice *device,
                     WGPUBindGroupLayout &layout,
                     std::vector<WGPUBindGroupEntry> &bindGroupEntries);
        ~AppBindGroup();
        WGPUBindGroup WgpuBindGroup;
    };
} // namespace Pretop::RHI
