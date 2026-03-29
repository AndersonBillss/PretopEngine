#pragma once
#include <webgpu/webgpu.h>
#include "appAdapter.hpp"
#include "appInstance.hpp"

class AppDevice
{
public:
    WGPUDevice wgpuDevice;
    AppDevice(AppInstance instance, AppAdapter adapter);

private:
    WGPUDevice requestDeviceSync(
        WGPUInstance instance,
        WGPUAdapter adapter,
        WGPUDeviceDescriptor const *descriptor);

    WGPUDeviceDescriptor createDeviceDescriptor(WGPUInstance instance, WGPUAdapter adapter);
};
