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

    std::string otherShaderSource = R"(
        struct VertexInput {
            @location(0) position: vec2f,
            @location(1) color: vec3f,
        };

        struct VertexOutput {
            @builtin(position) position: vec4f,
            @location(0) color: vec3f,
        };

        struct MyUniforms {
            color: f32,
            time: f32,
        };

        @group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var out: VertexOutput;
            let ratio = 640.0 / 480.0; // The width and height of the target surface
            let offset = vec2f(-0.6875, -0.463); // The offset that we want to apply to the position
            let offsetX = -cos(uMyUniforms.time) / 3;
            let offsetY = sin(uMyUniforms.time) / 3;
            out.position = vec4f(in.position.x + offset.x + offsetX, (in.position.y + offset.y + offsetY) * ratio, 0.0, 1.0);
            out.color = vec3f(in.color.x, in.color.y, in.color.z * uMyUniforms.color);
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

    AppBuffer vertices(application.device,
                       std::initializer_list<std::initializer_list<float>>{
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
    AppBuffer indices(application.device, std::initializer_list<std::initializer_list<uint16_t>>{
                                              {0, 1, 2},
                                              {3, 4, 5},
                                              {6, 7, 8},
                                              {9, 10, 11},
                                              {12, 13, 14},
                                          },
                      WGPUBufferUsage_Index);

    AppVertexLayout vertexLayout = {{LayoutType::Float32x2, LayoutType::Float32x3}};
    AppBindingLayout bindingLayout(application.device, {{sizeof(MyUniforms)}});
    AppPipeline pipeline(application.device, shader, application.windowFormat, vertexLayout, bindingLayout);
    AppBuffer myUniformBuffer(application.device, sizeof(MyUniforms), WGPUBufferUsage_Uniform);
    auto bindGroups = bindingLayout.createBindGroups(application.device, {{&myUniformBuffer}});

    application.writeBuf(myUniformBuffer);
    application.writeBuf(vertices);
    application.writeBuf(indices);

    float seconds = 0;
    application.run([&](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        seconds += dt;
                        MyUniforms uniforms;
                        uniforms.color = (sin(seconds * 2.32325) + 1) / 2;
                        uniforms.time = seconds;
                        myUniformBuffer.set(uniforms);
                        application.writeBuf(myUniformBuffer);

                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        std::vector<AppBuffer *> bufs = {&vertices};

                        commandBuffer.addCommand(command, pipeline, bufs, vertexLayout, indices, sizeof(uint16_t), bindGroups);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        application.submitCommandBuffer(commandBuffer);

                        return commandBuffer; });

    return 0;
}