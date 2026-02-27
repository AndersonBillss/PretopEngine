#include "createInstance.hpp"
#include <iostream>

WGPUInstance createInstance()
{
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;
    WGPUInstanceFeatureName features[] = {WGPUInstanceFeatureName_TimedWaitAny};
    desc.requiredFeatureCount = 1;
    desc.requiredFeatures = features;
    auto instance = wgpuCreateInstance(&desc);

    if (!instance)
    {
        std::cerr << "Could not initialize WebGPU!" << std::endl;
        exit(1);
    }
    return instance;
}