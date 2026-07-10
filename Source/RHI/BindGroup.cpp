#include <vector>
#include <iostream>
#include "BindGroup.hpp"

namespace Pretop::RHI
{
    BindGroup::BindGroup(
        Device *device,
        WGPUBindGroupLayout &layout,
        std::vector<WGPUBindGroupEntry> &bindGroupEntries)
    {
        WGPUBindGroupDescriptor bindGroupDesc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.layout = layout;
        bindGroupDesc.entryCount = bindGroupEntries.size();
        bindGroupDesc.entries = bindGroupEntries.data();
        WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device->WgpuDevice, &bindGroupDesc);

        this->WgpuBindGroup = wgpuDeviceCreateBindGroup(device->WgpuDevice, &bindGroupDesc);
    }

    BindGroup::~BindGroup()
    {
        wgpuBindGroupRelease(WgpuBindGroup);
    }
} // namespace Pretop::RHI
