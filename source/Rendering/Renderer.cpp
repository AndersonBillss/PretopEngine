#include <array>
#include <cstring>
#include "Renderer.hpp"
#include "../SHARED_/GetAssetId.hpp"

namespace Pretop::Rendering
{
    WGPUStringView wgpuStr(const char *data)
    {
        size_t len = strlen(data);
        return {
            /*.data=*/data,
            /*.length=*/len,
        };
    }

    Renderer::Renderer(Pretop::Asset::AssetManager *assetManager, Core::GraphicsContext ctx, WGPUSurface windowSurface, WGPUTextureFormat windowFormat)
    {
        this->_ctx = ctx;
        this->_windowSurface = windowSurface;
        this->_windowFormat = windowFormat;
        this->_assetManager = assetManager;
        this->_shaderRef = assetManager->LoadShaderModule(Pretop::Utils::GetAssetId("shaders/shader.wgsl"));
    }

    void Renderer::AddObject(GpuMesh object)
    {
        this->_objects.push_back(object);
    }

    void Renderer::Render()
    {
        WGPUCommandEncoderDescriptor encoderDescriptor = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Command encoder"),
        };
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(this->_ctx.Device, &encoderDescriptor);

        WGPURenderPassColorAttachment renderPassColorAttachment = {
            /*.nextInChain=*/nullptr,
            /*.view=*/this->_windowView,
            /*.depthSlice=*/WGPU_DEPTH_SLICE_UNDEFINED,
            /*.resolveTarget=*/nullptr,
            /*.loadOp=*/WGPULoadOp_Clear,
            /*.storeOp=*/WGPUStoreOp_Store,
            /*.clearValue=*/WGPUColor{0.2, 0.2, 0.2, 1.0},
        };
        WGPURenderPassDescriptor renderPassDescriptor = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Texture render pass"),
            /*.colorAttachmentCount=*/1,
            /*.colorAttachments=*/&renderPassColorAttachment,
            /*.depthStencilAttachment=*/&this->_depthStencilAttachment,
            /*.occlusionQuerySet=*/nullptr,
            /*.timestampWrites=*/nullptr,
        };
        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);

        wgpuRenderPassEncoderSetPipeline(renderPass, this->_pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(
            renderPass, 0, this->_objects[0].vertexBuffer, 0, wgpuBufferGetSize(this->_objects[0].vertexBuffer));
        wgpuRenderPassEncoderSetIndexBuffer(
            renderPass, this->_objects[0].indexBuffer, this->_objects[0].indexFormat, 0, wgpuBufferGetSize(this->_objects[0].indexBuffer));
        wgpuRenderPassEncoderSetBindGroup(renderPass, 0, this->_bindGroup, 0, nullptr);

        int indexSize = this->_objects[0].indexFormat == WGPUIndexFormat_Uint16 ? sizeof(uint16_t) : sizeof(uint32_t);
        wgpuRenderPassEncoderDrawIndexed(
            renderPass, wgpuBufferGetSize(this->_objects[0].indexBuffer) / indexSize, 1, 0, 0, 0);
        wgpuRenderPassEncoderEnd(renderPass);
        wgpuRenderPassEncoderRelease(renderPass);

        WGPUCommandBufferDescriptor cmdBufferDescriptor = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Texture command buffer"),
        };
        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);

        wgpuQueueSubmit(this->_queue, 1, &commandBuffer);

        wgpuCommandEncoderRelease(encoder);
    }

    void Renderer::UpdateMVP(Math::Mat4x4 Model, Math::Mat4x4 View, Math::Mat4x4 Projection)
    {
        this->_uniforms.ModelMatrix = Model;
        this->_uniforms.ViewMatrix = View;
        this->_uniforms.ProjectionMatrix = Projection;
        wgpuQueueWriteBuffer(
            this->_queue,
            this->_uniformBuffer,
            0,
            &this->_uniforms,
            sizeof(MyUniforms));
    }

    bool Renderer::PollReady()
    {
        Pretop::RHI::Shader *shader;
        auto result = this->_assetManager->GetShaderModule(this->_shaderRef, &shader);
        if (result != Asset::AssetManager::AssetResult::Success)
        {
            if (result != Asset::AssetManager::AssetResult::NotReady)
            {
                std::cout << this->_assetManager->GetError(this->_shaderRef) << std::endl;
            }
            return false;
        }
        this->_afterShadersLoadedStep(shader->WgpuShader);
    }

    void Renderer::_afterShadersLoadedStep(WGPUShaderModule shaderModule)
    {
        WGPUVertexAttribute vertexAttributes[2];
        vertexAttributes[0] = {
            /*.nextInChain=*/nullptr,
            /*.format=*/WGPUVertexFormat_Float32x3,
            /*.offset=*/0,
            /*.shaderLocation=*/0,
        };
        vertexAttributes[1] = {
            /*.nextInChain=*/nullptr,
            /*.format=*/WGPUVertexFormat_Float32x3,
            /*.offset=*/sizeof(float) * 3,
            /*.shaderLocation=*/1,
        };
        WGPUVertexBufferLayout vertexBufferLayout = {
            /*.nextInChain=*/nullptr,
            /*.stepMode=*/WGPUVertexStepMode_Vertex,
            /*.arrayStride=*/sizeof(float) * 6,
            /*.attributeCount=*/2,
            /*.attributes=*/vertexAttributes,
        };

        WGPUBindGroupLayoutEntry bindingLayoutEntry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
        bindingLayoutEntry.binding = 0;
        bindingLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
        bindingLayoutEntry.buffer.minBindingSize = sizeof(MyUniforms);
        bindingLayoutEntry.buffer.hasDynamicOffset = true;
        bindingLayoutEntry.visibility = WGPUShaderStage_Vertex;

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc{};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.entryCount = 1;
        bindGroupLayoutDesc.entries = &bindingLayoutEntry;

        WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(this->_ctx.Device, &bindGroupLayoutDesc);

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Pipeline layout"),
            /*.bindGroupLayoutCount=*/1,
            /*.bindGroupLayouts=*/&bindGroupLayout,
            /*.immediateSize=*/0};

        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(this->_ctx.Device, &pipelineLayoutDesc);

        WGPUBindGroupEntry bindings = WGPU_BIND_GROUP_ENTRY_INIT;
        bindings.binding = 0;
        bindings.buffer = this->_uniformBuffer;
        bindings.size = sizeof(MyUniforms);

        WGPUBindGroupDescriptor bindGroupDesc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
        bindGroupDesc.nextInChain = nullptr;
        bindGroupDesc.layout = bindGroupLayout;
        bindGroupDesc.entryCount = 1;
        bindGroupDesc.entries = &bindings;
        this->_bindGroup = wgpuDeviceCreateBindGroup(_ctx.Device, &bindGroupDesc);

        WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
        bufferDesc.label = wgpuStr("Uniform Buffer");
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        bufferDesc.size = sizeof(MyUniforms);
        bufferDesc.mappedAtCreation = false;
        this->_uniformBuffer = wgpuDeviceCreateBuffer(this->_ctx.Device, &bufferDesc);

        _uniforms.ProjectionMatrix = Math::Mat4x4::Identity();
        _uniforms.ViewMatrix = Math::Mat4x4::Identity();
        _uniforms.ModelMatrix = Math::Mat4x4::Identity();
        _uniforms.Color = 0;
        _uniforms.Time = 0;
        wgpuQueueWriteBuffer(
            this->_queue,
            this->_uniformBuffer,
            0,
            &this->_uniforms,
            sizeof(MyUniforms));

        WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;
        WGPUDepthStencilState depthStencilState = {
            /*.nextInChain=*/nullptr,
            /*.format=*/depthTextureFormat,
            /*.depthWriteEnabled=*/WGPUOptionalBool_False,
            /*.depthCompare=*/WGPUCompareFunction_Less,
            /*.stencilFront=*/WGPU_STENCIL_FACE_STATE_INIT,
            /*.stencilBack=*/WGPU_STENCIL_FACE_STATE_INIT,
            /*.stencilReadMask=*/0,
            /*.stencilWriteMask=*/0,
            /*.depthBias=*/0,
            /*.depthBiasSlopeScale=*/0.f,
            /*.depthBiasClamp=*/0.f,
        };

        WGPUTextureDescriptor depthTextureDesc = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Depth texture"),
            /*.usage=*/WGPUTextureUsage_RenderAttachment,
            /*.dimension=*/WGPUTextureDimension_2D,
            /*.size=*/{800, 600, 1},
            /*.format=*/depthTextureFormat,
            /*.mipLevelCount=*/1,
            /*.sampleCount=*/1,
            /*.viewFormatCount=*/1,
            /*.viewFormats=*/&depthTextureFormat,
        };
        WGPUTexture depthTexture = wgpuDeviceCreateTexture(this->_ctx.Device, &depthTextureDesc);

        WGPUTextureViewDescriptor depthTextureViewDesc = {
            /*.nextInChain=*/nullptr,
            /*.label=*/wgpuStr("Depth texture view"),
            /*.format=*/depthTextureFormat,
            /*.dimension=*/WGPUTextureViewDimension_2D,
            /*.baseMipLevel=*/0,
            /*.mipLevelCount=*/1,
            /*.baseArrayLayer=*/0,
            /*.arrayLayerCount=*/1,
            /*.aspect=*/WGPUTextureAspect_DepthOnly,
            /*.usage=*/WGPUTextureUsage_RenderAttachment,
        };
        WGPUTextureView depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);

        this->_depthStencilAttachment = {
            /*.nextInChain=*/nullptr,
            /*.view=*/depthTextureView,
            /*.depthLoadOp=*/WGPULoadOp_Clear,
            /*.depthStoreOp=*/WGPUStoreOp_Store,
            /*.depthClearValue=*/1.0f,
            /*.depthReadOnly=*/false,
            /*.stencilLoadOp=*/WGPULoadOp_Undefined,
            /*.stencilStoreOp=*/WGPUStoreOp_Undefined,
            /*.stencilClearValue=*/0,
            /*.stencilReadOnly=*/false,
        };

        WGPUColorTargetState colorTargetState = {
            /*.nextInChain=*/nullptr,
            /*.format=*/this->_windowFormat,
            /*.blend=*/nullptr,
            /*.writeMask=*/WGPUColorWriteMask_All,
        };
        WGPUFragmentState fragmentState = {
            /*.nextInChain=*/nullptr,
            /*.module=*/shaderModule,
            /*.entryPoint=*/wgpuStr("fs_main"),
            /*.constantCount=*/0,
            /*.constants=*/nullptr,
            /*.targetCount=*/1,
            /*.targets=*/&colorTargetState,
        };

        WGPURenderPipelineDescriptor pipelineDesc = {
            /*.nextInChain=*/nullptr,
            /*.label=*/WGPU_STRING_VIEW_INIT,
            /*.layout=*/pipelineLayout,
            /*.vertex=*/{
                /*.nextInChain=*/nullptr,
                /*.module=*/shaderModule,
                /*.entryPoint=*/wgpuStr("vs_main"),
                /*.constantCount=*/0,
                /*.constants=*/nullptr,
                /*.bufferCount=*/1,
                /*.buffers=*/&vertexBufferLayout,
            },
            /*.primitive=*/{
                /*.nextInChain=*/nullptr,
                /*.topology=*/WGPUPrimitiveTopology_TriangleList,
                /*.stripIndexFormat=*/WGPUIndexFormat_Undefined,
                /*.frontFace=*/WGPUFrontFace_CCW,
                /*.cullMode=*/WGPUCullMode_Back,
                /*.unclippedDepth=*/false,
            },
            /*.depthStencil=*/&depthStencilState,
            /*.multisample=*/{
                /*.nextInChain=*/nullptr,
                /*.count=*/1,
                /*.mask=*/0xFFFFFFFF,
                /*.alphaToCoverageEnabled=*/false,
            },
            /*.fragment=*/&fragmentState,
        };
        this->_pipeline = wgpuDeviceCreateRenderPipeline(this->_ctx.Device, &pipelineDesc);
    }
}
