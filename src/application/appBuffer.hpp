#pragma once
#include <cstdint>
#include <type_traits>
#include <webgpu/webgpu.h>

template <typename T>
class AppBuffer
{
public:
    AppBuffer(AppDevice &device, std::initializer_list<std::initializer_list<T>> data, WGPUBufferUsage usage)
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
        this->_vec.resize(ceilFour(this->_vec.size()));

        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        const std::string bufferLabel = "Test index buffer";
        bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
        bufferDesc.usage = WGPUBufferUsage_CopyDst | usage;
        bufferDesc.size = this->numBytes();
        bufferDesc.mappedAtCreation = false;
        this->wgpuBuffer = wgpuDeviceCreateBuffer(device.wgpuDevice, &bufferDesc);
    }

    ~AppBuffer()
    {
        wgpuBufferRelease(this->wgpuBuffer);
    }

    size_t numBytes() const
    {
        return _vec.size() * sizeof(T);
    }
    size_t count() const
    {
        return _vec.size();
    }

    size_t numRows() const
    {
        return _vec.size() / _stride;
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

    size_t ceilFour(size_t n)
    {
        size_t numBytes = n * sizeof(T);
        size_t roundedUp = (numBytes + 3) & ~3;
        return roundedUp / sizeof(T);
    }
};