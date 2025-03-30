#pragma once
#include "Defines.h"
#ifdef _WIN32
#include <GLFW/glfw3.h>
#endif
#ifdef ANDROID
#include <android/native_window.h>
#include <android/input.h>
#endif
#include <chrono>
#include "Debug.h"
#include "Debugger.h"
#include "Scene.h"
#include "AudioManager.h"
#include "UIButton.h"
#include "UITestScene.h"

//Input stuff
#include "Input.h"
#include "Observer.h"


class Application : public Observer
{
public:
    #ifdef _WIN32
    GLFWwindow* m_window;
    #endif
    Scene* m_scene;
    UITestScene* m_uiScene;

    const std::string m_defaultScene = "assets/scenes/scene.json";


    static int m_windowWidth;
    static int m_windowHeight;
    static bool isMouse0Down;
    static bool isMouse1Down;
    static bool isMouse2Down;

#ifdef ANDROID
    ANativeWindow* m_window;
#endif

    //input stuff
    void InitInput();
    static void InitGamepad();
    static void InitStates();
    Input m_input;
    static double m_mouseXpos, m_mouseYpos;
    static int tilemapWidth, tilemapHeight;
#ifdef _WIN32

    //gamepad stuff
    inline static std::vector<GLFWgamepadstate> m_gamepadStateVec;
    inline static std::vector<GLFWgamepadstate> m_gamepadStatePreviousVec;
#endif
    inline static int m_numOfGamepads; //changed to inline static in order to test hotplugging
    inline static int m_currentMaxGamepads;

    void Init()
    {
#ifdef _WIN32
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Window", NULL, NULL);
        if (m_window == NULL)
        {
#ifdef DEBUGGER
            Debugger::AddError("Error", "Failed to create window", true);
#endif
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(m_window);
        glfwSetFramebufferSizeCallback(m_window, OnResize);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
#ifdef DEBUGGER
            Debugger::AddError("Error", "Failed to initialize GLAD", true);
#endif
            return;
        }
#endif
#ifdef OPENGL
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        glEnable(GL_DEPTH_TEST);
#endif
        m_scene = Scene::LoadFromJson(m_defaultScene);
        m_uiScene = new UITestScene();
        #ifdef DEBUGGER
        Debugger::Init(m_window, m_scene); 
        Debugger::m_sceneNameLoaded = m_defaultScene;
        Debugger::m_sceneNameToLoad = m_defaultScene;
        Debugger::m_input = &m_input;
        Debugger::m_app = this;
        #endif

        m_scene->SetInput(&m_input);
        m_scene->Init();
        m_uiScene->Init();
        
        //itialises all input related code
        InitInput();
    }

    void Loop()
    {

#if defined(_WIN32)
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point oldTime = std::chrono::steady_clock::now();
        Time time;
        while (!glfwWindowShouldClose(m_window))
        #elif defined(ANDROID)
        //will be looped automatically
        static std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        static std::chrono::steady_clock::time_point oldTime = std::chrono::steady_clock::now();
        static Time time; //make static so its not reset
#else
        Debug::Error("No Graphics API set!", true);
        return;
#endif
        {
            auto currentTime = std::chrono::steady_clock::now();
            time.m_deltaTime = std::chrono::duration<float, std::milli>(currentTime - oldTime).count();
            time.m_timeSinceStart = std::chrono::duration<float, std::milli>(currentTime - startTime).count();
            oldTime = currentTime;

            #ifdef _WIN32
            glfwSwapBuffers(m_window);
            glfwPollEvents();
#endif
#ifdef DEBUGGER
            if (!Debugger::m_gamePaused)
#endif
            m_scene->Update(time);
            m_uiScene->Update(time);

#ifdef _WIN32
            //gamepad input
            if (m_numOfGamepads > 0) //only runs if at least one gamepad is detected
            {
                for (int i = 0; i < m_currentMaxGamepads; i++)
                {
                    if (glfwGetGamepadName(i) != NULL && &m_gamepadStateVec[i] != nullptr)
                    {
                        if (glfwGetGamepadState(i, &m_gamepadStateVec[i]))
                        {
                            m_input.ControllerInput(i, &m_gamepadStateVec[i], &m_gamepadStatePreviousVec[i]); //calls Input.h functions, acts like funcs on lines 427-451
                            m_gamepadStatePreviousVec[i] = m_gamepadStateVec[i]; //resolves gamepad state after all observers react to gamepad input
                        }
                    }
                }
            }
#endif
#ifdef OPENGL
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif


            m_scene->Render();

#ifdef DEBUGGER
            if(!Debugger::m_uiPaused)
#endif
            m_uiScene->Render();


#ifdef DEBUGGER
            Debugger::Render(time);
#endif

            m_scene->EndFrame();
            AudioManager::GetInstance().Update();
            m_uiScene->EndFrame();
        }
    }

