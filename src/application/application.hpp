#include "../window/window.hpp"
#include <webgpu/webgpu.h>
#include "appAdapter.hpp"
#include "appDevice.hpp"

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
    void createQueue();

    bool _logQueueCommands;
    WGPUInstance _instance;
    AppAdapter _adapter;
    AppDevice _device;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
    Window *_window;
};
