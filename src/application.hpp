#include <webgpu/webgpu.h>

class Application
{
public:
    Application();
    void setSurfaceTexture(WGPUTexture &tex);

private:
    void getInstance();
    void getDevice();
    void getAdapter();
    void getQueue();
    void createEncoder();

    WGPUInstance _instance;
    WGPUAdapter _adapter;
    WGPUDevice _device;
    WGPUQueue _queue;
    WGPUSurface _windowSurface;
};
