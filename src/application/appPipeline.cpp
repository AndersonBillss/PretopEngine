#include "appPipeline.hpp"

AppPipeline::AppPipeline(
    AppDevice &device,
    AppShader &shader,
    WGPUTextureFormat format,
    AppVertexLayout &vertexLayout,
    AppBindingLayout &pipelineLayout)
{
    std::vector<WGPUVertexBufferLayout> layouts = {};
    for (const auto &layout : vertexLayout.bufferLayouts)
    {
        WGPUVertexBufferLayout vertexBufferLayout = WGPU_VERTEX_BUFFER_LAYOUT_INIT;
        vertexBufferLayout.attributeCount = layout.wgpuAttrs.size();
        vertexBufferLayout.attributes = layout.wgpuAttrs.data();
        vertexBufferLayout.arrayStride = layout.arrayStride;
        vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
        layouts.push_back(vertexBufferLayout);
    }

    WGPURenderPipelineDescriptor pipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;
    pipelineDesc.vertex.bufferCount = layouts.size();
    pipelineDesc.layout = pipelineLayout.wgpuLayout;
    pipelineDesc.vertex.buffers = layouts.data();

    WGPUDepthStencilState depthStencilState = WGPU_DEPTH_STENCIL_STATE_INIT;
    depthStencilState.format = WGPUTextureFormat_Undefined;
    depthStencilState.depthWriteEnabled = WGPUOptionalBool_False;
    depthStencilState.depthCompare = WGPUCompareFunction_Always;
    depthStencilState.stencilReadMask = 0xFFFFFFFF;
    depthStencilState.stencilWriteMask = 0xFFFFFFFF;
    depthStencilState.depthBias = 0;
    depthStencilState.depthBiasSlopeScale = 0;
    depthStencilState.depthBiasClamp = 0;
    depthStencilState.depthCompare = WGPUCompareFunction_Less;
    depthStencilState.depthWriteEnabled = WGPUOptionalBool_True;
    // Store the format in a variable as later parts of the code depend on it
    WGPUTextureFormat depthTextureFormat = WGPUTextureFormat_Depth24Plus;
    depthStencilState.format = depthTextureFormat;
    // Deactivate the stencil alltogether
    depthStencilState.stencilReadMask = 0;
    depthStencilState.stencilWriteMask = 0;
    pipelineDesc.depthStencil = &depthStencilState;
    // Create the depth texture
    WGPUTextureDescriptor depthTextureDesc = WGPU_TEXTURE_DESCRIPTOR_INIT;
    depthTextureDesc.dimension = WGPUTextureDimension_2D;
    depthTextureDesc.format = depthTextureFormat;
    depthTextureDesc.mipLevelCount = 1;
    depthTextureDesc.sampleCount = 1;
    depthTextureDesc.size = {800, 600, 1};
    depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;
    depthTextureDesc.viewFormatCount = 1;
    depthTextureDesc.viewFormats = &depthTextureFormat;
    WGPUTexture depthTexture = wgpuDeviceCreateTexture(device.wgpuDevice, &depthTextureDesc);

    // Create the view of the depth texture manipulated by the rasterizer
    WGPUTextureViewDescriptor depthTextureViewDesc = WGPU_TEXTURE_VIEW_DESCRIPTOR_INIT;
    depthTextureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
    depthTextureViewDesc.baseArrayLayer = 0;
    depthTextureViewDesc.arrayLayerCount = 1;
    depthTextureViewDesc.baseMipLevel = 0;
    depthTextureViewDesc.mipLevelCount = 1;
    depthTextureViewDesc.dimension = WGPUTextureViewDimension_2D;
    depthTextureViewDesc.format = depthTextureFormat;
    WGPUTextureView depthTextureView = wgpuTextureCreateView(depthTexture, &depthTextureViewDesc);

    // We now add a depth/stencil attachment:
    this->wgpuDepthStencilAttachment = WGPU_RENDER_PASS_DEPTH_STENCIL_ATTACHMENT_INIT;
    // Setup depth/stencil attachment

    // The view of the depth texture
    this->wgpuDepthStencilAttachment.view = depthTextureView;

    // The initial value of the depth buffer, meaning "far"
    this->wgpuDepthStencilAttachment.depthClearValue = 1.0f;
    // Operation settings comparable to the color attachment
    this->wgpuDepthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
    this->wgpuDepthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
    // we could turn off writing to the depth buffer globally here
    this->wgpuDepthStencilAttachment.depthReadOnly = false;

    pipelineDesc.depthStencil = &depthStencilState;

    WGPUFragmentState fragmentState = WGPU_FRAGMENT_STATE_INIT;
    fragmentState.module = shader.wgpuShader;
    std::string fragmentEntryPoint = "fs_main";
    fragmentState.entryPoint = WGPUStringView{fragmentEntryPoint.c_str(), fragmentEntryPoint.size()};
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;

    pipelineDesc.vertex.module = shader.wgpuShader;
    std::string vertexEntryPoint = "vs_main";
    pipelineDesc.vertex.entryPoint = WGPUStringView{vertexEntryPoint.c_str(), vertexEntryPoint.size()};

    WGPUBlendState blendState{};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget = WGPU_COLOR_TARGET_STATE_INIT;
    colorTarget.format = format;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    this->wgpuPipeline = wgpuDeviceCreateRenderPipeline(device.wgpuDevice, &pipelineDesc);
}

AppPipeline::~AppPipeline()
{
    wgpuRenderPipelineRelease(this->wgpuPipeline);
}
