#pragma once
#include "Observer.h"
#include "BaseComponent.h"
#include <functional>
class InputObserverComponent : public BaseComponent , public Observer
{
public:
    friend class Debugger;
    friend class DebuggerFunctions;
    friend class DebuggerMenus;

	std::function<void(int key, int action)> m_keyCallback;
    std::function<void(double xPos, double yPos)> m_cursorCallback;
    std::function<void(int mouseButton, int action)> m_mouseClickCallback;
    std::function<void(double xOffset, double yOffset)> m_scrollWheelCallback;
    std::function<void(float x, float y, float z)> m_androidGryoCallback;
    std::function<void(bool down, float x, float y, int touchIndex)> m_androidTouchCallback;
    std::function<void(float x, float y, int touchIndex)> m_androidMotionCallback;
#ifdef _WIN32
    std::function<void(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState)> m_controllerCallback;
#endif

    virtual void KeyNotify(int key, int action) override { if (m_keyCallback) m_keyCallback(key, action); }
    virtual void CursorNotify(double xPos, double yPos) override { if (m_cursorCallback) m_cursorCallback(xPos, yPos); }
    virtual void MouseButtonNotify(int mouseButton, int action) override { if (m_mouseClickCallback) m_mouseClickCallback(mouseButton, action); }
    virtual void ScrollWheelNotify(double xOffset, double yOffset) override { if (m_scrollWheelCallback) m_scrollWheelCallback(xOffset, yOffset); }
#ifdef ANDROID
    virtual void AndroidGryoNotify(float x, float y, float z) override { if (m_androidGryoCallback) m_androidGryoCallback(x, y, z); }
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) override { if (m_androidTouchCallback) m_androidTouchCallback(down, x, y, touchIndex); }
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) override { if (m_androidMotionCallback) m_androidMotionCallback(x, y, touchIndex); }
#endif
#ifdef _WIN32
    virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) override { if (m_controllerCallback) m_controllerCallback(cid, state, prevState); };
#endif
    virtual void Init() override;
    virtual void Cleanup() override;

    CreateComponentID(InputObserverComponent);

    std::string GetName() override { return "InputObserverComponent"; };
};

