#include "Application.h"

int Application::m_windowWidth = 1280;
int Application::m_windowHeight = 720;
double Application::m_mouseXpos, Application::m_mouseYpos;
bool Application::isMouse0Down;
bool Application::isMouse1Down;
bool Application::isMouse2Down;
int Application::tilemapWidth = 200;
int Application::tilemapHeight = 200;

void Application::InitInput()
{
    //input stuff
    m_input.AddObserver(this, "Application.h"); //adds Application.h as an observer to input
    m_input.AddObserver(m_scene, "Scene.h"); //adds Scene.h as an observer, for testing
    m_input.AddObserver(m_uiScene, "UIScene.h"); //adds UIScene.h as an observer, for testing
#ifdef _WIN32
    glfwSetWindowUserPointer(m_window, &m_input); //allows for non-static calls in glfw static funcs

    //sets callbacks
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetCursorPosCallback(m_window, CursorPosCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);
    glfwSetJoystickCallback(ControllerCallback); //used for tracking controller connects/disconnects

    //sets cursor modes, defaults to normal mode
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //keeps cursor on screen

    //controller
    InitGamepad();
    InitStates();
    
    #ifdef _DEBUG
    std::cout << "num of gamepads detected: " << m_numOfGamepads << std::endl;
    #endif
#endif
}

void Application::InitGamepad()
{
#ifdef _WIN32
    std::cout << "InitGamepad\n";

    //controller stuff
    Application::m_numOfGamepads = 0; //changed to inline static in order to test hotplugging
    for (int i = 0; i < 16; i++)
    {
        if (glfwJoystickIsGamepad(i))
        {
#ifdef _DEBUG
            std::cout << "Controller detected: " << glfwGetGamepadName(i) << std::endl;
#endif

            m_numOfGamepads++;
        }
    }
#endif
}

void Application::InitStates()
{
#ifdef _WIN32
    std::cout << "InitStates\n";

    for (size_t i = 0; i < m_numOfGamepads; i++)
    {
        GLFWgamepadstate state;
        m_gamepadStateVec.push_back(state);
        m_gamepadStatePreviousVec.push_back(state);

        if (m_numOfGamepads > m_currentMaxGamepads)
        {
            m_currentMaxGamepads = m_numOfGamepads;
        }
    }
#endif
}
