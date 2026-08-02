#include "WindowFactory.hpp"
#include "../Core/Platform.hpp"

#ifdef PRETOP_PLATFORM_WEB
#include "WEB_CanvasWindow.hpp"
#else // not PRETOP_PLATFORM_WEB
#include "NATIVE_GlfwWindow.hpp"
#endif // not PRETOP_PLATFORM_WEB

namespace Pretop::Window
{
        namespace WindowFactory
        {
                std::unique_ptr<Window> CreateWindow(int width, int height, std::string title)
                {
#ifdef PRETOP_PLATFORM_WEB
                        return std::make_unique<CanvasWindow>(width, height, title);
#else  // not PRETOP_PLATFORM_WEB
                        return std::make_unique<GlfwWindow>(width, height, title);
#endif // not PRETOP_PLATFORM_WEB
                }

                std::unique_ptr<Window> CreateWindow(std::string title)
                {
#ifdef PRETOP_PLATFORM_WEB
                        return std::make_unique<CanvasWindow>(title);
#else  // not PRETOP_PLATFORM_WEB
                        return std::make_unique<GlfwWindow>(title);
#endif // not PRETOP_PLATFORM_WEB
                }
        }
} // namespace Pretop::Window
