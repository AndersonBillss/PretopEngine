#pragma once
#include <cstdint>
#include <type_traits>
#include <webgpu/webgpu.h>
#include <cstring>
#include <string>
#include <cstddef>

#include "Device.hpp"

class AppBuffer
{
public:
    AppBuffer(AppDevice *device, size_t size, WGPUBufferUsage usage)
    {
        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        const std::string bufferLabel = "Test index buffer";
        bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
        bufferDesc.usage = usage;
        bufferDesc.size = size;
        bufferDesc.mappedAtCreation = false;
        this->WgpuBuffer = wgpuDeviceCreateBuffer(device->WgpuDevice, &bufferDesc);
    }

    ~AppBuffer()
    {
        wgpuBufferRelease(this->WgpuBuffer);
    }

    std::vector<uint8_t> ZeroBuffer() const
    {
        std::vector<uint8_t> result;
        size_t numBytes = wgpuBufferGetSize(this->WgpuBuffer);
        for (size_t i = 0; i < numBytes; i++)
        {
            result.push_back(0);
        }
        return result;
    }

    WGPUBuffer WgpuBuffer;
};