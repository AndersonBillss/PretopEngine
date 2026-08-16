#pragma once
#include <webgpu/webgpu.h>
#include <vector>
#include <functional>
#include "Adapter.hpp"
#include "Instance.hpp"

namespace Pretop::RHI
{
    class Device
    {
    public:
        using RequestDeviceCallback = std::function<void(std::unique_ptr<Device>)>;
        Device(WGPUDevice device);
        static void Request(const Instance *instance, const Adapter *adapter, RequestDeviceCallback callback);
        void Inspect();
        WGPUDevice WgpuDevice;

    private:
        static WGPUDeviceDescriptor CreateDeviceDescriptor(const Instance *instance, const Adapter *adapter);
    };
} // namespace Pretop::RHI
