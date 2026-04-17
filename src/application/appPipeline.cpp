#include "appPipeline.hpp"

AppPipeline::AppPipeline(AppDevice device, AppShader shader, WGPUTextureFormat format)
{
    WGPUVertexAttribute positionAttrs[2];
    positionAttrs[0].shaderLocation = 0;
    positionAttrs[0].format = WGPUVertexFormat_Float32x2;
    positionAttrs[0].offset = 0;
    positionAttrs[1].shaderLocation = 1;
    positionAttrs[1].format = WGPUVertexFormat_Float32x3;
    positionAttrs[1].offset = 2 * sizeof(float);

    WGPUVertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.attributeCount = 2;
    vertexBufferLayout.attributes = positionAttrs;
    vertexBufferLayout.arrayStride = 5 * sizeof(float);
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    WGPURenderPipelineDescriptor pipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;
    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;

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
