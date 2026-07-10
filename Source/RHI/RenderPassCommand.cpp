#include "RenderPassCommand.hpp"

namespace Pretop::RHI
{
    RenderPassCommand::RenderPassCommand(
        Device *device,
        WGPUTextureView targetView,
        WGPURenderPassDepthStencilAttachment &depthStencilAttachment)
    {
        this->WgpuRenderPassDescriptor = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
        this->WgpuRenderPassDescriptor.nextInChain = nullptr;

        this->WgpuRenderPassColorAttachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;
        this->WgpuRenderPassColorAttachment.view = targetView;
        this->WgpuRenderPassColorAttachment.resolveTarget = nullptr;
        this->WgpuRenderPassColorAttachment.loadOp = WGPULoadOp_Clear;
        this->WgpuRenderPassColorAttachment.storeOp = WGPUStoreOp_Store;
        this->WgpuRenderPassColorAttachment.clearValue = WGPUColor{0.2, 0.2, 0.2, 1.0};
        this->WgpuRenderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        this->WgpuRenderPassDescriptor.colorAttachmentCount = 1;
        this->WgpuRenderPassDescriptor.colorAttachments = &this->WgpuRenderPassColorAttachment;
        this->WgpuRenderPassDescriptor.depthStencilAttachment = &depthStencilAttachment;
        this->WgpuRenderPassDescriptor.timestampWrites = nullptr;
    }
} // namespace Pretop::RHI
