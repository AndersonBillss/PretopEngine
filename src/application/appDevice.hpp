#pragma once
#include <webgpu/webgpu.h>
#include "appAdapter.hpp"
#include "appInstance.hpp"

class AppDevice
{
public:
    WGPUDevice wgpuDevice;
    AppDevice(AppInstance instance, AppAdapter adapter);

    void inspect();

private:
    WGPUDevice _requestDeviceSync(
        WGPUInstance instance,
        WGPUAdapter adapter,
        WGPUDeviceDescriptor const *descriptor);

    WGPUDeviceDescriptor _createDeviceDescriptor(WGPUInstance instance, WGPUAdapter adapter);
};
