#pragma once
#include <webgpu/webgpu.h>
#include "appAdapter.hpp"

class AppDevice
{
public:
    WGPUDevice wgpuDevice;
    AppDevice(WGPUInstance instance, AppAdapter adapter);

private:
    WGPUDevice requestDeviceSync(
        WGPUInstance instance,
        WGPUAdapter adapter,
        WGPUDeviceDescriptor const *descriptor);

    WGPUDeviceDescriptor createDeviceDescriptor(WGPUInstance instance, WGPUAdapter adapter);
};
