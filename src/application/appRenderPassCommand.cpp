#include "appRenderPassCommand.hpp"

AppRenderPassCommand::AppRenderPassCommand(AppDevice &device, WGPUTextureView targetView)
{
    this->wgpuRenderPassDescriptor = {};
    this->wgpuRenderPassDescriptor.nextInChain = nullptr;

    this->wgpuRenderPassColorAttachment = {};
    this->wgpuRenderPassColorAttachment.view = targetView;
    this->wgpuRenderPassColorAttachment.resolveTarget = nullptr;
    this->wgpuRenderPassColorAttachment.loadOp = WGPULoadOp_Clear;
    this->wgpuRenderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    this->wgpuRenderPassColorAttachment.clearValue = WGPUColor{0.0, 0.0, 0.0, 1.0};
    this->wgpuRenderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    this->wgpuRenderPassDescriptor.colorAttachmentCount = 1;
    this->wgpuRenderPassDescriptor.colorAttachments = &this->wgpuRenderPassColorAttachment;
    this->wgpuRenderPassDescriptor.depthStencilAttachment = nullptr;
    this->wgpuRenderPassDescriptor.timestampWrites = nullptr;
}