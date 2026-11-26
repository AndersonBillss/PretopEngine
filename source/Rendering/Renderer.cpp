#include <array>
#include <cstring>
#include "Renderer.hpp"

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

    Renderer::Renderer(Core::GraphicsContext ctx, WGPUSurface windowSurface)
    {
        this->_ctx = ctx;
        this->_windowSurface = windowSurface;

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
    }

    void Renderer::AddObject(Asset::AssetManager::AssetReference<Asset::ParsedData> object)
    {
        this->_objects.push_back(object);
    }

    void Renderer::render()
    {
    }

    void Renderer::updateMVP(Math::Mat4x4 Model, Math::Mat4x4 View, Math::Mat4x4 Projection)
    {
    }

    void Renderer::_initializeBindings()
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

        wgpuDeviceCreatePipelineLayout(this->_ctx.Device, &pipelineLayoutDesc);

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
    }
}
