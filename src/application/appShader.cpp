#include "appShader.hpp"
#include "../asset/modelParseError.hpp"
#include <iostream>

AppShader AppShader::pipeline(AppDevice &device, AppInstance &instance, AssetLoader *assetLoader, std::string_view src)
{
    auto compilationCallbackInfo = [](
                                       WGPUCompilationInfoRequestStatus status,
                                       struct WGPUCompilationInfo const *compilationInfo,
                                       void *_,
                                       void *__)
    {
        bool hasError = false;
        for (size_t i = 0; i < compilationInfo->messageCount; i++)
        {
            if (compilationInfo->messages[i].type == WGPUCompilationMessageType_Error)
            {
                std::cerr << "Engine stopped due to Shader compilation error" << std::endl;
                exit(1);
            }
        }
    };

    AppShader result;
    WGPUShaderSourceWGSL shaderWGSL = WGPU_SHADER_SOURCE_WGSL_INIT;
    shaderWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;

    auto handle = assetLoader->loadTextAsync("assets/" + std::string(src));
    handle.wait();
    auto handleResult = handle.get();
    if (!handleResult)
    {
        throw ModelParseError("Asset could not be loaded: " + handleResult.error);
    }
    std::string sourceCode = handleResult.data.data();
    shaderWGSL.code = WGPUStringView{sourceCode.c_str(), sourceCode.length()};

    WGPUShaderModuleDescriptor shaderDesc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
    shaderDesc.nextInChain = &shaderWGSL.chain;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device.wgpuDevice, &shaderDesc);

    WGPUCompilationInfoCallbackInfo callbackInfo = WGPU_COMPILATION_INFO_CALLBACK_INFO_INIT;
    callbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.callback = compilationCallbackInfo;
    WGPUFuture compilationFuture = wgpuShaderModuleGetCompilationInfo(
        shaderModule, callbackInfo);

    WGPUFutureWaitInfo waitInfo = {compilationFuture, 0};
    wgpuInstanceWaitAny(instance.wgpuInstance, 1, &waitInfo, UINT64_MAX);
    result.wgpuShader = shaderModule;
    return result;
}