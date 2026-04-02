#include "appPipeline.hpp"

AppPipeline::AppPipeline(Application &app, AppShader shader)
{
    WGPURenderPipelineDescriptor pipelineDesc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;

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
    colorTarget.format = app.windowFormat;
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    this->wgpuPipeline = wgpuDeviceCreateRenderPipeline(app.device.wgpuDevice, &pipelineDesc);
}

AppPipeline::~AppPipeline()
{
    wgpuRenderPipelineRelease(this->wgpuPipeline);
}
