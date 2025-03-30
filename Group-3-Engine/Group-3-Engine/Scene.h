#pragma once
#include "GameObject.h"
#include "ComponentBuffer.h"
#include "ComponentPtr.h"
#include "Debugger.h"
#include "Observer.h"
class Input;
class Resources;

#ifdef ANDROID //sealed doesnt seem to work on android sorry
class Scene : public Observer
#else
class Scene sealed : public Observer
#endif
{
protected:
	Input* m_input;
	friend class Debugger;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;
private:
	GameObject* m_rootObject;
	ComponentBuffer<16 * 1024> m_components;
	friend class Debugger;

	static Scene* m_currentScene;

public:
	Scene();
	virtual void Init();
	virtual void Update(Time time);
	virtual void Render();
	virtual void Cleanup();
	virtual void EndFrame();
	virtual void SetInput(Input* input);
	virtual Input* GetInput();

	int AddGameObject(GameObject&& gameObject);

	template <typename T, typename... Args>
	ComponentPtr<T> MakeComponent(Args... args);

	template<typename T>
	ComponentPtr<T> GetComponentOfType();
	std::vector<GameObject> m_gameObjects;

	template<typename T>
	std::vector<ComponentPtr<T>> GetComponentsOfType();

	void IterateComponents(std::function<void(void*)> func);
	void IterateGameObjects(std::function<void(GameObject&)> func);

	nlohmann::json SerialiseScene();
	void DeserialiseScene(const nlohmann::json& json);

	template<typename T>
	ComponentPtr<T> GetComponentByGuid(const Guid& guid);
	GameObject* GetGameObjectByGuid(const Guid& guid);
    GameObject* GetGameObjectByName(std::string name);
    std::vector<GameObject*> GetGameObjectsByName(std::string name);
	
	static Scene* GetCurrentScene();

	static Scene* LoadFromJson(std::string filePath);
	void WriteToJson(std::string filePath);

	friend class Debugger;
	friend class DebuggerMenus;
	friend class DebuggerFunctions;

#ifdef _WIN32
	void KeyNotify(int key, int action)
	{
#ifdef INPUTLOG
        std::cout << "Scene.h observed keyboard " << key << " " << (char)key << std::endl;
#endif
	};
	void CursorNotify(double xPos, double yPos) {};
	void MouseButtonNotify(int mouseButton, int action) 
    {
#ifdef INPUTLOG
        std::cout << "Scene.h observed mouse button " << mouseButton << std::endl;
#endif
    };
	void ScrollWheelNotify(double xOffset, double yOffset) {};
	void ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState)
	{
        //cid = controller id

        //controller buttons (ABXY, LB, RB, DPAD, START, SELECT, GUIDE)
        for (size_t i = 0; i < GLFW_GAMEPAD_BUTTON_LAST + 1; i++)
        {
            if (state->buttons[i] != prevState->buttons[i])
            {
                if (state->buttons[i] == GLFW_PRESS)
                {
#ifdef _DEBUG
                    std::cout << "Scene.h observed Controller " << cid;
#endif
                    switch (i)
                    {
                    case (GLFW_GAMEPAD_BUTTON_A):

#ifdef _DEBUG
                        std::cout << " button A pressed on " << glfwGetGamepadName(cid) << std::endl;
#endif
                        break;

                    case (GLFW_GAMEPAD_BUTTON_B):

#ifdef _DEBUG
                        std::cout << " button B pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_X):

#ifdef _DEBUG
                        std::cout << " button X pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_Y):

#ifdef _DEBUG
                        std::cout << " button Y pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_THUMB):

#ifdef _DEBUG
                        std::cout << " button L3 pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_THUMB):

#ifdef _DEBUG
                        std::cout << " button R3 pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_BUMPER):

#ifdef _DEBUG
                        std::cout << " button LB pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER):

#ifdef _DEBUG
                        std::cout << " button RB pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_START):

#ifdef _DEBUG
                        std::cout << " button START pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_BACK):

#ifdef _DEBUG
                        std::cout << " button BACK/SELECT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_UP):

#ifdef _DEBUG
                        std::cout << " button DPAD UP pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_DOWN):

#ifdef _DEBUG
                        std::cout << " button DPAD DOWN pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_LEFT):

#ifdef _DEBUG
                        std::cout << " button DPAD LEFT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_RIGHT):

#ifdef _DEBUG
                        std::cout << " button DPAD RIGHT pressed\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_GUIDE):

#ifdef _DEBUG
                        std::cout << " button GUIDE pressed\n";
#endif

                        break;

                    default:

                        break;
                    }
                }
                if (state->buttons[i] == GLFW_RELEASE)
                {
#ifdef _DEBUG
                    std::cout << "Scene.h observed Controller " << cid;
#endif          
                    switch (i)
                    {
                    case (GLFW_GAMEPAD_BUTTON_A):

#ifdef _DEBUG
                        std::cout << " button A release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_B):

#ifdef _DEBUG
                        std::cout << " button B release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_X):

#ifdef _DEBUG
                        std::cout << " button X release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_Y):

#ifdef _DEBUG
                        std::cout << " button Y release\n";
#endif 

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_THUMB):

#ifdef _DEBUG
                        std::cout << " button L3 release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_THUMB):

#ifdef _DEBUG
                        std::cout << " button R3 release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_LEFT_BUMPER):

#ifdef _DEBUG
                        std::cout << " button LB release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER):

#ifdef _DEBUG
                        std::cout << " button RB release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_START):

#ifdef _DEBUG
                        std::cout << " button START release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_BACK):

#ifdef _DEBUG
                        std::cout << " button BACK/SELECT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_UP):

#ifdef _DEBUG
                        std::cout << " button DPAD UP release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_DOWN):

#ifdef _DEBUG
                        std::cout << " button DPAD DOWN release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_LEFT):

#ifdef _DEBUG
                        std::cout << " button DPAD LEFT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_DPAD_RIGHT):

#ifdef _DEBUG
                        std::cout << " button DPAD RIGHT release\n";
#endif

                        break;

                    case (GLFW_GAMEPAD_BUTTON_GUIDE):

#ifdef _DEBUG
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
#ifdef _DEBUG
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
#ifdef _DEBUG
                    std::cout << "Trigger deadzone\n";
#endif
                    continue;
                }
            }

#ifdef _DEBUG
            std::cout << "Scene.h observed ";
#endif
            switch (i)
            {
            case GLFW_GAMEPAD_AXIS_LEFT_X:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " left stick X: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_X] << std::endl;
#endif

                break;

            case GLFW_GAMEPAD_AXIS_LEFT_Y:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " left stick Y: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y] << std::endl;
#endif

                break;

            case GLFW_GAMEPAD_AXIS_RIGHT_X:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " right stick X: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_X] << std::endl;
#endif

                break;

            case GLFW_GAMEPAD_AXIS_RIGHT_Y:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " right stick Y: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] << std::endl;
#endif

                break;

            case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " L2: " << state->axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] << std::endl;
#endif

                break;

            case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:

#ifdef _DEBUG
                std::cout << "Controller " << cid << " R2: " << state->axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] << std::endl;
#endif

                break;

            default:

                break;
            }
        }
	};
#endif
#ifdef ANDROID
    virtual void KeyNotify(int key, int action) override {};
    virtual void CursorNotify(double xPos, double yPos) override {};
    virtual void MouseButtonNotify(int mouseButton, int action) override {};
    virtual void ScrollWheelNotify(double xOffset, double yOffset) override {};
    virtual void AndroidGryoNotify(float x, float y, float z) override {};
    virtual void AndroidTouchNotify(bool down, float x, float y, int touchIndex) override {};
    virtual void AndroidMotionNotify(float x, float y, int touchIndex) override {};
#endif
};

template<typename T, typename... Args>
inline ComponentPtr<T> Scene::MakeComponent(Args... args)
{
	return m_components.CreateComponent<T>(args...);
}

template<typename T>
inline ComponentPtr<T> Scene::GetComponentOfType()
{
	return m_components.GetComponentOfType<T>();
}

template<typename T>
inline std::vector<ComponentPtr<T>> Scene::GetComponentsOfType()
{
	return m_components.GetComponentsOfType<T>();
}

template<typename T>
inline ComponentPtr<T> Scene::GetComponentByGuid(const Guid& guid)
{
	return m_components.GetComponentByGuid<T>(guid);
}
