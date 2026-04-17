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
    std::string otherShaderSource = R"(
        /**
         * A structure with fields labeled with vertex attribute locations can be used
         * as input to the entry point of a shader.
         */
        struct VertexInput {
            @location(0) position: vec2f,
            @location(1) color: vec3f,
        };

        /**
         * A structure with fields labeled with builtins and locations can also be used
         * as *output* of the vertex shader, which is also the input of the fragment
         * shader.
         */
        struct VertexOutput {
            @builtin(position) position: vec4f,
            // The location here does not refer to a vertex attribute, it just means
            // that this field must be handled by the rasterizer.
            // (It can also refer to another field of another struct that would be used
            // as input to the fragment shader.)
            @location(0) color: vec3f,
        };

        @vertex
        fn vs_main(in: VertexInput) -> VertexOutput {
            var out: VertexOutput; // create the output struct
            out.position = vec4f(in.position, 0.0, 1.0); // same as what we used to directly return
            out.color = in.color; // forward the color attribute to the fragment shader
            return out;
        }

        @fragment
        fn fs_main(in: VertexOutput) -> @location(0) vec4f {
            return vec4f(in.color, 1.0); // use the interpolated color coming from the vertex shader
        }
    )";
    AppShader shader = AppShader::pipeline(application.device, application.instance, otherShaderSource);

    application.logQueueCommands();
    application.setWindow(WindowFactory::createWindow("My Window"));

    AppVertexBuffer<float> buf(application.device,
                               {{-0.5, -0.5},
                                {+0.5, -0.5},
                                {+0.0, +0.5},

                                {-0.55f, -0.5},
                                {-0.05f, +0.5},
                                {-0.55f, +0.5}});

    AppVertexBuffer<float> buf2(application.device,
                                {// x,  y,    r,   g,   b
                                 {-0.5, -0.5, 1.0, 0.0, 0.0},
                                 {+0.5, -0.5, 0.0, 1.0, 0.0},
                                 {+0.0, +0.5, 0.0, 0.0, 1.0},
                                 {-0.55f, -0.5, 1.0, 1.0, 0.0},
                                 {-0.05f, +0.5, 1.0, 0.0, 1.0},
                                 {-0.55f, +0.5, 0.0, 1.0, 1.0}});

    AppPipeline pipeline(application.device, shader, application.windowFormat);

    application.writeVertices(buf2);

    application.run([&application, &pipeline, &buf2](
                        double dt,
                        WGPUTextureView targetView)
                    {
                        AppCommandBuffer commandBuffer(application.device);
                        std::cout << "DELTATIME: " << dt << std::endl;
                        AppRenderPassCommand command(application.device, targetView);
                        commandBuffer.addCommand(command, pipeline, buf2);
                        std::cout << "Submitting command..." << std::endl;
                        commandBuffer.finish();
                        return commandBuffer; });

    return 0;
}