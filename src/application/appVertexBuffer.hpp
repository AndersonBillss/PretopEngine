#pragma once
#include <webgpu/webgpu.h>
#include <initializer_list>
#include <vector>
#include <stdexcept>
#include "appDevice.hpp"

template <class T>
class AppVertexBuffer
{
public:
    AppVertexBuffer(AppDevice &device, std::initializer_list<std::initializer_list<T>> data)
    {
        this->_vec = std::vector<T>();
        size_t i = 0;
        for (auto &row : data)
        {
            if (i == 0)
            {
                this->_rowSize = row.size();
            }
            if (row.size() != this->_rowSize)
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
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        bufferDesc.size = this->size();
        bufferDesc.mappedAtCreation = false;
        this->wgpuBuffer = wgpuDeviceCreateBuffer(device.wgpuDevice, &bufferDesc);
    }

    ~AppVertexBuffer()
    {
        wgpuBufferRelease(this->wgpuBuffer);
    }

    size_t size()
    {
        return _vec.size() * sizeof(T);
    }

    void *data()
    {
        return _vec.data();
    }

    WGPUBuffer wgpuBuffer;

private:
    size_t _rowSize;
    std::vector<T> _vec;
};