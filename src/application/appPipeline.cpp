#include "appPipeline.hpp"
#include <iostream>

AppPipeline::AppPipeline(
    AppDevice device,
    AppShader shader,
    WGPUTextureFormat format,
    std::vector<AppVertexBufferLayout> vertexLayout)
{
    std::vector<WGPUVertexBufferLayout> layouts = {};
    for (const auto &layout : vertexLayout)
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
    pipelineDesc.vertex.buffers = layouts.data();

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
