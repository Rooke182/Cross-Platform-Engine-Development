#pragma once
#include "BaseComponent.h"
#include "GameObject.h"
#include "Input.h"
#include "Observer.h"
#include "Application.h"
#include "CameraComponent.h"
class VirtualButtonComponent;

struct KeyMovementVector
{
    float m_dir;
    bool m_isDown;
    int m_bind;
};

//added by Ben
struct MouseVector
{
    double m_xPos;
    double m_yPos;
    bool m_isDown;
    int m_bind;
};

class PlayerControllerComponent : public BaseComponent, public Observer
{
private:

#ifdef _WIN32
    //added by Ben
    bool m_clickToMove = false;
    double m_cursorXPos, m_cursorYPos;
    std::vector< MouseVector > m_mouseButtons = { {m_cursorXPos, m_cursorYPos, false, GLFW_MOUSE_BUTTON_1} };
#endif

    //input bindings
    std::vector< KeyMovementVector> m_XKeyMovement = { {-1, false, 'A'}, {1, false, 'D'}, {-1, false, 263}, {1, false, 262} };
    std::vector< KeyMovementVector> m_YKeyMovement = { {-1, false, 'W'}, {1, false, 'S'}, {-1, false, 265}, {1, false, 264} };

    std::vector< KeyMovementVector> m_interactKey = { {-1, false, 'Z'}, {-1, false, 257} };
    
#ifdef _WIN32
    //gamepad
    std::vector< KeyMovementVector> m_XGamepadMovement = { KeyMovementVector{-1, false, GLFW_GAMEPAD_BUTTON_DPAD_LEFT}, KeyMovementVector{1, false, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT} };
    std::vector< KeyMovementVector> m_XGamepadAxisMovement = { KeyMovementVector{0, false, GLFW_GAMEPAD_AXIS_LEFT_X} };

    std::vector< KeyMovementVector> m_YGamepadMovement = { KeyMovementVector{-1, false, GLFW_GAMEPAD_BUTTON_DPAD_UP}, KeyMovementVector{1, false, GLFW_GAMEPAD_BUTTON_DPAD_DOWN} };
    std::vector< KeyMovementVector> m_YGamepadAxisMovement = { KeyMovementVector{0, false, GLFW_GAMEPAD_AXIS_LEFT_Y} };

    std::vector< KeyMovementVector> m_interactGamepad = { KeyMovementVector{-1, false, GLFW_GAMEPAD_BUTTON_A} };
#endif
    //virtual buttons for android
    std::vector< ComponentPtr<VirtualButtonComponent> > m_virtualButtons;

    bool m_wasPressed = false;
public:
    Guid m_joystickVirtualButton = Guid::GetZeroGuid();
    Guid m_interactVirtualButton = Guid::GetZeroGuid();
    float m_movementSpeed = 150.0f;
    virtual void KeyNotify(int key, int action) override;
    virtual void CursorNotify(double xPos, double yPos) override;
    virtual void MouseButtonNotify(int mouseButton, int action) override;
    virtual void ScrollWheelNotify(double xOffset, double yOffset) override {}
#ifdef _WIN32
    virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) override;
#endif
#ifdef ANDROID
    virtual void AndroidGryoNotify(float x, float y, float z) override {}
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) {}
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) {}
#endif

    virtual void Init() override;
    virtual void Cleanup() override;
    virtual void Update(Time time) override;

    float GetXVector();
    float GetYVector();
    bool GetInteractJustPressed();

    CreateComponentID(PlayerControllerComponent);

    void RenderComponentMenu(int ID) override;
};

