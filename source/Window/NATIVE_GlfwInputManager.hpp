#include "InputManager.hpp"
#include <GLFW/glfw3.h>

namespace Pretop::Window
{

    class GlfwInputManager : InputManager
    {
    public:
        GlfwInputManager() {};

        bool IsDown(Key key) override;
        bool IsPressed(Key key) override;
        bool IsReleased(Key key) override;

        void Pump() override;

    private:
        GLFWwindow *_window;

        std::unordered_set<Key> _heldDownThisFrame;
        std::unordered_set<Key> _pressedThisFrame;
        std::unordered_set<Key> _releasedThisFrame;
    };
} // namespace name
