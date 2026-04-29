#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <cmath>

#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"
#include "application/appShader.hpp"
#include "application/appPipeline.hpp"
#include "application/appCommandBuffer.hpp"
#include "application/appRenderPassCommand.hpp"
#include "application/appVertexLayout.hpp"
#include "application/appBuffer.hpp"

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;

    std::string otherShaderSource = R"(
        struct VertexInput {
            @location(0) position: vec2f,
            @location(1) color: vec3f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) color: vec3f,
        };

        @group(0) @binding(0) var<uniform> uTime: f32;

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var out: VertexOutput;
            let ratio = 640.0 / 480.0; // The width and height of the target surface
            let offset = vec2f(-0.6875, -0.463); // The offset that we want to apply to the position
            out.position = vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
            let brightness = (sin(uTime) + 1)  / 2;
            out.color = vec3f(in.color.x, in.color.y, in.color.z * brightness);
            return out;
        }

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            return vec4f(in.color, 1.0);
        }
    )";
    AppShader shader = AppShader::pipeline(application.device, application.instance, otherShaderSource);

    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppBuffer<float> buf(application.device,
                         {
                             {0.5, 0.0, 0.0, 0.353, 0.612},
                             {1.0, 0.866, 0.0, 0.353, 0.612},
                             {0.0, 0.866, 0.0, 0.353, 0.612},
                             {0.75, 0.433, 0.0, 0.4, 0.7},
                             {1.25, 0.433, 0.0, 0.4, 0.7},
                             {1.0, 0.866, 0.0, 0.4, 0.7},
                             {1.0, 0.0, 0.0, 0.463, 0.8},
                             {1.25, 0.433, 0.0, 0.463, 0.8},
                             {0.75, 0.433, 0.0, 0.463, 0.8},
                             {1.25, 0.433, 0.0, 0.525, 0.91},
                             {1.375, 0.65, 0.0, 0.525, 0.91},
                             {1.125, 0.65, 0.0, 0.525, 0.91},
                             {1.125, 0.65, 0.0, 0.576, 1.0},
                             {1.375, 0.65, 0.0, 0.576, 1.0},
                             {1.25, 0.866, 0.0, 0.576, 1.0},
                         },
                         WGPUBufferUsage_Vertex);
    AppBuffer<uint16_t> indices(application.device, {
                                                        {0, 1, 2},
                                                        {3, 4, 5},
                                                        {6, 7, 8},
                                                        {9, 10, 11},
                                                        {12, 13, 14},
                                                    },
                                WGPUBufferUsage_Index);

    AppVertexLayout layout = {{LayoutType::Float32x2, LayoutType::Float32x3}};

    // Define binding layout
    WGPUBindGroupLayoutEntry bindingLayout = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;

    // The binding index as used in the @binding attribute in the shader
    bindingLayout.binding = 0;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = sizeof(float);

    // The stage that needs to access this resource
    bindingLayout.visibility = WGPUShaderStage_Vertex;
    // Create a bind group layout
    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(application.device.wgpuDevice, &bindGroupLayoutDesc);

    // Create the pipeline layout
    WGPUPipelineLayoutDescriptor layoutDesc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;
    WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(application.device.wgpuDevice, &layoutDesc);
    AppPipeline pipeline(application.device, shader, application.windowFormat, layout, pipelineLayout);

    AppBuffer<float> uTime(application.device, {{1}}, WGPUBufferUsage_Uniform);

    WGPUBindGroupEntry binding = WGPU_BIND_GROUP_ENTRY_INIT;
    // The index of the binding (the entries in bindGroupDesc can be in any order)
    binding.binding = 0;
    // The buffer it is actually bound to
    binding.buffer = uTime.wgpuBuffer;
    // We can specify an offset within the buffer, so that a single buffer can hold
    // multiple uniform blocks.
    binding.offset = 0;
    // And we specify again the size of the buffer.
    binding.size = sizeof(float);

    // A bind group contains one or multiple bindings
    WGPUBindGroupDescriptor bindGroupDesc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = bindGroupLayout;
    // There must be as many bindings as declared in the layout!
    bindGroupDesc.entryCount = 1;
    bindGroupDesc.entries = &binding;
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(application.device.wgpuDevice, &bindGroupDesc);

    application.writeBuf(uTime);
    application.writeVertices(std::initializer_list<AppBuffer<float> *>{&buf});
    application.writeIndex(indices);

    float seconds = 0;
    application.run([&application, &pipeline, &buf, &indices, &bindGroup, &uTime, &seconds](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        seconds += dt;
                        float *rawData = (float *)uTime.rawData();
                        *rawData = seconds;
                        application.writeBuf(uTime);

                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        std::vector<AppBuffer<float>*> bufs = {&buf};

                        commandBuffer.addCommand(command, pipeline, bufs, indices, bindGroup);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        application.submitCommandBuffer(commandBuffer);

                        return commandBuffer; });

    wgpuBindGroupRelease(bindGroup);
    wgpuPipelineLayoutRelease(pipelineLayout);
    wgpuBindGroupLayoutRelease(bindGroupLayout);

    return 0;
}