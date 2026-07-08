#include "BindingLayout.hpp"

namespace Pretop::RHI
{
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
            WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device->WgpuDevice, &bindGroupLayoutDesc);
            WgpuBindGroupLayouts.push_back(bindGroupLayout);
        }

        WGPUPipelineLayoutDescriptor layoutDesc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
        layoutDesc.bindGroupLayoutCount = WgpuBindGroupLayouts.size();
        layoutDesc.bindGroupLayouts = WgpuBindGroupLayouts.data();
        this->WgpuLayout = wgpuDeviceCreatePipelineLayout(device->WgpuDevice, &layoutDesc);
    }

    AppBindingLayout::~AppBindingLayout()
    {
        wgpuPipelineLayoutRelease(this->WgpuLayout);
        for (const auto &bindGroupLayout : this->WgpuBindGroupLayouts)
        {
            wgpuBindGroupLayoutRelease(bindGroupLayout);
        }
    }
} // namespace Pretop::RHI
