#include "appCommand.hpp"

AppCommand::AppCommand(AppDevice &device, AppPipeline &pipeline, WGPUTextureView targetView)
{
    WGPUCommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    std::string encoderLabel = "My command encoder";
    encoderDesc.label = {encoderLabel.c_str(), encoderLabel.size()};
    this->wgpuEncoder = wgpuDeviceCreateCommandEncoder(device.wgpuDevice, &encoderDesc);

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
    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(this->wgpuEncoder, &renderPassDesc);

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
    this->wgpuCommand = wgpuCommandEncoderFinish(this->wgpuEncoder, &cmdBufferDescriptor);
}

AppCommand::~AppCommand()
{
    wgpuCommandEncoderRelease(this->wgpuEncoder);
}
