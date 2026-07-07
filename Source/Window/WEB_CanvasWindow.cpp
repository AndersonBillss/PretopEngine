#include "WEB_CanvasWindow.hpp"
#include <emscripten/emscripten.h>
#include <iostream>

// EM_JS(void, setDocumentTitle, (const char *title), {
//     const myTitle = UTF8ToString(title);
//     console.log("New document title:", myTitle);
//     document.title = myTitle;
// });

CanvasWindow::~CanvasWindow()
{
}

CanvasWindow::CanvasWindow(unsigned int width, unsigned int height, std::string title) : Window(width, height, title)
{
    this->Width = width;
    this->Height = height;
    this->_title = std::move(title);
    // setDocumentTitle(this->_title.c_str());
}

CanvasWindow::CanvasWindow(std::string title) : CanvasWindow(DefaultWidth, DefaultHeight, title) {}

void CanvasWindow::SetOnTick(Window::TickCallback callback)
{
    this->_onTick = callback;
}

void CanvasWindow::SetOnExit(Window::ExitCallback callback)
{
    this->_onExit = callback;
}

void CanvasWindow::Run()
{
    em_arg_callback_func callback = [](void *data)
    {
        auto *self = static_cast<CanvasWindow *>(data);
        std::chrono::time_point now = std::chrono::steady_clock::now();
        double dtSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - self->_lastFrame).count();
        self->_lastFrame = now;
        self->_onTick(dtSeconds);
    };
    emscripten_set_main_loop_arg(callback, this, 0, 1);
}

bool CanvasWindow::IsInitialized()
{
    return true;
}

WGPUSurface CanvasWindow::GetSurface(WGPUInstance instance)
{
    WGPUEmscriptenSurfaceSourceCanvasHTMLSelector canvasSelector = WGPU_EMSCRIPTEN_SURFACE_SOURCE_CANVAS_HTML_SELECTOR_INIT;
    canvasSelector.chain.sType = WGPUSType_EmscriptenSurfaceSourceCanvasHTMLSelector;
    std::string canvasId = "canvas";
    canvasSelector.selector = WGPUStringView{
        /* data */ canvasId.c_str(),
        /* length */ canvasId.length(),
    };
    WGPUSurfaceDescriptor desc = WGPU_SURFACE_DESCRIPTOR_INIT;
    desc.nextInChain = &canvasSelector.chain;
    return wgpuInstanceCreateSurface(instance, &desc);
}
