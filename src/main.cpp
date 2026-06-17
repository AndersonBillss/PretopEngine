#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#define _USE_MATH_DEFINES
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
#include "math/linalg/mat4x4.hpp"
#include "math/constants.hpp"
#include "math/euler.hpp"

#include "asset/loadGlb.hpp"
#include "asset/modelParseError.hpp"

struct MyUniforms
{
    Mat4x4 projectionMatrix;
    Mat4x4 viewMatrix;
    Mat4x4 modelMatrix;
    float color;
    float time;
    float _pad[2];
};

const float scale = 3.0f;
int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    ParsedData model;
    try
    {
        model = loadGlb("assets/models/DiffuseTransmissionPlant.glb");
        std::cout << "Success!" << std::endl;
    }
    catch (ModelParseError &e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    Application application;
    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppShader shader = AppShader::pipeline(application.device, application.instance, "shaders/shader.wgsl");

    AppBuffer vertices(application.device, model.vertices.size() * sizeof(Vertex), WGPUBufferUsage_Vertex);
    application.writeVec(vertices, model.vertices);

    uint32_t indicesSize = 0;
    bool is32bitIndexBuffer = std::holds_alternative<std::vector<uint32_t>>(model.indices);
    if (is32bitIndexBuffer)
    {
        std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.indices);
        indicesSize = indicesVec.size() * sizeof(uint32_t);
    }
    else
    {
        std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.indices);
        indicesSize = indicesVec.size() * sizeof(uint16_t);
    }
    AppBuffer indices(application.device, indicesSize, WGPUBufferUsage_Index);
    if (is32bitIndexBuffer)
    {
        std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.indices);
        application.writeVec(indices, indicesVec);
    }
    else
    {
        std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.indices);
        application.writeVec(indices, indicesVec);
    }
    AppVertexLayout vertexLayout = {{LayoutType::Float32x3, LayoutType::Float32x3}};

    WGPUBindGroupLayoutEntry bindingLayoutEntry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    bindingLayoutEntry.binding = 0;
    bindingLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayoutEntry.buffer.minBindingSize = sizeof(MyUniforms);
    bindingLayoutEntry.buffer.hasDynamicOffset = true;
    bindingLayoutEntry.visibility = WGPUShaderStage_Vertex;
    AppBindingLayout bindingLayout(application.device, {{bindingLayoutEntry}});

    AppPipeline pipeline(application.device, shader, application.windowFormat, vertexLayout, bindingLayout);
    AppBuffer myUniformBuffer(application.device, sizeof(MyUniforms), WGPUBufferUsage_Uniform);

    std::vector<WGPUBindGroupEntry> bindings = {WGPU_BIND_GROUP_ENTRY_INIT};
    bindings[0].binding = 0;
    bindings[0].buffer = myUniformBuffer.wgpuBuffer;
    bindings[0].size = sizeof(MyUniforms);
    AppBindGroup bindGroup(application.device, bindingLayout.wgpuBindGroupLayouts[0], bindings);
    application.writeBufZero(myUniformBuffer);

    float seconds = 0;
    application.run([&](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        seconds += dt;
                        MyUniforms u;
                        u.color = (sin(seconds * 2.32325) + 1) / 2;
                        u.time = seconds;

                        Mat4x4 R1 = (Euler{90.0f * (float)deg2rad, 0, seconds}).toMatrix();
                        Mat4x4 S = Mat4x4::scale(scale);
                        u.modelMatrix = R1 * S;

                        Mat4x4 R2 = (Euler{-45.0f * (float)deg2rad, 0, 0}).toMatrix();
                        Mat4x4 T2 = Mat4x4::transform(0.0f, 0.0f, -4.0f);
                        u.viewMatrix = T2 * R2;

                        float near = 0.01f;
                        float far = 100.0f;
                        u.projectionMatrix = Mat4x4::perspective(0.01f, 100.0f, 60.0f * deg2rad, 640.0 / 480.0);
                        application.writeBuf(myUniformBuffer, u);

                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView, pipeline.wgpuDepthStencilAttachment);
                        std::vector<AppBuffer *> bufs = {&vertices};

                        auto cmd = commandBuffer.addCommand(command);
                        cmd->setPipeline(pipeline)
                        .setVertexBuffers(bufs)
                        .setBindGroup(&bindGroup, 0, {0});
                        if (is32bitIndexBuffer)
                        {
                            std::vector<uint32_t> indicesVec = std::get<std::vector<uint32_t>>(model.indices);
                            cmd->drawIndexed(indices, indicesVec.size(), WGPUIndexFormat_Uint32);
                        }
                        else
                        {
                            std::vector<uint16_t> indicesVec = std::get<std::vector<uint16_t>>(model.indices);
                            cmd->drawIndexed(indices, indicesVec.size(), WGPUIndexFormat_Uint16);
                        }
                        cmd->finish();

                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        application.submitCommandBuffer(commandBuffer);

                        return commandBuffer; });

    return 0;
}