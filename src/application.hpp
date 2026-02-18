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
};
