#include <iostream>
#include <unordered_map>
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"
#include "application/appShader.hpp"
#include "application/appPipeline.hpp"

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;

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
    AppShader shader = AppShader::pipeline(application, shaderSource);

    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppPipeline pipeline(application, shader);

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
                    wgpuRenderPassEncoderSetPipeline(renderPass, pipeline.wgpuPipeline);

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
    return 0;
}