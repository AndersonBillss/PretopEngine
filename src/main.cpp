#include <iostream>
#include <unordered_map>
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"
#include "application/appDevice.hpp"
#include "application/appShader.hpp"
#include "application/appPipeline.hpp"
#include "application/appCommand.hpp"

int main(int, char **)
{
    std::cout << "Hello, WebGPU!!" << std::endl;

    Application application;

    std::string shaderSource = R"(
        @vertex
        fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
            var p = vec2f(0.0, 0.0);
            if (in_vertex_index == 0u) {
                p = vec2f(-0.5, -0.5);
            } else if (in_vertex_index == 1u) {
                p = vec2f(0.5, -0.5);
            } else {
                p = vec2f(0.0, 0.5);
            }
            return vec4f(p, 0.0, 1.0);
        }

        @fragment
        fn fs_main() -> @location(0) vec4f {
            return vec4f(0.0, 0.4, 1.0, 1.0);
        }
    )";
    AppShader shader = AppShader::pipeline(application.device, application.instance, shaderSource);

    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppPipeline pipeline(application.device, shader, application.windowFormat);

    application.run([&application, &pipeline](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppCommand command(application.device, pipeline, targetView);
                        std::cout << "Submitting command..." << std::endl;
                        return command; });
    return 0;
}