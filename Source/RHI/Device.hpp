#pragma once
#include <webgpu/webgpu.h>
#include <vector>
#include <functional>
#include "Adapter.hpp"
#include "Instance.hpp"

namespace Pretop::RHI
{
    class AppDevice
    {
    public:
        using RequestDeviceCallback = std::function<void(std::unique_ptr<AppDevice>)>;
        AppDevice(WGPUDevice device);
        static void Request(const AppInstance *instance, const AppAdapter *adapter, RequestDeviceCallback callback);
        void Inspect();
        WGPUDevice WgpuDevice;

    private:
        static WGPUDeviceDescriptor CreateDeviceDescriptor(const AppInstance *instance, const AppAdapter *adapter);
    };
} // namespace Pretop::RHI
