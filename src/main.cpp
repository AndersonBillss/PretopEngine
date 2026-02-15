#include <iostream>
#include <unordered_map>
#include "requestAdapter.hpp"
#include "requestDevice.hpp"
#include "printStringView.hpp"
#include "window/windowFactory.hpp"

void onDeviceLost(WGPUDevice const *device, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
{
  if (reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
    std::cout << "WGPU device lost: " << message << std::endl;
}

void onDeviceUncapturedError(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, void *, void *)
{
  std::cout << "WGPU device error: " << message << std::endl;
}

void inspectDevice(WGPUDevice &device)
{
  WGPULimits limits = WGPU_LIMITS_INIT;
  bool success = wgpuDeviceGetLimits(device, &limits) == WGPUStatus_Success;
  if (success)
  {
    std::cout << "\nDevice limits:" << std::endl;
    std::cout << " - maxTextureDimension1D: " << limits.maxTextureDimension1D << std::endl;
    std::cout << " - maxTextureDimension2D: " << limits.maxTextureDimension2D << std::endl;
    std::cout << " - maxTextureDimension3D: " << limits.maxTextureDimension3D << std::endl;
    std::cout << " - maxTextureArrayLayers: " << limits.maxTextureArrayLayers << std::endl;
  }
}

void inspectAdapter(WGPUAdapter &adapter)
{
  WGPULimits supportedLimits = {};
  supportedLimits.nextInChain = nullptr;

  bool success = wgpuAdapterGetLimits(adapter, &supportedLimits) == WGPUStatus_Success;

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

  WGPUSupportedFeatures supportedFeatures;
  wgpuAdapterGetFeatures(adapter, &supportedFeatures);

  std::cout << "\nSupported features:" << std::endl;
  for (size_t i = 0; i < supportedFeatures.featureCount; i++)
  {
    std::cout << " - ";
    auto it = featureToString.find(supportedFeatures.features[i]);
    if (it == featureToString.end())
    {
      std::cout << "Unknown feature: " << supportedFeatures.features[i] << std::endl;
    }
    else
    {
      std::cout << it->second << std::endl;
    }
  }

  delete[] supportedFeatures.features;

  WGPUAdapterInfo adapterInfo = WGPU_ADAPTER_INFO_INIT;
  WGPUStatus adapterInfoResult = wgpuAdapterGetInfo(adapter, &adapterInfo);

  std::cout << "\nAdapter properties:" << std::endl;
  std::cout << " - vendorID: " << adapterInfo.vendorID << std::endl;
  if (adapterInfo.vendor.length)
  {
    std::cout << " - vendor: " << adapterInfo.vendor << std::endl;
  }
  if (adapterInfo.architecture.length)
  {
    std::cout << " - architecture: " << adapterInfo.architecture << std::endl;
  }
  std::cout << " - deviceID: " << adapterInfo.deviceID << std::endl;
  if (adapterInfo.device.length)
  {
    std::cout << " - device: " << adapterInfo.device << std::endl;
  }
  if (adapterInfo.description.length)
  {
    std::cout << " - description: " << adapterInfo.description << std::endl;
  }
  std::cout << std::hex;
  std::cout << " - adapterType: 0x" << adapterInfo.adapterType << std::endl;
  std::cout << " - backendType: 0x" << adapterInfo.backendType << std::endl;
  std::cout << std::dec; // Restore decimal numbers
}

int main(int, char **)
{
  std::cout << "Hello, WebGPU!!" << std::endl;

  WGPUInstanceDescriptor desc = {};
  desc.nextInChain = nullptr;
  WGPUInstanceFeatureName features[] = {WGPUInstanceFeatureName_TimedWaitAny};
  desc.requiredFeatureCount = 1;
  desc.requiredFeatures = features;

  // We create the instance using this descriptor
  WGPUInstance instance = wgpuCreateInstance(&desc);

  if (!instance)
  {
    std::cerr << "Could not initialize WebGPU!" << std::endl;
    return 1;
  }

  std::cout << "Requesting adapter..." << std::endl;

  WGPURequestAdapterOptions adapterOpts = {};
  adapterOpts.nextInChain = nullptr;
  WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);
  std::cout << "Got adapter: " << adapter << std::endl;
  inspectAdapter(adapter);

  std::cout << "\nRequesting device..." << std::endl;
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

  WGPUDevice device = requestDeviceSync(instance, adapter, &deviceDescriptor);
  std::cout << "Got device: " << device << std::endl;
  inspectDevice(device);

  WGPUQueue queue = wgpuDeviceGetQueue(device);

  WGPUQueueWorkDoneCallback onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, WGPUStringView message, void *, void *)
  {
    std::unordered_map<WGPUQueueWorkDoneStatus, std::string> statusToString = {
        {WGPUQueueWorkDoneStatus_Success, "WGPUQueueWorkDoneStatus_Success"},
        {WGPUQueueWorkDoneStatus_CallbackCancelled, "WGPUQueueWorkDoneStatus_CallbackCancelled"},
        {WGPUQueueWorkDoneStatus_Error, "WGPUQueueWorkDoneStatus_Error"},
        {WGPUQueueWorkDoneStatus_Force32, "WGPUQueueWorkDoneStatus_Force32"},
    };
    std::string stringifiedStatus = "Unknown";
    auto it = statusToString.find(status);
    if (it != statusToString.end())
    {
      stringifiedStatus = it->second;
    }
    std::cout << "Queued work finished with status: " << stringifiedStatus << std::endl;
    std::cout << message << std::endl;
  };

  WGPUQueueWorkDoneCallbackInfo workQueueWorkDoneCb = {
      /* nextInChain */ nullptr,
      /* mode */ WGPUCallbackMode_AllowSpontaneous,
      /* callback */ onQueueWorkDone,
      /* userdata1 */ nullptr,
      /* userdata2 */ nullptr};
  wgpuQueueOnSubmittedWorkDone(queue, workQueueWorkDoneCb);

  WGPUCommandEncoderDescriptor encoderDesc = WGPU_COMMAND_ENCODER_DESCRIPTOR_INIT;
  WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

  std::string debugMarkerData = "Do something";
  WGPUStringView wgpuMarkerData = {
      /* data */ debugMarkerData.c_str(),
      /* length */ debugMarkerData.size()};
  wgpuCommandEncoderInsertDebugMarker(encoder, wgpuMarkerData);

  WGPUCommandBufferDescriptor cmdBufferDescriptor = WGPU_COMMAND_BUFFER_DESCRIPTOR_INIT;
  WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
  wgpuCommandEncoderRelease(encoder);
  std::cout << "Submitting command..." << std::endl;
  wgpuQueueSubmit(queue, 1, &command);
  wgpuCommandBufferRelease(command);
  std::cout << "Command submitted." << std::endl;

  auto window = WindowFactory::createWindow("My Window");
  std::cout << "Getting surface...\n";
  auto surface = window->getSurface(instance);
  std::cout << "Got surface!\n";
  WGPUSurfaceConfiguration surfaceConfig = WGPU_SURFACE_CONFIGURATION_INIT;
  surfaceConfig.width = 640;
  surfaceConfig.height = 480;

  // Get the preferred texture format for the GPU
  std::cout << "Looking for available formats" << std::endl;
  WGPUSurfaceTexture surfaceTexture = WGPU_SURFACE_TEXTURE_INIT;
  WGPUSurfaceCapabilities surfaceCapabilities;
  wgpuSurfaceGetCapabilities(surface, adapter, &surfaceCapabilities);
  std::cout << "Found " << surfaceCapabilities.formatCount << " formats" << std::endl;
  for (int i = 0; i < surfaceCapabilities.formatCount; i++)
  {
    std::cout << "SURFACE CAPABILITY: " << std::hex << surfaceCapabilities.formats[i] << std::endl;
  }

  surfaceConfig.viewFormatCount = 0;
  surfaceConfig.viewFormats = nullptr;
  surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
  surfaceConfig.device = device;
  surfaceConfig.presentMode = WGPUPresentMode_Fifo;
  surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;
  wgpuSurfaceConfigure(surface, &surfaceConfig);

  window->setOnTick([](double dt)
                    { std::cout << "DeltaTime: " << dt << std::endl; });
  window->setOnExit([surface, queue, device, adapter, instance]()
                    {
    wgpuSurfaceUnconfigure(surface);
    wgpuSurfaceRelease(surface);
    wgpuQueueRelease(queue);
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuInstanceRelease(instance); });
  // window->run();

  return 0;
}