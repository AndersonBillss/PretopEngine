#include "createDevice.hpp"
#include "../printStringView.hpp"
#include "requestDevice.hpp"
#include <iostream>

void onDeviceLost(WGPUDevice const *device, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
{
    if (reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
        std::cout << "WGPU device lost: " << message << std::endl;
}
void onDeviceUncapturedError(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, void *, void *)
{
    std::cout << "WGPU device error: " << message << std::endl;
}

WGPUDevice createDevice(WGPUInstance instance, WGPUAdapter adapter)
{
    WGPUDeviceDescriptor deviceDescriptor = WGPU_DEVICE_DESCRIPTOR_INIT;
    WGPUDeviceLostCallbackInfo deviceLostCb = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode_AllowSpontaneous,
        /* callback */ onDeviceLost,
        /* userdata1 */ nullptr,
        /* userdata2 */ nullptr,
    };
    deviceDescriptor.deviceLostCallbackInfo = deviceLostCb;

    WGPUUncapturedErrorCallbackInfo uncapturedCb = {
        /* nextInChain */ nullptr,
        /* callback */ onDeviceUncapturedError,
        /* userdata1 */ nullptr,
        /* userdata2 */ nullptr,
    };
    deviceDescriptor.uncapturedErrorCallbackInfo = uncapturedCb;

    return requestDeviceSync(instance, adapter, &deviceDescriptor);
}