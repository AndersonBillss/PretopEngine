#include <memory>
#include <webgpu/webgpu.h>
#include "../window/window.hpp"
#include "appAdapter.hpp"
#include "appDevice.hpp"
#include "appInstance.hpp"
#include <functional>

class Application
{
public:
    using TickCallback = std::function<WGPUCommandBuffer(
        double dt,
        WGPUTextureView targetView,
        AppDevice device)>;

    Application();

    void run(TickCallback cb);

    void setWindow(std::unique_ptr<Window> win);
    void logQueueCommands();
    WGPUTextureView getNextSurfaceTextureView();
    WGPUTextureFormat windowFormat;

    Application *inspectInstance();
    Application *inspectDevice();
    Application *inspectAdapter();
    Application *inspectQueue();

private:
    void createQueue();

    bool _logQueueCommands;
    AppInstance _instance;
    AppAdapter _adapter;
    AppDevice _device;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
    std::unique_ptr<Window> _window;
};
