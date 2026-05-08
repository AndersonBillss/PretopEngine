#include "appBindingLayout.hpp"

AppBindingLayout::AppBindingLayout(AppDevice &device, std::initializer_list<std::initializer_list<uint32_t>> sizes)
{
    for (const auto &bindGroupSize : sizes)
    {
        std::vector<WGPUBindGroupLayoutEntry> bindingLayouts;
        size_t i = 0;
        for (const auto &size : bindGroupSize)
        {
            WGPUBindGroupLayoutEntry bindingLayout = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
            bindingLayout.binding = i;
            bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
            bindingLayout.buffer.minBindingSize = size;
            bindingLayout.visibility = WGPUShaderStage_Vertex;
            bindingLayouts.push_back(bindingLayout);
            i++;
        }
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
        bindGroupLayoutDesc.entryCount = bindingLayouts.size();
        bindGroupLayoutDesc.entries = bindingLayouts.data();
        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(device.wgpuDevice, &bindGroupLayoutDesc);
        wgpuBindGroupLayouts.push_back(bindGroupLayout);
    }

    WGPUPipelineLayoutDescriptor layoutDesc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layoutDesc.bindGroupLayoutCount = wgpuBindGroupLayouts.size();
    layoutDesc.bindGroupLayouts = wgpuBindGroupLayouts.data();
    this->wgpuLayout = wgpuDeviceCreatePipelineLayout(device.wgpuDevice, &layoutDesc);
}

AppBindingLayout::~AppBindingLayout()
{
    wgpuPipelineLayoutRelease(this->wgpuLayout);
    for (const auto &bindGroupLayout : this->wgpuBindGroupLayouts)
    {
        wgpuBindGroupLayoutRelease(bindGroupLayout);
    }
}

std::vector<std::unique_ptr<AppBindGroup>> AppBindingLayout::createBindGroups(
    AppDevice &device,
    std::initializer_list<std::initializer_list<AppBuffer *>> bufs)
{
    std::vector<std::unique_ptr<AppBindGroup>> result;
    size_t currIndex = 0;
    for (const auto &buf : bufs)
    {
        result.push_back(
            std::make_unique<AppBindGroup>(device, currIndex, this->wgpuBindGroupLayouts[currIndex], buf));
        currIndex++;
    }
    return result;
}
