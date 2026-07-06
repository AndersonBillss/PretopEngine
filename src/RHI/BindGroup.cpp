#include <vector>
#include <iostream>
#include "BindGroup.hpp"

AppBindGroup::AppBindGroup(
    AppDevice *device,
    WGPUBindGroupLayout &layout,
    std::vector<WGPUBindGroupEntry> &bindGroupEntries)
{
    WGPUBindGroupDescriptor bindGroupDesc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = layout;
    bindGroupDesc.entryCount = bindGroupEntries.size();
    bindGroupDesc.entries = bindGroupEntries.data();
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device->wgpuDevice, &bindGroupDesc);

    this->wgpuBindGroup = wgpuDeviceCreateBindGroup(device->wgpuDevice, &bindGroupDesc);
}

AppBindGroup::~AppBindGroup()
{
    wgpuBindGroupRelease(wgpuBindGroup);
}
