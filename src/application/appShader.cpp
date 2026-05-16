#include "appShader.hpp"
#include "../asset/readFileBytes.hpp"
#include <iostream>

AppShader AppShader::pipeline(AppDevice &device, AppInstance &instance, std::string src)
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
    std::vector<std::byte> bytes = readFileBytes("assets/" + src);
    shaderWGSL.code = {(char *)bytes.data(), bytes.size()};

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