#include <iostream>
#include <unordered_map>
#include "requestAdapter.hpp"
#include "requestDevice.hpp"
#include "printStringView.hpp"
#include "window/windowFactory.hpp"
#include "application/application.hpp"

int main(int, char **)
{
  std::cout << "Hello, WebGPU!!" << std::endl;

  auto window = WindowFactory::createWindow("My Window");
  Application application;
  application.logQueueCommands();
  application.setWindow(window.get());
  window->run();

  return 0;
}