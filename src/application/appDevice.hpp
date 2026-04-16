#pragma once
#include <webgpu/webgpu.h>
#include <vector>
#include "appAdapter.hpp"
#include "appInstance.hpp"

class AppDevice
{
public:
    WGPUDevice wgpuDevice;
    AppDevice(const AppInstance &instance, const AppAdapter &adapter);

    void inspect();

private:
    WGPUDevice _requestDeviceSync(
        const AppInstance &instance,
        const AppAdapter &adapter,
        WGPUDeviceDescriptor const *descriptor);

    WGPUDeviceDescriptor _createDeviceDescriptor(const AppInstance &instance, const AppAdapter &adapter);
};
