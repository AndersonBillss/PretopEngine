#include "window/window.hpp"
#include <webgpu/webgpu.h>

class Application
{
public:
    Application();
    void setWindow(Window *win);
    WGPUTextureView getNextSurfaceTextureView();
    void logQueueCommands();

    Application *inspectInstance();
    Application *inspectDevice();
    Application *inspectAdapter();
    Application *inspectQueue();


private:
    void createInstance();
    void createDevice();
    void createAdapter();
    void createQueue();

    bool _logQueueCommands;
    WGPUInstance _instance;
    WGPUAdapter _adapter;
    WGPUDevice _device;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
    Window *_window;
};
