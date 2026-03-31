#include <iostream>
#include <unordered_map>
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"

void compilationCallbackInfo(
    WGPUCompilationInfoRequestStatus status,
    struct WGPUCompilationInfo const *compilationInfo,
    void *_,
    void *__)
{
    const std::unordered_map<WGPUCompilationMessageType, std::string> msgTypesToString = {
        {WGPUCompilationMessageType_Error, "Error"},
        {WGPUCompilationMessageType_Warning, "Warning"},
        {WGPUCompilationMessageType_Info, "Info"},
    };
    for (size_t i = 0; i < compilationInfo->messageCount; i++)
    {
        auto msg = compilationInfo->messages[i];
        auto it = msgTypesToString.find(msg.type);
        if (it == msgTypesToString.end())
        {
            std::cout << "Unknown message:" << std::endl;
        }
        else
        {
            std::cout << it->second << ":" << std::endl;
        }
        std::cout << msg.message << std::endl;
    }
}

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;
    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    std::string shaderSource = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
            var p = vec2f(0.0, 0.0);
            if (in_vertex_index == 0u) {
                p = vec2f(-0.5, -0.5);
            } else if (in_vertex_index == 1u) {
                p = vec2f(0.5, -0.5);
            } else {
                p = vec2f(0.0, 0.5);
            }
            return vec4f(p, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.0, 0.4, 1.0, 1.0);
        }
    )";
    WGPUShaderSourceWGSL shaderWGSL = WGPU_SHADER_SOURCE_WGSL_INIT;
    shaderWGSL.chain.sType = WGPUSType_ShaderSourceWGSL;
    shaderWGSL.code = {shaderSource.c_str(), shaderSource.size()};

    WGPUShaderModuleDescriptor shaderDesc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
    shaderDesc.nextInChain = &shaderWGSL.chain;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(application.device.wgpuDevice, &shaderDesc);

    WGPUCompilationInfoCallbackInfo callbackInfo = WGPU_COMPILATION_INFO_CALLBACK_INFO_INIT;
    callbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
    callbackInfo.callback = compilationCallbackInfo;
    WGPUFuture compilationFuture = wgpuShaderModuleGetCompilationInfo(
        shaderModule, callbackInfo);

    WGPURenderPipelineDescriptor pipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;

    WGPUFragmentState fragmentState = WGPU_FRAGMENT_STATE_INIT;
    fragmentState.module = shaderModule;
    std::string fragmentEntryPoint = "fs_main";
    fragmentState.entryPoint = WGPUStringView{fragmentEntryPoint.c_str(), fragmentEntryPoint.size()};
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;

    pipelineDesc.vertex.module = shaderModule;
    std::string vertexEntryPoint = "vs_main";
    pipelineDesc.vertex.entryPoint = WGPUStringView{vertexEntryPoint.c_str(), vertexEntryPoint.size()};

    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget = WGPU_COLOR_TARGET_STATE_INIT;
    colorTarget.format = application.windowFormat;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(application.device.wgpuDevice, &pipelineDesc);

    application.run([&application, &pipeline](
                        double dt,
                        WGPUTextureView targetView,
                        AppDevice device)
                    {
                    std::cout << "DELTATIME: " << dt << std::endl;
                    WGPUCommandEncoderDescriptor encoderDesc = {};
                    encoderDesc.nextInChain = nullptr;
                    std::string encoderLabel = "My command encoder";
                    encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
                    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device.wgpuDevice, &encoderDesc);

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
                    
                    // Select which render pipeline to use
                    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);

                    // Draw 1 instance of a 3-vertices shape
                    wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

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
                    return command; });

    wgpuRenderPipelineRelease(pipeline);
    return 0;
}