	void Cleanup()
	{
        //m_scene->WriteToJson("assets/scenes/scene.json");
        
        #ifdef DEBUGGER
        Debugger::Cleanup();
#endif

        m_scene->Cleanup();
        m_uiScene->Cleanup();

#ifdef _WIN32
        glfwTerminate();
#endif
    }

#ifdef _WIN32
    static void OnResize(GLFWwindow* window, int width, int height)
    {
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
    }
#endif


#ifdef ANDROID
    void OnAndroidInput(const AInputEvent* event, int pointerID)
    {
#ifdef DEBUGGER
        ImGui_ImplAndroid_HandleInputEvent(event);
    #endif
        int32_t event_type = AInputEvent_getType(event);
        switch (event_type)
        {
            case AINPUT_EVENT_TYPE_MOTION:

                int32_t event_action = AMotionEvent_getAction(event);
                int32_t event_pointer_index = (event_action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                event_action &= AMOTION_EVENT_ACTION_MASK;

                switch (event_action) {
                    case AMOTION_EVENT_ACTION_DOWN:
                        m_input.AndroidTouch(true, AMotionEvent_getX(event, event_pointer_index),
                                             AMotionEvent_getY(event, event_pointer_index), pointerID);
                        break;
                    case AMOTION_EVENT_ACTION_UP:
                        m_input.AndroidTouch(false, AMotionEvent_getX(event, event_pointer_index),
                                             AMotionEvent_getY(event, event_pointer_index), pointerID);
                        break;
                    case AMOTION_EVENT_ACTION_MOVE:
                        m_input.AndroidMotion(AMotionEvent_getX(event, event_pointer_index),
                                             AMotionEvent_getY(event, event_pointer_index), pointerID);
                        break;
                }

                break;
        }
    }
    void OnAndroidResize(int width, int height)
    {
        m_windowWidth = width;
        m_windowHeight = height;
        glViewport(0, 0, width, height);
    }
    void OnAndroidGyro(float x, float y, float z)
    {
        m_input.AndroidGryoCallback(x, y, z);
    }
#endif

#ifdef _WIN32
    //input stuff
    //these are called by SubjectInstance.h, these are only here becasue Application.h is set as an observer, if it wasn't an observer, these following 4 functions wouldn't be here
    void KeyNotify(int key, int action) //does this when notified by subject, in this case the input
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
            default:

                break;

            case (GLFW_KEY_ESCAPE):

                ExitApplication();
                break;

            case (GLFW_KEY_ENTER):

                m_input.ListObservers();
                break;

            case (GLFW_KEY_SPACE):
#ifdef INPUTLOG
                std::cout << "num of gamepads detected: " << m_numOfGamepads << std::endl;
                for (size_t i = 0; i < 16; i++)
                {
                    if (glfwGetGamepadName(i) != NULL)
                    {
                        std::cout << "Gamepad " << i << " " << glfwGetGamepadName(i) << std::endl;
                    }
                }
#endif

                break;

            case (GLFW_KEY_1):

                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //keeps curson on screen
                std::cout << "Cursor mode: normal\n";
                break;

            case (GLFW_KEY_2):

                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //hides cursor when over game window, or is supposed to, but doesnt atm, idk why
                std::cout << "Cursor mode: hidden\n";
                break;

            case (GLFW_KEY_3):

                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                std::cout << "Cursor mode: captured\n";
                break;

            case (GLFW_KEY_4):

                glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //captures cursor in game window
                std::cout << "Cursor mode: disabled\n";
                break;

            case (GLFW_KEY_D):

                if (m_mouseXpos > 640 && m_mouseXpos < 1280)
                {
                    if (m_mouseYpos > 360 && m_mouseYpos < 720)
                    {
                        std::cout << "keyboard D press in region\n";
                    }
                }
                break;

            case (GLFW_KEY_K):

                //testing observer removal
                m_input.RemoveObserver(this);
                break;
            
            case (GLFW_KEY_L):

                //testing observer removal
                m_input.RemoveObserver(m_scene);
                break;
            }

#ifdef INPUTLOG
            std::cout << "Application.h Observed keyboard " << key << "(" << (char)key << ")" << " pressed" << std::endl;
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif

