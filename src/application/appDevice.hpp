#pragma once
#include <webgpu/webgpu.h>
#include <vector>
#include <functional>
#include "appAdapter.hpp"
#include "appInstance.hpp"

class AppDevice
{
public:
    using RequestDeviceCallback = std::function<void(std::unique_ptr<AppDevice>)>;
    AppDevice(WGPUDevice device);
    static void request(const AppInstance *instance, const AppAdapter *adapter, RequestDeviceCallback cb);
    void inspect();
    WGPUDevice wgpuDevice;

private:
    static WGPUDeviceDescriptor _createDeviceDescriptor(const AppInstance *instance, const AppAdapter *adapter);
};
