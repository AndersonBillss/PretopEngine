#pragma once
#include <cstdint>
#include <type_traits>
#include <webgpu/webgpu.h>
#include <cstring>
#include <string>
#include <cstddef>

#include "appDevice.hpp"

class AppBuffer
{
public:
    template <typename T>
    AppBuffer(AppDevice &device, std::initializer_list<std::initializer_list<T>> data, WGPUBufferUsage usage)
    {
        std::vector<T> vec = std::vector<T>();
        size_t i = 0;
        size_t stride = 0;
        for (auto &row : data)
        {
            if (i == 0)
            {
                stride = row.size();
            }
            if (row.size() != stride)
            {
                throw std::invalid_argument("Malformed list provided");
            }
            for (auto &item : row)
            {
                vec.push_back(item);
            }
            i++;
        }
        vec.resize(_ceilFour(vec.size(), sizeof(T)));
        size_t unitCount = vec.size();
        this->_data = (std::byte *)std::malloc(vec.size() * sizeof(T));
        this->_numBytes = vec.size() * sizeof(T);
        std::memcpy(this->_data, vec.data(), this->_numBytes);

        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        const std::string bufferLabel = "Test index buffer";
        bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
        bufferDesc.usage = WGPUBufferUsage_CopyDst | usage;
        bufferDesc.size = this->numBytes();
        bufferDesc.mappedAtCreation = false;
        this->wgpuBuffer = wgpuDeviceCreateBuffer(device.wgpuDevice, &bufferDesc);
    }

    template <typename T>
    AppBuffer(AppDevice &device, std::initializer_list<T> data, WGPUBufferUsage usage)
    {
        std::vector<T> vec = std::vector<T>();
        for (auto &item : data)
        {
            vec.push_back(item);
        }
        vec.resize(_ceilFour(vec.size(), sizeof(T)));
        size_t unitCount = vec.size();
        this->_data = (std::byte *)std::malloc(vec.size() * sizeof(T));
        this->_numBytes = vec.size() * sizeof(T);
        std::memcpy(this->_data, vec.data(), this->_numBytes);

        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        const std::string bufferLabel = "Test index buffer";
        bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
        bufferDesc.usage = WGPUBufferUsage_CopyDst | usage;
        bufferDesc.size = this->numBytes();
        bufferDesc.mappedAtCreation = false;
        this->wgpuBuffer = wgpuDeviceCreateBuffer(device.wgpuDevice, &bufferDesc);
    }

    AppBuffer(AppDevice &device, size_t size, WGPUBufferUsage usage)
    {
        this->_numBytes = _ceilFour(size);
        this->_data = (std::byte *)std::malloc(size);
        for (size_t i = 0; i < this->_numBytes; i++)
        {
            this->_data[i] = std::byte{0x0};
        }

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

    template <class T>
    inline T *get()
    {
        return (T *)this->_data;
    }

    template <class T>
    inline T *get(size_t offset)
    {
        return (T *)((uint8_t *)this->_data + 256);
    }

    template <class T>
    inline void set(T &data)
    {
        memcpy(this->_data, &data, sizeof(T));
    }

    size_t numBytes() const
    {
        return _numBytes;
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
    std::byte *_data;
    size_t _numBytes;

    size_t _ceilFour(size_t n, size_t unitSize)
    {
        size_t numBytes = n * unitSize;
        size_t roundedUp = _ceilFour(numBytes);
        return roundedUp / unitSize;
    }
    size_t _ceilFour(size_t numBytes)
    {
        return (numBytes + 3) & ~3;
    }
};