#include "appDevice.hpp"
#include <iostream>
#include "../printStringView.hpp"

AppDevice::AppDevice(const AppInstance &instance, const AppAdapter &adapter)
{
    WGPUDeviceDescriptor desc = _createDeviceDescriptor(instance, adapter);
    WGPULimits requiredLimits = WGPU_LIMITS_INIT;
    requiredLimits.maxVertexAttributes = 1;
    requiredLimits.maxVertexBuffers = 1;
    requiredLimits.maxBufferSize = 6 * 2 * sizeof(float);
    requiredLimits.maxVertexBufferArrayStride = 2 * sizeof(float);
    desc.requiredLimits = &requiredLimits;
    wgpuDevice = _requestDeviceSync(instance, adapter, &desc);
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

WGPUDevice AppDevice::_requestDeviceSync(const AppInstance &instance,
                                         const AppAdapter &adapter,
                                         WGPUDeviceDescriptor const *descriptor)
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
        adapter.wgpuAdapter,
        descriptor,
        info);
    WGPUFutureWaitInfo waitInfo = {f, 0};

    WGPUWaitStatus status = wgpuInstanceWaitAny(instance.wgpuInstance, 1, &waitInfo, UINT64_MAX);
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
WGPUDeviceDescriptor AppDevice::_createDeviceDescriptor(const AppInstance &instance, const AppAdapter &adapter)
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
