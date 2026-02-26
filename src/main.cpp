#include <iostream>
#include <unordered_map>
#include "requestAdapter.hpp"
#include "requestDevice.hpp"
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application.hpp"

// void onDeviceLost(WGPUDevice const *device, WGPUDeviceLostReason reason, WGPUStringView message, void *, void *)
// {
//   if (reason != WGPUDeviceLostReason::WGPUDeviceLostReason_Destroyed)
//     std::cout << "WGPU device lost: " << message << std::endl;
// }

// void onDeviceUncapturedError(WGPUDevice const *device, WGPUErrorType type, WGPUStringView message, void *, void *)
// {
//   std::cout << "WGPU device error: " << message << std::endl;
// }

int main(int, char **)
{
  std::cout << "Hello, WebGPU!!" << std::endl;

  Application application;
  auto window = WindowFactory::createWindow("My Window");
  application.setWindowSurface(window.get());

  window->setOnTick([](double dt)
                    { std::cout << "DeltaTime: " << dt << std::endl; });
  window->run();

  return 0;
}