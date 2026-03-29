#include "appDevice.hpp"
#include <iostream>
#include "../printStringView.hpp"

AppDevice::AppDevice(WGPUInstance instance, AppAdapter adapter)
{
    WGPUDeviceDescriptor desc = createDeviceDescriptor(instance, adapter.wgpuAdapter);
    wgpuDevice = requestDeviceSync(instance, adapter.wgpuAdapter, &desc);
}


WGPUDevice AppDevice::requestDeviceSync(WGPUInstance instance, WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor)
{
    WGPUDevice device = nullptr;

    auto onDescriptorRequestEnded = [](
                                        WGPURequestDeviceStatus status,
                                        WGPUDevice device,
                                        WGPUStringView message,
                                        void *pDevice,
                                        void *_)
    {
        WGPUDevice *userDevice = reinterpret_cast<WGPUDevice *>(pDevice);
        if (status == WGPURequestDeviceStatus_Success)
        {
            *userDevice = device;
        }
        else
        {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
    };

    WGPURequestDeviceCallbackInfo info = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode::WGPUCallbackMode_WaitAnyOnly,
        /* callback */ onDescriptorRequestEnded,
        /* userdata 1 */ &device,
        /* userdata 2 */ nullptr,
    };
    WGPUFuture f = wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        info);
    WGPUFutureWaitInfo waitInfo = {f, 0};

    WGPUWaitStatus status = wgpuInstanceWaitAny(instance, 1, &waitInfo, UINT64_MAX);
    return device;
}

void onDeviceLost(WGPUDevice const *device, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
{
    if (reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
        std::cout << "WGPU device lost: " << message << std::endl;
}
void onDeviceUncapturedError(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, void *, void *)
{
    std::cout << "WGPU device error: " << message << std::endl;
}
WGPUDeviceDescriptor AppDevice::createDeviceDescriptor(WGPUInstance instance, WGPUAdapter adapter)
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

    return deviceDescriptor;
}
