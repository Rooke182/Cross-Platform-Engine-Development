#pragma once

//#include "Subject.h"
#include "SubjectInstance.h"
#include <iostream>
#ifdef _WIN32
#include <GLFW/glfw3.h>
#endif
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <functional>
#include <vector>
#include <string>

class Input
{
private:

	SubjectInstance m_subjInst;

	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;


public:

	void AddObserver(Observer* observer, std::string string)
	{
		m_subjInst.AddObserver(observer, string);
	}

	void RemoveObserver(Observer* observer)
	{
		m_subjInst.RemoveObserver(observer);
	}

	void ListObservers()
	{
		m_subjInst.ListObservers();
	}
#ifdef _WIN32
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		m_subjInst.NotifyKey(key, action);
	}

	void CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
	{
		m_subjInst.NotifyCursor(xPos, yPos);
	}

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		m_subjInst.NotifyMouseButton(button, action);
	}

	void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		m_subjInst.NotifyScrollWheel(xOffset, yOffset);
	}

	void ControllerInput(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState)
	{
		m_subjInst.NotifyController(cid, state, prevState);
	}
#endif
#ifdef ANDROID
    void AndroidTouch(bool down, float x, float y, int touchIndex)
    {
        m_subjInst.NotifyAndroidTouch(down, x, y, touchIndex);
    }
    void AndroidMotion(float x, float y, int touchIndex)
    {
        m_subjInst.NotifyAndroidMotion(x, y, touchIndex);
    }
    void AndroidGamepad()
    {

    }
    void AndroidJoystick()
    {

    }
    void AndroidGryoCallback(float x, float y, float z)
    {
        m_subjInst.NotifyAndroidGryo(x, y, z);
    }
#endif
};
