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
#include "application/appBindingLayout.hpp"
#include "application/appBuffer.hpp"
#include "application/appBindGroup.hpp"
#include "math/ceilToBufferOffset.hpp"

struct MyUniforms
{
    float color;
    float time;
    float _pad[2];
};

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;
    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppShader shader = AppShader::pipeline(application.device, application.instance, "shaders/shader.wgsl");

    AppBuffer vertices(application.device,
                       std::initializer_list<std::initializer_list<float>>{
                           {-0.5, -0.5, -0.3, 1.0, 1.0, 1.0},
                           {+0.5, -0.5, -0.3, 1.0, 1.0, 1.0},
                           {+0.5, +0.5, -0.3, 1.0, 1.0, 1.0},
                           {-0.5, +0.5, -0.3, 1.0, 1.0, 1.0},
                           {+0.0, +0.0, +0.5, 0.5, 0.5, 0.5},
                       },
                       WGPUBufferUsage_Vertex);
    AppBuffer indices(application.device, std::initializer_list<std::initializer_list<uint16_t>>{
                                              {0, 1, 2},
                                              {0, 2, 3},
                                              {0, 1, 4},
                                              {1, 2, 4},
                                              {2, 3, 4},
                                              {3, 0, 4},
                                          },
                      WGPUBufferUsage_Index);

    AppVertexLayout vertexLayout = {{LayoutType::Float32x3, LayoutType::Float32x3}};

    WGPUBindGroupLayoutEntry bindingLayoutEntry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    bindingLayoutEntry.binding = 0;
    bindingLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayoutEntry.buffer.minBindingSize = sizeof(MyUniforms);
    bindingLayoutEntry.buffer.hasDynamicOffset = true;
    bindingLayoutEntry.visibility = WGPUShaderStage_Vertex;
    AppBindingLayout bindingLayout(application.device, {{bindingLayoutEntry}});

    AppPipeline pipeline(application.device, shader, application.windowFormat, vertexLayout, bindingLayout);
    uint32_t uniformOffsetSize = ceilToBufferOffset(sizeof(MyUniforms));
    AppBuffer myUniformBuffer(application.device, uniformOffsetSize * 2, WGPUBufferUsage_Uniform);

    std::vector<WGPUBindGroupEntry> bindings = {WGPU_BIND_GROUP_ENTRY_INIT};
    bindings[0].binding = 0;
    bindings[0].buffer = myUniformBuffer.wgpuBuffer;
    bindings[0].size = sizeof(MyUniforms);
    AppBindGroup bindGroup(application.device, bindingLayout.wgpuBindGroupLayouts[0], bindings);

    application.writeBuf(myUniformBuffer);
    application.writeBuf(vertices);
    application.writeBuf(indices);

    float seconds = 0;
    application.run([&](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        seconds += dt;
                        MyUniforms *u1 = myUniformBuffer.get<MyUniforms>();
                        u1->color = (sin(seconds * 2.32325) + 1) / 2;
                        u1->time = seconds;
                        MyUniforms *u2 = myUniformBuffer.get<MyUniforms>(256);
                        u2->color = (cos(seconds * 2.32325) + 1) / 2;
                        u2->time = -seconds;
                        application.writeBuf(myUniformBuffer);

                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        std::vector<AppBuffer *> bufs = {&vertices};

                        commandBuffer.addCommand(command)
                        ->setPipeline(pipeline)
                        .setVertexBuffers(bufs)
                        .setBindGroup(&bindGroup, 0, {0})
                        .drawIndexed(indices, indices.numBytes() / sizeof(uint16_t))
                        // .setBindGroup(&bindGroup, 0, {256})
                        // .drawIndexed(indices, indices.numBytes() / sizeof(uint16_t))
                        .finish();

                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        application.submitCommandBuffer(commandBuffer);

                        return commandBuffer; });

    return 0;
}