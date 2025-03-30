#pragma once

#include "Observer.h"
#include <iostream>
#include <vector>
#include <string>

class Input;

class SubjectInstance
{
private:

	std::vector<Observer*> m_observers;

public:
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;

	void AddObserver(Observer* observer, std::string string)
	{
		std::cout << "number of observers: " << m_observers.size() << std::endl;
		m_observers.push_back(observer);
		std::cout << "added observer " << observer << "(" << string << ") of Input.h" << std::endl;
		std::cout << "number of observers: " << m_observers.size() << std::endl;
	}

	//TODO - add code to remove observer from observer list
	//Matty - This has caused me anguish beyond belief
	// one - try to name functions diffrently dont have a function call a function with the same name
	// two - this is really important re-impliment this asap i dont know if this is what you want me to do
	// three - im stupid and should of checked if it even existed which is kinda my fault
	// four - after note i just checked you did add this on your own branch... i feel even more stupid
	void RemoveObserver(Observer* observer)
	{
		std::cout << "number of observers: " << m_observers.size() << std::endl;
		m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
		std::cout << "removed observer " << observer << " of Input.h" << std::endl;
		std::cout << "number of observers: " << m_observers.size() << std::endl;
	}

	void ListObservers()
	{
		for (Observer* observer : m_observers)
		{
			std::cout << observer << std::endl;
		}
	}

	void NotifyKey(int key, int action) 
	{
		for (Observer* observer : m_observers)
		{
			if (observer != nullptr)
				observer->KeyNotify(key, action);
		}
	}

	void NotifyCursor(double xPos, double yPos) 
	{
		for (Observer* observer : m_observers) 
		{
			if (observer != nullptr)
				observer->CursorNotify(xPos, yPos);
		}
	}

	void NotifyMouseButton(int button, int action) 
	{
		for (Observer* observer : m_observers)
		{
			if (observer != nullptr)
				observer->MouseButtonNotify(button, action);
		}
	}

	void NotifyScrollWheel(double xOffset, double yOffset) 
	{
		for (Observer* observer : m_observers)
		{
			if (observer != nullptr)
				observer->ScrollWheelNotify(xOffset, yOffset);
		}
	}
#ifdef ANDROID
    void NotifyAndroidGryo(float x, float y, float z)
    {
        for (Observer* observer : m_observers)
        {
			if (observer != nullptr)
				observer->AndroidGryoNotify(x, y, z);
        }
    }
    void NotifyAndroidTouch(bool down, float x, float y, int touchIndex)
    {
        for (Observer* observer : m_observers)
        {
			if (observer != nullptr)
				observer->AndroidTouchNotify(down, x, y, touchIndex);
        }
    }
    void NotifyAndroidMotion(float x, float y, int touchIndex)
    {
        for (Observer* observer : m_observers)
        {
			if (observer != nullptr)
				observer->AndroidMotionNotify(x, y, touchIndex);
        }
    }

#endif

#ifdef _WIN32
	void NotifyController(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState)
	{
		for (Observer* observer : m_observers)
		{
			if (observer != nullptr)
				observer->ControllerNotify(cid, state, prevState);
		}
	}
#endif
};