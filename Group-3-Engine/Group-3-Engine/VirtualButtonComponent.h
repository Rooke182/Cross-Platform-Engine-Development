#pragma once
#include "BaseComponent.h"
#include "Observer.h"
class VirtualButtonComponent : public BaseComponent, public Observer
{
private: 
    glm::vec3 m_joystickVector = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_defaultPosition; //store for joystick moving
    float m_mouseX = 0;
    float m_mouseY = 0;
    bool m_isDown = false;
#ifdef ANDROID
    int m_touchIndex = -1;
#endif

    bool CheckOverlap();

    inline void Press();
    inline void Release();
    inline void UpdateJoystick();

public: 
    bool m_isJoystick = false;
    bool CheckIsDown() { return m_isDown; }
    virtual void KeyNotify(int key, int action) override {};
    virtual void CursorNotify(double xPos, double yPos) override { m_mouseX = xPos; m_mouseY = yPos; UpdateJoystick(); }
    virtual void MouseButtonNotify(int mouseButton, int action) override;
    virtual void ScrollWheelNotify(double xOffset, double yOffset) override {}
#ifdef _WIN32
    virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) override {};
#endif
#ifdef ANDROID
    virtual void AndroidGryoNotify(float x, float y, float z) override {}
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex);
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) 
    { 
        if (m_isDown && touchIndex == m_touchIndex)
        {
            //update touch position
            m_mouseX = x; 
            m_mouseY = y;
            UpdateJoystick();
        }
    }
#endif
    virtual void Init() override;
    virtual void Cleanup() override;
    virtual void Update(Time time) override;

    CreateComponentID(VirtualButtonComponent);

    glm::vec3 GetJoystickVector() { return m_joystickVector; }
    std::string GetName() override { return "VirtualButtonComponent"; };
};

