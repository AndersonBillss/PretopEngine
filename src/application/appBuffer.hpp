#pragma once
#include <cstdint>
#include <type_traits>
#include <webgpu/webgpu.h>
#include <cstring>

class AppBuffer
{
public:
    template <typename T>
    AppBuffer(AppDevice &device, std::initializer_list<std::initializer_list<T>> data, WGPUBufferUsage usage)
    {
        std::vector<T> vec = std::vector<T>();
        this->_unitSize = sizeof(T);
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
                vec.push_back(item);
            }
            i++;
        }
        vec.resize(_ceilFour(vec.size()));
        this->_unitCount = vec.size();
        this->_data = std::malloc(vec.size() * sizeof(T));
        std::memcpy(this->_data, vec.data(), vec.size() * sizeof(T));

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
        delete[] this->_data;
        wgpuBufferRelease(this->wgpuBuffer);
    }

    size_t numBytes() const
    {
        return _unitCount * _unitSize;
    }
    size_t count() const
    {
        return _unitCount;
    }

    size_t numRows() const
    {
        return this->_unitCount / _stride;
    }
    size_t stride()
    {
        return _stride;
    }

    const void *data() const
    {
        return this->_data;
    }

    void *rawData() const
    {
        return this->_data;
    }

    WGPUBuffer wgpuBuffer;

private:
    void *_data;
    size_t _unitSize;
    size_t _unitCount;
    size_t _stride;

    size_t _ceilFour(size_t n)
    {
        size_t numBytes = n * this->_unitSize;
        size_t roundedUp = (numBytes + 3) & ~3;
        return roundedUp / this->_unitSize;
    }
};