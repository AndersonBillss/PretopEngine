#include "Adapter.hpp"
#include <iostream>
#include <unordered_map>

#include "../PrintStringView.hpp"
#include "../Core/Platform.hpp"

namespace Pretop::RHI
{
    Adapter::Adapter(WGPUAdapter adapter)
    {
        WGPURequestAdapterOptions adapterOpts = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
        this->WgpuAdapter = adapter;
    }

    struct AdapterRequestUserData
    {
        Adapter::RequestAdapterCallback Callback;
    };
    void Adapter::Request(Instance *instance, RequestAdapterCallback callback)
    {
        auto onAdapterRequestEnded = [](
                                         WGPURequestAdapterStatus status,
                                         WGPUAdapter adapter,
                                         WGPUStringView message,
                                         void *userDataPointer,
                                         void *)
        {
            AdapterRequestUserData *userData = reinterpret_cast<AdapterRequestUserData *>(userDataPointer);
            if (status == WGPURequestAdapterStatus_Success)
            {
                std::unique_ptr<Adapter> result = std::make_unique<Adapter>(adapter);
                userData->Callback(std::move(result));
                delete userData;
            }
            else
            {
                std::cout << "Could not get WebGPU adapter: " << message << std::endl;
            }
        };
        WGPURequestAdapterOptions adapterOpts = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
#ifdef PRETOP_PLATFORM_WINDOWS
        adapterOpts.backendType = WGPUBackendType_D3D12;
#endif // PRETOP_PLATFORM_WINDOWS
        auto *userData = new AdapterRequestUserData{callback};
        WGPURequestAdapterCallbackInfo info = {
            /* nextInChain */ nullptr,
            /* mode */ WGPUCallbackMode::WGPUCallbackMode_AllowSpontaneous,
            /* callback */ onAdapterRequestEnded,
            /* userdata 1 */ userData,
            /* userdata 2 */ nullptr,
        };
        wgpuInstanceRequestAdapter(instance->WgpuInstance, &adapterOpts, info);
    }

    void Adapter::Inspect()
    {
        WGPULimits supportedLimits = {};
        supportedLimits.nextInChain = nullptr;

        bool success = wgpuAdapterGetLimits(this->WgpuAdapter, &supportedLimits) == WGPUStatus_Success;

        if (success)
        {
            std::cout << "\nAdapter limits:" << std::endl;
            std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
            std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
            std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
            std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
        }

        WGPUSupportedFeatures supportedFeatures;
        wgpuAdapterGetFeatures(WgpuAdapter, &supportedFeatures);
        delete[] supportedFeatures.features;
    }
} // namespace Pretop::RHI
