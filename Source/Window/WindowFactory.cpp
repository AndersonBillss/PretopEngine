#include "WindowFactory.hpp"
#ifdef __EMSCRIPTEN__
#include "WEB_CanvasWindow.hpp"
#else // not __EMSCRIPTEN
#include "NATIVE_GlfwWindow.hpp"
#endif // not __EMSCRIPTEN

namespace Pretop::Window
{
        namespace WindowFactory
        {
                std::unique_ptr<Window> CreateWindow(int width, int height, std::string title)
                {
#ifdef __EMSCRIPTEN__
                        return std::make_unique<CanvasWindow>(width, height, title);
#else  // not __EMSCRIPTEN
                        return std::make_unique<GlfwWindow>(width, height, title);
#endif // not __EMSCRIPTEN
                }

                std::unique_ptr<Window> CreateWindow(std::string title)
                {
#ifdef __EMSCRIPTEN__
                        return std::make_unique<CanvasWindow>(title);
#else  // not __EMSCRIPTEN
                        return std::make_unique<GlfwWindow>(title);
#endif // not __EMSCRIPTEN
                }
        }
} // namespace Pretop::Window
