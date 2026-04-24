#pragma once
#include <cstdint>
#include <type_traits>
#include <webgpu/webgpu.h>

template <typename T>
class AppIndexBuffer
{
    static_assert(
        std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>,
        "T must be uint16_t or uint32_t");

public:
    AppIndexBuffer(AppDevice &device, std::initializer_list<std::initializer_list<T>> data)
    {
        this->_vec = std::vector<T>();
        size_t i = 0;
        for (auto &row : data)
        {
            if (i == 0)
            {
                this->_stride = row.size();
            }
            if (row.size() != this->_stride)
            {
                throw std::invalid_argument("Malformed list provided");
            }
            for (auto &item : row)
            {
                this->_vec.push_back(item);
            }
            i++;
        }

        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        const std::string bufferLabel = "Test buffer";
        bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
        bufferDesc.size = this->size();
        bufferDesc.mappedAtCreation = false;
        this->wgpuBuffer = wgpuDeviceCreateBuffer(device.wgpuDevice, &bufferDesc);
    }

    ~AppIndexBuffer()
    {
        wgpuBufferRelease(this->wgpuBuffer);
    }

    size_t size() const
    {
        return _vec.size() * sizeof(T);
    }
    size_t count()
    {
        return _vec.size();
    }
    size_t stride()
    {
        return _stride;
    }

    const void *data() const
    {
        return _vec.data();
    }

    WGPUBuffer wgpuBuffer;

private:
    std::vector<T> _vec;
    size_t _stride;
};