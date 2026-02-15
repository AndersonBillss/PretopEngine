#include "NATIVE_glfwWindow.hpp"
#include <iostream>
#include <chrono>

#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#include <windows.h>

GlfwWindow::~GlfwWindow()
{
    glfwTerminate();
}

GlfwWindow::GlfwWindow(int width, int height, std::string title) : Window(width, height, title)
{
    this->width_ = width;
    this->height_ = height;
    this->title_ = std::move(title);
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    _win = glfwCreateWindow(width, height, "GLFW Window", nullptr, nullptr);
}

GlfwWindow::GlfwWindow(std::string title) : GlfwWindow(default_width, default_height, title) {}

void GlfwWindow::setOnTick(Window::TickCallback cb)
{
    this->_onTick = cb;
}
void GlfwWindow::setOnExit(Window::ExitCallback cb)
{
    this->_onExit = cb;
}

void GlfwWindow::run()
{
    std::chrono::time_point last = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(_win))
    {
        std::chrono::time_point now = std::chrono::steady_clock::now();
        double dtSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - last).count();
        last = now;
        glfwPollEvents();
        this->_onTick(dtSeconds);
    }
    this->_onExit();
}

bool GlfwWindow::isInitialized()
{
    return _win != nullptr;
}

WGPUSurface GlfwWindow::getSurface(WGPUInstance instance)
{
    HWND hwnd = glfwGetWin32Window(this->_win);
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
}
