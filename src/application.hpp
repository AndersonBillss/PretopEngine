#include "window/window.hpp"
#include <webgpu/webgpu.h>

class Application
{
public:
    Application();
    void setWindowSurface(Window *win);
    WGPUTextureView getNextSurfaceTextureView();

private:
    void createInstance();
    void createDevice();
    void createAdapter();
    void createQueue();

    Application *inspectInstance();
    Application *inspectDevice();
    Application *inspectAdapter();
    Application *inspectQueue();

    WGPUInstance _instance;
    WGPUAdapter _adapter;
    WGPUDevice _device;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
};
