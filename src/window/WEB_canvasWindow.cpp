#include "WEB_canvasWindow.hpp"
#include <emscripten/emscripten.h>
#include <iostream>

EM_JS(void, setDocumentTitle, (const char *title), {
    const myTitle = UTF8ToString(title);
    console.log("New document title:", myTitle);
    document.title = myTitle;
});

CanvasWindow::~CanvasWindow()
{
}

CanvasWindow::CanvasWindow(int width, int height, std::string title) : Window(width, height, title)
{
    this->width = width;
    this->height = height;
    this->title_ = std::move(title);
    setDocumentTitle(this->title_.c_str());
}

CanvasWindow::CanvasWindow(std::string title) : CanvasWindow(default_width, default_height, title) {}

void CanvasWindow::setOnTick(Window::TickCallback cb)
{
    this->_onTick = cb;
}

void CanvasWindow::setOnExit(Window::ExitCallback cb)
{
    this->_onExit = cb;
}

void CanvasWindow::run()
{
    em_arg_callback_func cb = [](void *data)
    {
        auto *self = static_cast<CanvasWindow *>(data);
        std::chrono::time_point now = std::chrono::steady_clock::now();
        double dtSeconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - self->_lastFrame).count();
        self->_lastFrame = now;
        self->_onTick(dtSeconds);
    };
    emscripten_set_main_loop_arg(cb, this, 0, 1);
}

bool CanvasWindow::isInitialized()
{
    return true;
}

WGPUSurface CanvasWindow::getSurface(WGPUInstance instance)
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
