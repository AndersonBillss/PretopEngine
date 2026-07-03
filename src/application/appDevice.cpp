#include "appDevice.hpp"
#include <iostream>
#include "../printStringView.hpp"
#include "../limits/limits.hpp"

AppDevice::AppDevice(WGPUDevice device)
{
    this->wgpuDevice = device;
}

void AppDevice::request(const AppInstance *instance, const AppAdapter *adapter, RequestDeviceCallback cb)
{
    auto onDescriptorRequestEnded = [](
                                        WGPURequestDeviceStatus status,
                                        WGPUDevice device,
                                        WGPUStringView message,
                                        void *pCb,
                                        void *_)
    {
        RequestDeviceCallback userCb = *reinterpret_cast<RequestDeviceCallback *>(pCb);
        if (status == WGPURequestDeviceStatus_Success)
        {
            std::unique_ptr<AppDevice> appDevice = std::make_unique<AppDevice>(device);
            userCb(std::move(appDevice));
        }
        else
        {
            std::cout << "Could not get WebGPU device: " << message << std::endl;
        }
    };

    WGPUDeviceDescriptor descriptor = _createDeviceDescriptor(instance, adapter);
    WGPURequestDeviceCallbackInfo info = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode::WGPUCallbackMode_AllowSpontaneous,
        /* callback */ onDescriptorRequestEnded,
        /* userdata 1 */ &cb,
        /* userdata 2 */ nullptr,
    };
    wgpuAdapterRequestDevice(
        adapter->wgpuAdapter,
        &descriptor,
        info);
}

void AppDevice::inspect()
{
    WGPULimits limits = WGPU_LIMITS_INIT;
    bool success = wgpuDeviceGetLimits(this->wgpuDevice, &limits) == WGPUStatus_Success;
    if (success)
    {
        std::cout << "\nDevice limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
    }
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
WGPUDeviceDescriptor AppDevice::_createDeviceDescriptor(const AppInstance *instance, const AppAdapter *adapter)
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
