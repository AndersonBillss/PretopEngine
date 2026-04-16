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
#include "application/appVertexBuffer.hpp"

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

    AppVertexBuffer<float> buf(application.device,
                               {{-0.5, -0.5},
                                {+0.5, -0.5},
                                {+0.0, +0.5},

                                {-0.55f, -0.5},
                                {-0.05f, +0.5},
                                {-0.55f, +0.5}});

    AppPipeline pipeline(application.device, shader, application.windowFormat);

    application.writeVertices(buf);

    application.run([&application, &pipeline, &buf](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        commandBuffer.addCommand(command, pipeline, buf);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        return commandBuffer; });

    return 0;
}