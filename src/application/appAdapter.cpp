#include "appAdapter.hpp"
#include <iostream>
#include <unordered_map>
#include "../printStringView.hpp"

static const std::unordered_map<WGPUFeatureName, std::string> featureToString = {
    {WGPUFeatureName_CoreFeaturesAndLimits, "WGPUFeatureName_CoreFeaturesAndLimits"},
    {WGPUFeatureName_DepthClipControl, "WGPUFeatureName_DepthClipControl"},
    {WGPUFeatureName_Depth32FloatStencil8, "WGPUFeatureName_Depth32FloatStencil8"},
    {WGPUFeatureName_TextureCompressionBC, "WGPUFeatureName_TextureCompressionBC"},
    {WGPUFeatureName_TextureCompressionBCSliced3D, "WGPUFeatureName_TextureCompressionBCSliced3D"},
    {WGPUFeatureName_TextureCompressionETC2, "WGPUFeatureName_TextureCompressionETC2"},
    {WGPUFeatureName_TextureCompressionASTC, "WGPUFeatureName_TextureCompressionASTC"},
    {WGPUFeatureName_TextureCompressionASTCSliced3D, "WGPUFeatureName_TextureCompressionASTCSliced3D"},
    {WGPUFeatureName_TimestampQuery, "WGPUFeatureName_TimestampQuery"},
    {WGPUFeatureName_IndirectFirstInstance, "WGPUFeatureName_IndirectFirstInstance"},
    {WGPUFeatureName_ShaderF16, "WGPUFeatureName_ShaderF16"},
    {WGPUFeatureName_RG11B10UfloatRenderable, "WGPUFeatureName_RG11B10UfloatRenderable"},
    {WGPUFeatureName_BGRA8UnormStorage, "WGPUFeatureName_BGRA8UnormStorage"},
    {WGPUFeatureName_Float32Filterable, "WGPUFeatureName_Float32Filterable"},
    {WGPUFeatureName_Float32Blendable, "WGPUFeatureName_Float32Blendable"},
    {WGPUFeatureName_ClipDistances, "WGPUFeatureName_ClipDistances"},
    {WGPUFeatureName_DualSourceBlending, "WGPUFeatureName_DualSourceBlending"},
    {WGPUFeatureName_TextureFormatsTier1, "WGPUFeatureName_TextureFormatsTier1"},
    {WGPUFeatureName_TextureFormatsTier2, "WGPUFeatureName_TextureFormatsTier2"},
    {WGPUFeatureName_PrimitiveIndex, "WGPUFeatureName_PrimitiveIndex"},
    {WGPUFeatureName_Subgroups, "WGPUFeatureName_Subgroups"},

#ifndef __EMSCRIPTEN__
    // Native backend-specific features
    {WGPUFeatureName_TextureComponentSwizzle, "WGPUFeatureName_TextureComponentSwizzle"},
    {WGPUFeatureName_DawnInternalUsages, "WGPUFeatureName_DawnInternalUsages"},
    {WGPUFeatureName_DawnMultiPlanarFormats, "WGPUFeatureName_DawnMultiPlanarFormats"},
    {WGPUFeatureName_DawnNative, "WGPUFeatureName_DawnNative"},
    {WGPUFeatureName_ChromiumExperimentalTimestampQueryInsidePasses, "WGPUFeatureName_ChromiumExperimentalTimestampQueryInsidePasses"},
    {WGPUFeatureName_ImplicitDeviceSynchronization, "WGPUFeatureName_ImplicitDeviceSynchronization"},
    {WGPUFeatureName_TransientAttachments, "WGPUFeatureName_TransientAttachments"},
    {WGPUFeatureName_MSAARenderToSingleSampled, "WGPUFeatureName_MSAARenderToSingleSampled"},
    {WGPUFeatureName_D3D11MultithreadProtected, "WGPUFeatureName_D3D11MultithreadProtected"},
    {WGPUFeatureName_ANGLETextureSharing, "WGPUFeatureName_ANGLETextureSharing"},
    {WGPUFeatureName_PixelLocalStorageCoherent, "WGPUFeatureName_PixelLocalStorageCoherent"},
    {WGPUFeatureName_PixelLocalStorageNonCoherent, "WGPUFeatureName_PixelLocalStorageNonCoherent"},
    {WGPUFeatureName_Unorm16TextureFormats, "WGPUFeatureName_Unorm16TextureFormats"},
    {WGPUFeatureName_MultiPlanarFormatExtendedUsages, "WGPUFeatureName_MultiPlanarFormatExtendedUsages"},
    {WGPUFeatureName_MultiPlanarFormatP010, "WGPUFeatureName_MultiPlanarFormatP010"},
    {WGPUFeatureName_HostMappedPointer, "WGPUFeatureName_HostMappedPointer"},
    {WGPUFeatureName_MultiPlanarRenderTargets, "WGPUFeatureName_MultiPlanarRenderTargets"},
    {WGPUFeatureName_MultiPlanarFormatNv12a, "WGPUFeatureName_MultiPlanarFormatNv12a"},
    {WGPUFeatureName_FramebufferFetch, "WGPUFeatureName_FramebufferFetch"},
    {WGPUFeatureName_BufferMapExtendedUsages, "WGPUFeatureName_BufferMapExtendedUsages"},
    {WGPUFeatureName_AdapterPropertiesMemoryHeaps, "WGPUFeatureName_AdapterPropertiesMemoryHeaps"},
    {WGPUFeatureName_AdapterPropertiesD3D, "WGPUFeatureName_AdapterPropertiesD3D"},
    {WGPUFeatureName_AdapterPropertiesVk, "WGPUFeatureName_AdapterPropertiesVk"},
    {WGPUFeatureName_DawnFormatCapabilities, "WGPUFeatureName_DawnFormatCapabilities"},
    {WGPUFeatureName_DawnDrmFormatCapabilities, "WGPUFeatureName_DawnDrmFormatCapabilities"},
    {WGPUFeatureName_MultiPlanarFormatNv16, "WGPUFeatureName_MultiPlanarFormatNv16"},
    {WGPUFeatureName_MultiPlanarFormatNv24, "WGPUFeatureName_MultiPlanarFormatNv24"},
    {WGPUFeatureName_MultiPlanarFormatP210, "WGPUFeatureName_MultiPlanarFormatP210"},
    {WGPUFeatureName_MultiPlanarFormatP410, "WGPUFeatureName_MultiPlanarFormatP410"},
    {WGPUFeatureName_SharedTextureMemoryVkDedicatedAllocation, "WGPUFeatureName_SharedTextureMemoryVkDedicatedAllocation"},
    {WGPUFeatureName_SharedTextureMemoryAHardwareBuffer, "WGPUFeatureName_SharedTextureMemoryAHardwareBuffer"},
    {WGPUFeatureName_SharedTextureMemoryDmaBuf, "WGPUFeatureName_SharedTextureMemoryDmaBuf"},
    {WGPUFeatureName_SharedTextureMemoryOpaqueFD, "WGPUFeatureName_SharedTextureMemoryOpaqueFD"},
    {WGPUFeatureName_SharedTextureMemoryZirconHandle, "WGPUFeatureName_SharedTextureMemoryZirconHandle"},
    {WGPUFeatureName_SharedTextureMemoryDXGISharedHandle, "WGPUFeatureName_SharedTextureMemoryDXGISharedHandle"},
    {WGPUFeatureName_SharedTextureMemoryD3D11Texture2D, "WGPUFeatureName_SharedTextureMemoryD3D11Texture2D"},
    {WGPUFeatureName_SharedTextureMemoryIOSurface, "WGPUFeatureName_SharedTextureMemoryIOSurface"},
    {WGPUFeatureName_SharedTextureMemoryEGLImage, "WGPUFeatureName_SharedTextureMemoryEGLImage"},
    {WGPUFeatureName_SharedFenceVkSemaphoreOpaqueFD, "WGPUFeatureName_SharedFenceVkSemaphoreOpaqueFD"},
    {WGPUFeatureName_SharedFenceSyncFD, "WGPUFeatureName_SharedFenceSyncFD"},
    {WGPUFeatureName_SharedFenceVkSemaphoreZirconHandle, "WGPUFeatureName_SharedFenceVkSemaphoreZirconHandle"},
    {WGPUFeatureName_SharedFenceDXGISharedHandle, "WGPUFeatureName_SharedFenceDXGISharedHandle"},
    {WGPUFeatureName_SharedFenceMTLSharedEvent, "WGPUFeatureName_SharedFenceMTLSharedEvent"},
    {WGPUFeatureName_SharedBufferMemoryD3D12Resource, "WGPUFeatureName_SharedBufferMemoryD3D12Resource"},
    {WGPUFeatureName_StaticSamplers, "WGPUFeatureName_StaticSamplers"},
    {WGPUFeatureName_YCbCrVulkanSamplers, "WGPUFeatureName_YCbCrVulkanSamplers"},
    {WGPUFeatureName_ShaderModuleCompilationOptions, "WGPUFeatureName_ShaderModuleCompilationOptions"},
    {WGPUFeatureName_DawnLoadResolveTexture, "WGPUFeatureName_DawnLoadResolveTexture"},
    {WGPUFeatureName_DawnPartialLoadResolveTexture, "WGPUFeatureName_DawnPartialLoadResolveTexture"},
    {WGPUFeatureName_MultiDrawIndirect, "WGPUFeatureName_MultiDrawIndirect"},
    {WGPUFeatureName_DawnTexelCopyBufferRowAlignment, "WGPUFeatureName_DawnTexelCopyBufferRowAlignment"},
    {WGPUFeatureName_FlexibleTextureViews, "WGPUFeatureName_FlexibleTextureViews"},
    {WGPUFeatureName_ChromiumExperimentalSubgroupMatrix, "WGPUFeatureName_ChromiumExperimentalSubgroupMatrix"},
    {WGPUFeatureName_SharedFenceEGLSync, "WGPUFeatureName_SharedFenceEGLSync"},
    {WGPUFeatureName_DawnDeviceAllocatorControl, "WGPUFeatureName_DawnDeviceAllocatorControl"},
    {WGPUFeatureName_AdapterPropertiesWGPU, "WGPUFeatureName_AdapterPropertiesWGPU"},
    {WGPUFeatureName_SharedBufferMemoryD3D12SharedMemoryFileMappingHandle, "WGPUFeatureName_SharedBufferMemoryD3D12SharedMemoryFileMappingHandle"},
    {WGPUFeatureName_SharedTextureMemoryD3D12Resource, "WGPUFeatureName_SharedTextureMemoryD3D12Resource"},
    {WGPUFeatureName_ChromiumExperimentalSamplingResourceTable, "WGPUFeatureName_ChromiumExperimentalSamplingResourceTable"},
    {WGPUFeatureName_Force32, "WGPUFeatureName_Force32"},
#endif // not __EMSCRIPTEN__
};

AppAdapter::AppAdapter(AppInstance instance)
{
    WGPURequestAdapterOptions adapterOpts = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
    this->wgpuAdapter = _requestAdapterSync(instance.wgpuInstance, &adapterOpts);
}

void AppAdapter::inspect()
{
    WGPULimits supportedLimits = {};
    supportedLimits.nextInChain = nullptr;

    bool success = wgpuAdapterGetLimits(this->wgpuAdapter, &supportedLimits) == WGPUStatus_Success;

    if (success)
    {
        std::cout << "\nAdapter limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
    }

    WGPUSupportedFeatures supportedFeatures;
    wgpuAdapterGetFeatures(wgpuAdapter, &supportedFeatures);

    std::cout << "\nSupported features:" << std::endl;
    for (size_t i = 0; i < supportedFeatures.featureCount; i++)
    {
        std::cout << " - ";
        auto it = featureToString.find(supportedFeatures.features[i]);
        if (it == featureToString.end())
        {
            std::cout << "Unknown feature: " << supportedFeatures.features[i] << std::endl;
            ;
        }
        else
        {
            std::cout << it->second << std::endl;
        }
    }

    delete[] supportedFeatures.features;
}

WGPUAdapter AppAdapter::_requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const *options)
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