            key = true;
        }
        else if (action == GLFW_RELEASE)
        {
            switch (key)
            {
            default:

                break;
            }

#ifdef INPUTLOG
            std::cout << "Application.h Observed keyboard " << key << "(" << (char)key << ")" << " released" << std::endl;
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif

            key = false;
        }
        else if (action == GLFW_REPEAT)
        {
            switch (key)
            {
            default:

                break;
            }

#ifdef INPUTLOG
            std::cout << "Application.h Observed keyboard " << key << "(" << (char)key << ")" << " held" << std::endl;
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif
        }
    }
    void CursorNotify(double xPos, double yPos)
    {
        m_mouseXpos = xPos;
        m_mouseYpos = yPos;

        #ifdef INPUTLOG
        //std::cout << "Application.h observed cursor x " << xPos << " y " << yPos << std::endl;
        #endif
    }
    void MouseButtonNotify(int mouseButton, int action)
    {
        if (action == GLFW_PRESS)
        {
#ifdef INPUTLOG
            std::cout << "Application.h observed mouse button " << mouseButton << " pressed\n";
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif



            if (mouseButton == 0)
            {
				isMouse0Down = true;
            }
            if (mouseButton == 2)
            {
                isMouse1Down = true;
            }
            if (mouseButton == 2)
            {
                isMouse2Down = true;
            }

            mouseButton = true;
        }
        else if (action == GLFW_RELEASE)
        {
#ifdef INPUTLOG
            std::cout << "Application.h observed mouse button " << mouseButton << " released\n";
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
            #endif

            if (mouseButton == 0)
            {
                isMouse0Down = false;
            }
            if (mouseButton == 2)
            {
                isMouse1Down = false;
            }
            if (mouseButton == 2)
            {
                isMouse2Down = false;
            }
            
            mouseButton = false;
        }
        else if (action == GLFW_REPEAT)
        {
#ifdef INPUTLOG
            std::cout << "Application.h observed mouse button " << mouseButton << " held\n";
            std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif
        }
    }
    void ScrollWheelNotify(double xOffset, double yOffset)
    {
#ifdef INPUTLOG
        std::cout << "Application.h observed scroll x" << xOffset << " y " << yOffset << std::endl;
        std::cout << "at x " << m_mouseXpos << " y " << m_mouseYpos << std::endl;
#endif
    }

    //used to test input
    void ExitApplication()
    {
        std::cout << "Window close\n";
        glfwSetWindowShouldClose(m_window, GL_TRUE); //closes window if ESC is pressed
    }

    

    //these all call Input.h functions
    //GetWindowUserPointer and pointing to input class' functions allow for non-static calls
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        input->KeyCallback(window, key, scancode, action, mods);
#ifdef DEBUGGER
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
    }
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        input->CursorPosCallback(window, xpos, ypos);
#ifdef DEBUGGER
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif
    }
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        input->MouseButtonCallback(window, button, action, mods);
#ifdef DEBUGGER
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif
    }
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(window));
        input->ScrollCallback(window, xoffset, yoffset);
