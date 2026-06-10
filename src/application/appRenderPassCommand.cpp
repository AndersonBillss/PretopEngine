#include "appRenderPassCommand.hpp"

AppRenderPassCommand::AppRenderPassCommand(
    AppDevice &device,
    WGPUTextureView targetView,
    WGPURenderPassDepthStencilAttachment &depthStencilAttachment)
{
    this->wgpuRenderPassDescriptor = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
    this->wgpuRenderPassDescriptor.nextInChain = nullptr;

    this->wgpuRenderPassColorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;
    this->wgpuRenderPassColorAttachment.view = targetView;
    this->wgpuRenderPassColorAttachment.resolveTarget = nullptr;
    this->wgpuRenderPassColorAttachment.loadOp = WGPULoadOp_Clear;
    this->wgpuRenderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    this->wgpuRenderPassColorAttachment.clearValue = WGPUColor{0.2, 0.2, 0.2, 1.0};
    this->wgpuRenderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    this->wgpuRenderPassDescriptor.colorAttachmentCount = 1;
    this->wgpuRenderPassDescriptor.colorAttachments = &this->wgpuRenderPassColorAttachment;
    this->wgpuRenderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;
    this->wgpuRenderPassDescriptor.timestampWrites = nullptr;
}