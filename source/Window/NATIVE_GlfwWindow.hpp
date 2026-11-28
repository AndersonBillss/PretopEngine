#pragma once
#include "Window.hpp"
#include "NATIVE_GlfwInputManager.hpp"
#include <GLFW/glfw3.h>
#include <string>

namespace Pretop::Window
{
    class GlfwWindow : public Window
    {
    public:
        ~GlfwWindow() override;
        GlfwWindow(unsigned int width, unsigned int height, std::string title);
        GlfwWindow(std::string title);

        void SetOnTick(Window::TickCallback callback) override;
        void SetOnExit(Window::ExitCallback callback) override;
        void Run() override;
        bool IsInitialized() override;
        WGPUSurface GetSurface(WGPUInstance instance) override;
        // std::unique_ptr<GlfwInputManager> Input;

    private:
        Window::TickCallback _onTick;
        Window::ExitCallback _onExit;
        GLFWwindow *_window;
    };
} // namespace Pretop::Window
