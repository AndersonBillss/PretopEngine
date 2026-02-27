#include <iostream>
#include <unordered_map>
#include "requestAdapter.hpp"
#include "requestDevice.hpp"
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application.hpp"

int main(int, char **)
{
  std::cout << "Hello, WebGPU!!" << std::endl;

  Application application;
  auto window = WindowFactory::createWindow("My Window");
  application.setWindow(window.get());
  window->run();

  return 0;
}