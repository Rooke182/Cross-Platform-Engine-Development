#pragma once

//for gamepad stuff
#ifdef _WIN32
#define GLFW_INCLUDE_NONE //will probably break something, sorry if it does
#include <GLFW/glfw3.h>
#endif

class Input;

class Observer
{
public:

	//controller deadzone variables
	float m_analogueDeadzone = 0.1f;
	float m_triggerDeadzone = -1.0f;

	virtual ~Observer() {}; 

	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;

	virtual void KeyNotify(int key, int action) = 0;
	virtual void CursorNotify(double xPos, double yPos) = 0;
	virtual void MouseButtonNotify(int mouseButton, int action) = 0;
	virtual void ScrollWheelNotify(double xOffset, double yOffset) = 0;
#ifdef ANDROID
    virtual void AndroidGryoNotify(float x, float y, float z) = 0;
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) = 0;
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) = 0;
#endif
#ifdef _WIN32
	virtual void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) = 0;
#endif
};

