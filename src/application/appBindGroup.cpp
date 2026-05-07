#include <vector>
#include <iostream>
#include "appBindGroup.hpp"

AppBindGroup::AppBindGroup(
    AppDevice &device,
    size_t startBinding,
    WGPUBindGroupLayout &layout,
    std::initializer_list<AppBuffer *> bufs)
{
    size_t i = startBinding;

    std::vector<WGPUBindGroupEntry> bindGroupEntries;
    for (const auto &buf : bufs)
    {
        WGPUBindGroupEntry binding = WGPU_BIND_GROUP_ENTRY_INIT;
        binding.binding = i;
        binding.buffer = buf->wgpuBuffer;
        binding.offset = 0;
        binding.size = buf->numBytes();
        bindGroupEntries.push_back(binding);
        i++;
    }

    WGPUBindGroupDescriptor bindGroupDesc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = layout;
    bindGroupDesc.entryCount = bindGroupEntries.size();
    bindGroupDesc.entries = bindGroupEntries.data();
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(device.wgpuDevice, &bindGroupDesc);

    this->wgpuBindGroup = wgpuDeviceCreateBindGroup(device.wgpuDevice, &bindGroupDesc);
}

AppBindGroup::~AppBindGroup()
{
    wgpuBindGroupRelease(wgpuBindGroup);
}
