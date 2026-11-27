#include "InputManager.hpp"

namespace Pretop::Window
{

    class GlfwInputManager : InputManager
    {
    public:
        bool isDown(Key key) override;
        bool isPressed(Key key) override;
        bool isReleased(Key key) override;

        void pump() override;

    private:
        std::unordered_set<Key> _heldDownThisFrame;
        std::unordered_set<Key> _pressedThisFrame;
        std::unordered_set<Key> _releasedThisFrame;
    };
} // namespace name
