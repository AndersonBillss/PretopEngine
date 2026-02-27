#include "requestAdapter.hpp"
#include <iostream>
#include "printStringView.hpp"

WGPUAdapter requestAdapterSync(
    WGPUInstance instance,
    WGPURequestAdapterOptions const *options)
{
    WGPUAdapter adapter = nullptr;

    auto onAdapterRequestEnded = [](
                                     WGPURequestAdapterStatus status,
                                     WGPUAdapter adapter,
                                     WGPUStringView message,
                                     void *pAdapter,
                                     void *_)
    {
        WGPUAdapter *userAdapter = reinterpret_cast<WGPUAdapter *>(pAdapter);
        if (status == WGPURequestAdapterStatus_Success)
        {
            *userAdapter = adapter;
        }
        else
        {
            std::cout << "Could not get WebGPU adapter: " << message << std::endl;
        }
    };
    WGPURequestAdapterOptions adapterOpts = {};
#ifndef WEBGPU_BACKEND_EMSCRIPTEN
    adapterOpts.backendType = WGPUBackendType_D3D12;
#endif // !WEBGPU_BACKEND_EMSCRIPTEN
    WGPURequestAdapterCallbackInfo info = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode::WGPUCallbackMode_WaitAnyOnly,
        /* callback */ onAdapterRequestEnded,
        /* userdata 1 */ &adapter,
        /* userdata 2 */ nullptr,
    };
    WGPUFuture f = wgpuInstanceRequestAdapter(
        instance,
        &adapterOpts,
        info);
    WGPUFutureWaitInfo waitInfo = {f, 0};

    wgpuInstanceWaitAny(instance, 1, &waitInfo, UINT64_MAX);
    return adapter;
}