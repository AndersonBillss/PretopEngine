#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include "Device.hpp"
#include "Buffer.hpp"

namespace Pretop::RHI
{
    class BindGroup
    {
    public:
        BindGroup(Device *device,
                     WGPUBindGroupLayout &layout,
                     std::vector<WGPUBindGroupEntry> &bindGroupEntries);
        ~BindGroup();
        WGPUBindGroup WgpuBindGroup;
    };
} // namespace Pretop::RHI
