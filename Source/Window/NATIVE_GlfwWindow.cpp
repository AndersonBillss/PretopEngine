#include "NATIVE_GlfwWindow.hpp"
#include <iostream>
#include <chrono>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <windows.h>
#endif // _WIN32

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3native.h>
#endif // __linux__

namespace Pretop::Window
{
    GlfwWindow::~GlfwWindow()
    {
        glfwTerminate();
    }

    GlfwWindow::GlfwWindow(unsigned int width, unsigned int height, std::string title) : Window(width, height, title)
    {
        this->Width = width;
        this->Height = height;
        this->_title = std::move(title);
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(-1);
        }
        _window = glfwCreateWindow(width, height, this->_title.c_str(), nullptr, nullptr);
    }

    GlfwWindow::GlfwWindow(std::string title) : GlfwWindow(DefaultWidth, DefaultHeight, title) {}

    void GlfwWindow::SetOnTick(Window::TickCallback callback)
    {
        this->_onTick = callback;
    }
    void GlfwWindow::SetOnExit(Window::ExitCallback callback)
    {
        this->_onExit = callback;
    }

    void GlfwWindow::Run()
    {
        std::chrono::time_point last = std::chrono::steady_clock::now();
        while (!glfwWindowShouldClose(_window))
        {
            std::chrono::time_point now = std::chrono::steady_clock::now();
            double dtSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - last).count();
            last = now;
            glfwPollEvents();
            this->_onTick(dtSeconds);
        }
        this->_onExit();
    }

    bool GlfwWindow::IsInitialized()
    {
        return _window != nullptr;
    }

    WGPUSurface GlfwWindow::GetSurface(WGPUInstance instance)
    {
#ifdef _WIN32
        HWND hwnd = glfwGetWin32Window(this->_window);
        HINSTANCE hinstance = GetModuleHandle(NULL);

        WGPUSurfaceSourceWindowsHWND fromWindowsHWND;
        fromWindowsHWND.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
        fromWindowsHWND.chain.next = NULL;
        fromWindowsHWND.hinstance = hinstance;
        fromWindowsHWND.hwnd = hwnd;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromWindowsHWND.chain;
        surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
#endif // _WIN32
#ifdef __linux__
        wl_surface *surface = glfwGetWaylandWindow(this->_window);
        wl_display *display = glfwGetWaylandDisplay();

        struct WGPUSurfaceSourceWaylandSurface fromWayland;
        fromWayland.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
        fromWayland.chain.next = NULL;
        fromWayland.display = display;
        fromWayland.surface = surface;

        WGPUSurfaceDescriptor surfaceDescriptor;
        surfaceDescriptor.nextInChain = &fromWayland.chain;
        surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

        return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
#endif // __linux__
    }
} // namespace Pretop::Window
