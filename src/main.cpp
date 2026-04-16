#include <iostream>
#include <unordered_map>
#include <vector>

#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"
#include "application/appShader.hpp"
#include "application/appPipeline.hpp"
#include "application/appCommandBuffer.hpp"
#include "application/appRenderPassCommand.hpp"

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;

    std::string shaderSource = R"(
        @vertex
        fn vs_main(@location(0) in_vertex_position: vec2f) -> @builtin(position) vec4f {
            return vec4f(in_vertex_position, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.0, 0.4, 1.0, 1.0);
        }
    )";
    AppShader shader = AppShader::pipeline(application.device, application.instance, shaderSource);

    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    WGPUBufferDescriptor bufferDesc = WGPU_BUFFER_DESCRIPTOR_INIT;
    bufferDesc.nextInChain = nullptr;
    const std::string bufferLabel = "Test buffer";
    bufferDesc.label = WGPUStringView{bufferLabel.c_str(), bufferLabel.size()};
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    bufferDesc.size = 24;
    bufferDesc.mappedAtCreation = false;
    WGPUBuffer buffer1 = wgpuDeviceCreateBuffer(application.device.wgpuDevice, &bufferDesc);

    std::vector<float> vertexData = {
        -0.5, -0.5,
        +0.5, -0.5,
        +0.0, +0.5};

    AppPipeline pipeline(application.device, shader, application.windowFormat, buffer1);

    uint32_t vertexCount = static_cast<uint32_t>(vertexData.size() / 2);
    application.writeVertices(buffer1, vertexData);

    application.run([&application, &pipeline, &buffer1](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        commandBuffer.addCommand(command, pipeline, buffer1);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        return commandBuffer; });

    wgpuBufferRelease(buffer1);
    return 0;
}