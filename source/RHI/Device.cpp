#include "Device.hpp"
#include <iostream>
#include "../PrintStringView.hpp"
#include "../Limits/Limits.hpp"

namespace Pretop::RHI
{
    Device::Device(WGPUDevice device)
    {
        this->WgpuDevice = device;
    }

    struct DeviceRequestUserData
    {
        Device::RequestDeviceCallback Callback;
    };
    void Device::Request(const Instance *instance, const Adapter *adapter, RequestDeviceCallback callback)
    {
        auto onDescriptorRequestEnded = [](
                                            WGPURequestDeviceStatus status,
                                            WGPUDevice device,
                                            WGPUStringView message,
                                            void *userDataPointer,
                                            void *)
        {
            DeviceRequestUserData *userData = reinterpret_cast<DeviceRequestUserData *>(userDataPointer);
            if (status == WGPURequestDeviceStatus_Success)
            {
                std::unique_ptr<Device> appDevice = std::make_unique<Device>(device);
                userData->Callback(std::move(appDevice));
                delete userData;
            }
            else
            {
                std::cout << "Could not get WebGPU device: " << message << std::endl;
            }
        };

        WGPUDeviceDescriptor descriptor = CreateDeviceDescriptor(instance, adapter);
        auto *userData = new DeviceRequestUserData{callback};
        WGPURequestDeviceCallbackInfo info = {
            /* nextInChain */ nullptr,
            /* mode */ WGPUCallbackMode::WGPUCallbackMode_AllowSpontaneous,
            /* callback */ onDescriptorRequestEnded,
            /* userdata 1 */ userData,
            /* userdata 2 */ nullptr,
        };
        wgpuAdapterRequestDevice(
            adapter->WgpuAdapter,
            &descriptor,
            info);
    }

    void Device::Inspect()
    {
        WGPULimits limits = WGPU_LIMITS_INIT;
        bool success = wgpuDeviceGetLimits(this->WgpuDevice, &limits) == WGPUStatus_Success;
        if (success)
        {
            std::cout << "\nDevice limits:" << std::endl;
            std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
            std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
            std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
            std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
        }
    }

    void OnDeviceLost(WGPUDevice const *, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
    {
        if (reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
            std::cout << "WGPU device lost: " << message << std::endl;
    }
    void OnDeviceUncapturedError(WGPUDevice const *, WGPUErrorType, WGPUStringView message, void *, void *)
    {
        std::cout << "WGPU device error: " << message << std::endl;
    }
    WGPUDeviceDescriptor Device::CreateDeviceDescriptor(const Instance *instance, const Adapter *adapter)
    {
        WGPUDeviceDescriptor deviceDescriptor = WGPU_DEVICE_DESCRIPTOR_INIT;
        WGPUDeviceLostCallbackInfo deviceLostCb = {
            /* nextInChain */ nullptr,
            /* mode */ WGPUCallbackMode_AllowSpontaneous,
            /* callback */ OnDeviceLost,
            /* userdata1 */ nullptr,
            /* userdata2 */ nullptr,
        };
        deviceDescriptor.deviceLostCallbackInfo = deviceLostCb;

        WGPUUncapturedErrorCallbackInfo uncapturedCb = {
            /* nextInChain */ nullptr,
            /* callback */ OnDeviceUncapturedError,
            /* userdata1 */ nullptr,
            /* userdata2 */ nullptr,
        };
        deviceDescriptor.uncapturedErrorCallbackInfo = uncapturedCb;

        return deviceDescriptor;
    }
} // namespace Pretop::RHI
