#include <iostream>
#include <unordered_map>
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"

int main(int, char **)
{
  std::cout << "Hello, WebGPU!!" << std::endl;

  Application application;
  application.logQueueCommands();
  application.setWindow(WindowFactory::createWindow("My Window"));
  application.run([](
                      double dt,
                      WGPUTextureView targetView,
                      WGPUCommandEncoder encoder)
                  {
                    std::cout << "DeltaTime: " << dt << std::endl;

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
                    return command; });

  return 0;
}