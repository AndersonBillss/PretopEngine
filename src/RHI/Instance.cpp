#include "Instance.hpp"
#include <iostream>

AppInstance::AppInstance()
{
    this->wgpuInstance = createInstance();
}

WGPUInstance AppInstance::createInstance()
{
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    desc.requiredFeatureCount = 0;
    desc.requiredFeatures = nullptr;
    auto instance = wgpuCreateInstance(&desc);

    if (!instance)
    {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        exit(1);
    }
    return instance;
}