#ifdef DEBUGGER
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
#endif
    }

    //gamepad stuff
    static void ControllerCallback(int cid, int event)
    {
        if (event == GLFW_CONNECTED)
        {
            #ifdef INPUTLOG
            std::cout << "Controller " << cid << " (" << glfwGetJoystickName(cid) << "/" << glfwGetJoystickUserPointer << ")" << " connected\n";
            #endif      

            m_numOfGamepads++; //for testing hotplugging, would error out normally but temporarily changed to inline static, works
            InitGamepad(); //reinits controllers, idk if needed, seems to work without being called again
            InitStates();
        }
        else if (event == GLFW_DISCONNECTED)
        {
            #ifdef INPUTLOG
            std::cout << "Controller disconnected\n";
            #endif      

            m_numOfGamepads--; //for testing hotplugging, would error out normally but temporarily changed to inline static, works
        }
    }
    //this is called by SubjectInstance.h, only here becasue Application.h is set as an observer, if it wasn't an observer, this function wouldn't be here
    void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState) //Aarons good, cool function, called by SubjectInstance.h
    {
        //cid = controller id

        //controller buttons (ABXY, LB, RB, DPAD, START, SELECT, GUIDE)
        for (size_t i = 0; i < GLFW_GAMEPAD_BUTTON_LAST + 1; i++)
        {
            if (state->buttons[i] != prevState->buttons[i])
            {
                if (state->buttons[i] == GLFW_PRESS)
                {
#ifdef INPUTLOG
                    std::cout << "Application.h observed Controller " << cid;
#endif
                    switch (i)
                    {
                    case (GLFW_GAMEPAD_BUTTON_A):

#ifdef INPUTLOG
                        std::cout << " button A pressed on " << glfwGetGamepadName(cid) << std::endl;
                        std::cout << "num of gamepads detected: " << m_numOfGamepads << std::endl;
#endif
                        break;

                    case (GLFW_GAMEPAD_BUTTON_B):

#ifdef INPUTLOG
                        std::cout << " button B pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_X):

#ifdef INPUTLOG
                        std::cout << " button X pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_Y):

#ifdef INPUTLOG
                        std::cout << " button Y pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_THUMB):

#ifdef INPUTLOG
                        std::cout << " button L3 pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_THUMB):

#ifdef INPUTLOG
                        std::cout << " button R3 pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_BUMPER):

#ifdef INPUTLOG
                        std::cout << " button LB pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER):

#ifdef INPUTLOG
                        std::cout << " button RB pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_START):

#ifdef INPUTLOG
                        std::cout << " button START pressed\n";
#endif

                        ExitApplication();

                        break;

                    case (GLFW_GAMEPAD_BUTTON_BACK):

#ifdef INPUTLOG
                        std::cout << " button BACK/SELECT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_UP):

#ifdef INPUTLOG
                        std::cout << " button DPAD UP pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_DOWN):

#ifdef INPUTLOG
                        std::cout << " button DPAD DOWN pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_LEFT):

#ifdef INPUTLOG
                        std::cout << " button DPAD LEFT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_RIGHT):

#ifdef INPUTLOG
                        std::cout << " button DPAD RIGHT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_GUIDE):

#ifdef INPUTLOG
                        std::cout << " button GUIDE pressed\n";
#endif

                        break;

                    default:

                        break;
                    }
                }
                if (state->buttons[i] == GLFW_RELEASE)
                {
#ifdef INPUTLOG
                    std::cout << "Application.h observed Controller " << cid;
#endif          

                    switch (i)
                    {
                    case (GLFW_GAMEPAD_BUTTON_A):

#ifdef INPUTLOG
                        std::cout << " button A release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_B):

#ifdef INPUTLOG
                        std::cout << " button B release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_X):

#ifdef INPUTLOG
                        std::cout << " button X release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_Y):

#ifdef INPUTLOG
                        std::cout << " button Y release\n";
#endif 

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_THUMB):

#ifdef INPUTLOG
                        std::cout << " button L3 release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_THUMB):

#ifdef INPUTLOG
                        std::cout << " button R3 release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_BUMPER):

#ifdef INPUTLOG
                        std::cout << " button LB release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER):

#ifdef INPUTLOG
                        std::cout << " button RB release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_START):

#ifdef INPUTLOG
                        std::cout << " button START release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_BACK):

#ifdef INPUTLOG
                        std::cout << " button BACK/SELECT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_UP):

#ifdef INPUTLOG
                        std::cout << " button DPAD UP release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_DOWN):

#ifdef INPUTLOG
                        std::cout << " button DPAD DOWN release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_LEFT):

#ifdef INPUTLOG
                        std::cout << " button DPAD LEFT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_RIGHT):

#ifdef INPUTLOG
                        std::cout << " button DPAD RIGHT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_GUIDE):

#ifdef INPUTLOG
                        std::cout << " button GUIDE release\n";
#endif

                        break;

                    default:

                        break;
                    }
                }
            }
        }

        //controller axes (left stick X/Y, right stick X/Y, left/right triggers)
        for (size_t i = 0; i < GLFW_GAMEPAD_AXIS_LAST + 1; i++)
        {
            if (i <= 3)
            {
                //area between -0.1 and 0.1 on both X and Y axes for both analogue sticks acts as deadzone
                if (state->axes[i] <= m_analogueDeadzone && state->axes[i] >= -m_analogueDeadzone)
                {
#ifdef INPUTLOG
                    std::cout << "Analogue stick deadzone\n";
#endif
                    continue;
                }
            }
            else if (i > 3)
            {
                //both controller triggers being fully de-pressed acts as deadzone
                if (state->axes[i] == m_triggerDeadzone)
                {
#ifdef INPUTLOG
                    std::cout << "Trigger deadzone\n";
#endif
                    continue;
                }
            }

#ifdef INPUTLOG
            std::cout << "Application.h observed ";
#endif
            switch (i)
            {
                case GLFW_GAMEPAD_AXIS_LEFT_X:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " left stick X: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_X] << std::endl;
#endif

                    break;

                case GLFW_GAMEPAD_AXIS_LEFT_Y:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " left stick Y: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y] << std::endl;
#endif

                    break;

                case GLFW_GAMEPAD_AXIS_RIGHT_X:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " right stick X: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_X] << std::endl;
#endif

                    break;

                case GLFW_GAMEPAD_AXIS_RIGHT_Y:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " right stick Y: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] << std::endl;
#endif

                    break;

                case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " L2: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] << std::endl;
#endif

                    break;

                case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:

#ifdef INPUTLOG
                    std::cout << "Controller " << cid << " R2: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] << std::endl;
#endif

                    break;

                default:

                    break;
            }
        }
    }
#endif
#ifdef ANDROID
    virtual void KeyNotify(int key, int action) {}
    virtual void CursorNotify(double xPos, double yPos) {}
    virtual void MouseButtonNotify(int mouseButton, int action) {}
    virtual void ScrollWheelNotify(double xOffset, double yOffset) {}

    virtual void AndroidGryoNotify(float x, float y, float z)
    {
        //std::string test = std::to_string(x) + ":" + std::to_string(y) + ":" + std::to_string(z);
        //LOGV("%s", test.c_str());
    }
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex)
    {
        std::string test = std::to_string(down) + ":" + std::to_string(x) + ":" + std::to_string(y) + ":" + std::to_string(touchIndex);
        LOGV("%s", test.c_str());
    }
    virtual void AndroidMotionNotify(float x, float y, int touchIndex)
    {
        std::string test = "move:" + std::to_string(x) + ":" + std::to_string(y) + ":" + std::to_string(touchIndex);
        LOGV("%s", test.c_str());
    }
#endif
};