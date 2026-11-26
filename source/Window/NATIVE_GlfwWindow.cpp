#include <iostream>
#include <chrono>

#ifdef PRETOP_PLATFORM_WINDOWS
#include "../Core/WindowsCompat.hpp"
#endif // PRETOP_PLATFORM_WINDOWS

#include "NATIVE_GlfwWindow.hpp"
#include "../Core/Platform.hpp"

#ifdef PRETOP_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // PRETOP_PLATFORM_WINDOWS

#ifdef PRETOP_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#endif // PRETOP_PLATFORM_LINUX

#include <GLFW/glfw3native.h>

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
#ifdef PRETOP_PLATFORM_WINDOWS
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
#endif // PRETOP_PLATFORM_WINDOWS
#ifdef PRETOP_PLATFORM_LINUX
        switch (glfwGetPlatform())
        {
        case GLFW_PLATFORM_WAYLAND:
        {
            struct WGPUSurfaceSourceWaylandSurface fromWayland;
            fromWayland.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
            fromWayland.chain.next = NULL;
            fromWayland.display = glfwGetWaylandDisplay();
            fromWayland.surface = glfwGetWaylandWindow(this->_window);

            WGPUSurfaceDescriptor surfaceDescriptor;
            surfaceDescriptor.nextInChain = &fromWayland.chain;
            surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

            return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
        }
        case GLFW_PLATFORM_X11:
        {
            struct WGPUSurfaceSourceXlibWindow fromX11{};
            fromX11.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
            fromX11.chain.next = NULL;
            fromX11.display = glfwGetX11Display();
            fromX11.window = glfwGetX11Window(this->_window);

            WGPUSurfaceDescriptor surfaceDescriptor;
            surfaceDescriptor.nextInChain = &fromX11.chain;
            surfaceDescriptor.label = WGPU_STRING_VIEW_INIT;

            return wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
        }
        default:
        {
            return nullptr;
        }
        }
#endif // PRETOP_PLATFORM_LINUX
    }
} // namespace Pretop::Window
