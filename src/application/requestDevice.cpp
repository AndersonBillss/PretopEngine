#include <iostream>
#include "requestDevice.hpp"
#include "printStringView.hpp"

WGPUDevice requestDeviceSync(
    WGPUInstance instance,
    WGPUAdapter adapter,
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
        adapter,
        descriptor,
        info);
    WGPUFutureWaitInfo waitInfo = {f, 0};

    WGPUWaitStatus status = wgpuInstanceWaitAny(instance, 1, &waitInfo, UINT64_MAX);
    return device;
}
