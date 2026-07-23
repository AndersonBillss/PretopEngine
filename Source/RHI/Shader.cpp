#include "Shader.hpp"
#include "../Asset/ModelParseError.hpp"
#include <iostream>

namespace Pretop::RHI
{
    Shader Shader::Pipeline(Device *device, Instance *instance, Asset::AssetLoader *assetLoader, std::string_view src)
    {
        auto compilationCallbackInfo = [](
                                           WGPUCompilationInfoRequestStatus,
                                           struct WGPUCompilationInfo const *compilationInfo,
                                           void *,
                                           void *)
        {
            for (size_t i = 0; i < compilationInfo->messageCount; i++)
            {
                if (compilationInfo->messages[i].type == WGPUCompilationMessageType_Error)
                {
                    std::cerr << "Engine stopped due to Shader compilation error" << std::endl;
                    exit(1);
                }
            }
        };

        Shader result;
        WGPUShaderSourceWGSL shaderWGSL = WGPU_SHADER_SOURCE_WGSL_INIT;
        shaderWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;

        // auto handle = assetLoader->LoadTextAsync("assets/" + std::string(src));
        // handle.Wait();
        // auto handleResult = handle.Get();
        // if (!handleResult)
        // {
        //     throw Asset::ModelParseError("Asset could not be loaded: " + handleResult.Error);
        // }
        // std::string sourceCode = handleResult.Data.data();
        // shaderWGSL.code = WGPUStringView{sourceCode.c_str(), sourceCode.length()};

        // WGPUShaderModuleDescriptor shaderDesc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
        // shaderDesc.nextInChain = &shaderWGSL.chain;

        // WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device->WgpuDevice, &shaderDesc);

        // WGPUCompilationInfoCallbackInfo callbackInfo = WGPU_COMPILATION_INFO_CALLBACK_INFO_INIT;
        // callbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
        // callbackInfo.callback = compilationCallbackInfo;
        // WGPUFuture compilationFuture = wgpuShaderModuleGetCompilationInfo(
        //     shaderModule, callbackInfo);

        // WGPUFutureWaitInfo waitInfo = {compilationFuture, 0};
        // wgpuInstanceWaitAny(instance->WgpuInstance, 1, &waitInfo, UINT64_MAX);
        // result.WgpuShader = shaderModule;
        return result;
    }
} // namespace Pretop::RHI
