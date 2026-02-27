#include <iostream>
#include <unordered_map>
#include "../printStringView.hpp"
#include "application.hpp"
#include "createInstance.hpp"
#include "createDevice.hpp"
#include "requestAdapter.hpp"

Application::Application()
{
    this->_logQueueCommands = false;
    this->_instance = createInstance();
    WGPURequestAdapterOptions adapterOpts = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
    this->_adapter = requestAdapterSync(this->_instance, &adapterOpts);
    this->_device = createDevice(_instance, _adapter);
    createQueue();
}

void Application::setWindow(Window *win)
{
    this->_window = win;
    WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
    this->_windowSurface = win->getSurface(this->_instance);

    std::cout << "Looking for available formats" << std::endl;
    WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
    WGPUSurfaceCapabilities surfaceCapabilities;
    wgpuSurfaceGetCapabilities(this->_windowSurface, this->_adapter, &surfaceCapabilities);

    // The first format in the list is the preffered format.
    // see https://webgpu-native.github.io/webgpu-headers/Surfaces.html#Surface-Creation
    surfaceConfig.format = surfaceCapabilities.formats[0];
    wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
    surfaceConfig.viewFormatCount = 0;
    surfaceConfig.viewFormats = nullptr;
    surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
    surfaceConfig.device = this->_device;
    surfaceConfig.presentMode = WGPUPresentMode_Fifo;
    surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;
    surfaceConfig.width = win->width;
    surfaceConfig.height = win->height;
    wgpuSurfaceConfigure(this->_windowSurface, &surfaceConfig);

    win->setOnExit([this]()
                   {
    wgpuSurfaceUnconfigure(this->_windowSurface);
    wgpuQueueRelease(this->_queue);
    wgpuSurfaceRelease(this->_windowSurface);
    wgpuDeviceRelease(this->_device);
    wgpuAdapterRelease(this->_adapter);
    wgpuInstanceRelease(this->_instance); });

    win->setOnTick([this](double dt)
                   {
                       std::cout << "DeltaTime: " << dt << std::endl;
                       WGPUTextureView targetView = getNextSurfaceTextureView();
                       if (!targetView)
                           return;

                       // Create a command encoder for the draw call
                       WGPUCommandEncoderDescriptor encoderDesc = {};
                       encoderDesc.nextInChain = nullptr;
                       std::string encoderLabel = "My command encoder";
                       encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
                       WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(this->_device, &encoderDesc);

                       // Create the render pass that clears the screen with our color
                       WGPURenderPassDescriptor renderPassDesc = {};
                       renderPassDesc.nextInChain = nullptr;

                       // The attachment part of the render pass descriptor describes the target texture of the pass
                       WGPURenderPassColorAttachment renderPassColorAttachment = {};
                       renderPassColorAttachment.view = targetView;
                       renderPassColorAttachment.resolveTarget = nullptr;
                       renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
                       renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
                       renderPassColorAttachment.clearValue = WGPUColor{0.9, 0.1, 0.2, 1.0};
                       renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

                       renderPassDesc.colorAttachmentCount = 1;
                       renderPassDesc.colorAttachments = &renderPassColorAttachment;
                       renderPassDesc.depthStencilAttachment = nullptr;
                       renderPassDesc.timestampWrites = nullptr;

                       // Create the render pass and end it immediately (we only clear the screen but do not draw anything)
                       WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);
                       wgpuRenderPassEncoderEnd(renderPass);
                       wgpuRenderPassEncoderRelease(renderPass);

                       // Finally encode and submit the render pass
                       WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
                       cmdBufferDescriptor.nextInChain = nullptr;
                       std::string cmdBufferLabel = "command buffer";
                       cmdBufferDescriptor.label = {cmdBufferLabel.c_str(), cmdBufferLabel.size()};
                       WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
                       wgpuCommandEncoderRelease(encoder);

                       std::cout << "Submitting command..." << std::endl;
                       wgpuQueueSubmit(this->_queue, 1, &command);
                       wgpuCommandBufferRelease(command);
                       std::cout << "Command submitted." << std::endl;

                       // At the end of the frame
                       wgpuTextureViewRelease(targetView);
#ifndef __EMSCRIPTEN__
                       wgpuSurfacePresent(this->_windowSurface);
#endif
                   });
}

void Application::createQueue()
{
    this->_queue = wgpuDeviceGetQueue(this->_device);

    WGPUQueueWorkDoneCallback onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, WGPUStringView message, void *data, void *)
    {
        auto self = static_cast<Application *>(data);
        if (self->_logQueueCommands)
        {
            std::unordered_map<WGPUQueueWorkDoneStatus, std::string> statusToString = {
                {WGPUQueueWorkDoneStatus_Success, "WGPUQueueWorkDoneStatus_Success"},
                {WGPUQueueWorkDoneStatus_CallbackCancelled, "WGPUQueueWorkDoneStatus_CallbackCancelled"},
                {WGPUQueueWorkDoneStatus_Error, "WGPUQueueWorkDoneStatus_Error"},
            };
            std::string stringifiedStatus = "Unknown";
            auto it = statusToString.find(status);
            if (it != statusToString.end())
            {
                stringifiedStatus = it->second;
            }
            std::cout << "Queued work finished with status: " << stringifiedStatus << std::endl;
            std::cout << message << std::endl;
        }
    };

    WGPUQueueWorkDoneCallbackInfo workQueueWorkDoneCb = {
        /* nextInChain */ nullptr,
        /* mode */ WGPUCallbackMode_AllowSpontaneous,
        /* callback */ onQueueWorkDone,
        /* userdata1 */ this,
        /* userdata2 */ nullptr};
    wgpuQueueOnSubmittedWorkDone(this->_queue, workQueueWorkDoneCb);
}

void Application::logQueueCommands()
{
    this->_logQueueCommands = true;
}

Application *Application::inspectInstance()
{
    return this;
}

Application *Application::inspectDevice()
{
    WGPULimits limits = WGPU_LIMITS_INIT;
    bool success = wgpuDeviceGetLimits(this->_device, &limits) == WGPUStatus_Success;
    if (success)
    {
        std::cout << "\nDevice limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
    }
    return this;
}

Application *Application::inspectAdapter()
{
    WGPULimits supportedLimits = {};
    supportedLimits.nextInChain = nullptr;

    bool success = wgpuAdapterGetLimits(this->_adapter, &supportedLimits) == WGPUStatus_Success;

    if (success)
    {
        std::cout << "\nAdapter limits:" << std::endl;
        std::cout << " - maxTextureDimension1D: " << supportedLimits.maxTextureDimension1D << std::endl;
        std::cout << " - maxTextureDimension2D: " << supportedLimits.maxTextureDimension2D << std::endl;
        std::cout << " - maxTextureDimension3D: " << supportedLimits.maxTextureDimension3D << std::endl;
        std::cout << " - maxTextureArrayLayers: " << supportedLimits.maxTextureArrayLayers << std::endl;
    }

    std::unordered_map<WGPUFeatureName, std::string> featureToString = {
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

    return this;
}

Application *Application::inspectQueue()
{
    return this;
}

WGPUTextureView Application::getNextSurfaceTextureView()
{
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(this->_windowSurface, &surfaceTexture);
    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
    {
        return nullptr;
    }

    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    std::string label = "Surface texture view";
    viewDescriptor.label = WGPUStringView{label.c_str(), label.size()};
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.usage = WGPUTextureUsage_RenderAttachment;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

    wgpuTextureRelease(surfaceTexture.texture);
    return targetView;
}
