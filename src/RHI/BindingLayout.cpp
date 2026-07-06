#include "BindingLayout.hpp"

AppBindingLayout::AppBindingLayout(
    AppDevice *device, std::initializer_list<std::initializer_list<WGPUBindGroupLayoutEntry>> layoutEntries)
{
    for (const auto &bindGroupLayouts : layoutEntries)
    {
        std::vector<WGPUBindGroupLayoutEntry> bindingLayouts;
        for (const auto &entry : bindGroupLayouts)
        {
            bindingLayouts.push_back(entry);
        }
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
        bindGroupLayoutDesc.entryCount = bindingLayouts.size();
        bindGroupLayoutDesc.entries = bindingLayouts.data();
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device->wgpuDevice, &bindGroupLayoutDesc);
        wgpuBindGroupLayouts.push_back(bindGroupLayout);
    }

    WGPUPipelineLayoutDescriptor layoutDesc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layoutDesc.bindGroupLayoutCount = wgpuBindGroupLayouts.size();
    layoutDesc.bindGroupLayouts = wgpuBindGroupLayouts.data();
    this->wgpuLayout = wgpuDeviceCreatePipelineLayout(device->wgpuDevice, &layoutDesc);
}

AppBindingLayout::~AppBindingLayout()
{
    wgpuPipelineLayoutRelease(this->wgpuLayout);
    for (const auto &bindGroupLayout : this->wgpuBindGroupLayouts)
    {
        wgpuBindGroupLayoutRelease(bindGroupLayout);
    }
}
