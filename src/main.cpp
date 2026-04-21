#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>

#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"
#include "application/appShader.hpp"
#include "application/appPipeline.hpp"
#include "application/appCommandBuffer.hpp"
#include "application/appRenderPassCommand.hpp"
#include "application/appVertexBuffer.hpp"
#include "application/appVertexLayout.hpp"

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

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var out: VertexOutput;
            out.position = vec4f(in.position, 0.0, 1.0);
            out.color = in.color;
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

    AppVertexBuffer<float> buf1(application.device,
                                {// x,  y,    r,   g,   b
                                 {-0.5, -0.5},
                                 {+0.5, -0.5},
                                 {+0.0, +0.5},
                                 {-0.55f, -0.5},
                                 {-0.05f, +0.5},
                                 {-0.55f, +0.5}});
    AppVertexBuffer<float> buf2(application.device,
                                {// x,  y,    r,   g,   b
                                 {1.0, 0.0, 0.0},
                                 {0.0, 1.0, 0.0},
                                 {0.0, 0.0, 1.0},
                                 {1.0, 1.0, 0.0},
                                 {1.0, 0.0, 1.0},
                                 {0.0, 1.0, 1.0}});

    AppVertexLayout layout = {{LayoutType::Float32x2}, {LayoutType::Float32x3}};
    AppPipeline pipeline(application.device, shader, application.windowFormat, layout);

    application.writeVertices(std::initializer_list<AppVertexBuffer<float> *>{&buf1, &buf2});

    application.run([&application, &pipeline, &buf1, &buf2](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        std::vector<AppVertexBuffer<float>*> bufs = {&buf1, &buf2};
                        commandBuffer.addCommand(command, pipeline, bufs);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        application.submitCommandBuffer(commandBuffer);
                        return commandBuffer; });

    return 0;
}