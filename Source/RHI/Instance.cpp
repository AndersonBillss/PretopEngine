#include "Instance.hpp"
#include <iostream>

AppInstance::AppInstance()
{
    this->WgpuInstance = CreateInstance();
}

WGPUInstance AppInstance::CreateInstance()
{
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    desc.requiredFeatureCount = 0;
    desc.requiredFeatures = nullptr;
    auto instance = wgpuCreateInstance(&desc);

    if (!instance)
    {
        std::cerr << "Could not Initialize WebGPU!" << std::endl;
        exit(1);
    }
    return instance;
}
