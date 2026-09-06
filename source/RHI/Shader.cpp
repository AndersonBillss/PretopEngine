#include "Shader.hpp"
#include "../Asset/ModelParseError.hpp"
#include <iostream>

namespace Pretop::RHI
{
    Shader Shader::Pipeline(Core::GraphicsContext context, const void *src, size_t srcSize)
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

        WGPUShaderSourceWGSL shaderWGSL = WGPU_SHADER_SOURCE_WGSL_INIT;
        shaderWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;

        shaderWGSL.code = WGPUStringView{static_cast<const char *>(src), srcSize};

        WGPUShaderModuleDescriptor shaderDesc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
        shaderDesc.nextInChain = &shaderWGSL.chain;

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(context.Device, &shaderDesc);

        WGPUCompilationInfoCallbackInfo callbackInfo = WGPU_COMPILATION_INFO_CALLBACK_INFO_INIT;
        callbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
        callbackInfo.callback = compilationCallbackInfo;
        wgpuShaderModuleGetCompilationInfo(
            shaderModule, callbackInfo);

        Shader result;
        result.WgpuShader = shaderModule;
        return result;
    }
} // namespace Pretop::RHI
