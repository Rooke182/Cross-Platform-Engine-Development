#include "PlayerControllerComponent.h"
#include "Input.h"
#include "Scene.h"
#include "AnimatorComponent.h"
#include "VirtualButtonComponent.h"
#include "AudioManager.h"


void PlayerControllerComponent::Init()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->AddObserver(this, "PlayerControllerComponent");

	m_virtualButtons = m_parent->GetScene()->GetComponentsOfType<VirtualButtonComponent>();
	AudioManager::GetInstance().LoadAudioClip("assets/sounds/punch.wav", "assets/sounds/punch.wav");
}

void PlayerControllerComponent::Cleanup()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->RemoveObserver(this);
}

void PlayerControllerComponent::Update(Time time)
{

	float xMovement = GetXVector();
	float yMovement = GetYVector();
	bool interact = GetInteractJustPressed();
	m_parent->GetTransform()->m_position.x += xMovement * m_movementSpeed * time.m_deltaTime * 0.001f;
	m_parent->GetTransform()->m_position.y += yMovement * m_movementSpeed * time.m_deltaTime * 0.001f;

	auto animator = m_parent->GetComponentOfType<AnimatorComponent>();
	if (animator != nullptr)
	{
		if (animator->GetCurrentAnimationName() != "attack" || animator->IsAnimationFinished())
		{
			if (xMovement != 0.0f || yMovement != 0.0f)
				animator->PlayAnimation("walk");
			else
				animator->PlayAnimation("idle");
		}

		if (interact)
		{
			animator->PlayAnimation("attack", false);
			AudioManager::GetInstance().PlayAudioClip("assets/sounds/punch.wav");
		}
	}


	//if (m_joystick != nullptr)
	//{
	//	std::cout << std::to_string(m_joystick->GetJoystickVector().x) << ":" << std::to_string(m_joystick->GetJoystickVector().y) << std::endl;
	//}
	

	BaseComponent::Update(time);
}

float PlayerControllerComponent::GetXVector()
{
	float value = 0.0f;
#ifdef _WIN32
	for (auto& vec : m_XKeyMovement)
		if (vec.m_isDown)
			value += vec.m_dir;
	for (auto& vec : m_XGamepadMovement)
		if (vec.m_isDown)
			value += vec.m_dir;
	for (auto& vec : m_XGamepadAxisMovement)
			value += vec.m_dir;
#endif

	for (auto& button : m_virtualButtons)
	{
		if (button->GetParent()->GetGuid() == m_joystickVirtualButton)
			value -= button->GetJoystickVector().x;
	}		

	return std::clamp(value, -1.0f, 1.0f);
}

float PlayerControllerComponent::GetYVector()
{
	float value = 0.0f;
#ifdef _WIN32
	for (auto& vec : m_YKeyMovement)
		if (vec.m_isDown)
			value += vec.m_dir;
	for (auto& vec : m_YGamepadMovement)
		if (vec.m_isDown)
			value += vec.m_dir;
	for (auto& vec : m_YGamepadAxisMovement)
		value += vec.m_dir;
#endif

	for (auto& button : m_virtualButtons)
	{
		if (button->GetParent()->GetGuid() == m_joystickVirtualButton)
			value -= button->GetJoystickVector().y;
	}

	return std::clamp(value, -1.0f, 1.0f);
}

bool PlayerControllerComponent::GetInteractJustPressed()
{
	bool pressed = false;
#ifdef _WIN32
	for (auto& vec : m_interactKey)
	{
		if (vec.m_isDown)
			pressed = true;
	}

	for (auto& vec : m_interactGamepad)
	{
		if (vec.m_isDown)
			pressed = true;
	}
#endif

	for (auto& button : m_virtualButtons)
	{
		if (button->GetParent()->GetGuid() == m_interactVirtualButton)
			if (button->CheckIsDown())
				pressed = true;
	}

	if (!m_wasPressed && pressed)
		return m_wasPressed = true; //now just pressed
	if (!pressed)
		m_wasPressed = false; //reset after releasing

	return false;
}

void PlayerControllerComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Player Controller Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		auto newjoystick = DebuggerFunctions::GameObjectDropDownPicker("Joystick:", "##joystick", m_joystickVirtualButton.ToString());
		if (newjoystick != "")
			m_joystickVirtualButton = Guid(newjoystick);

		auto newbutton = DebuggerFunctions::GameObjectDropDownPicker("Interact Button:", "##Button", m_interactVirtualButton.ToString());
		if (newbutton != "")
			m_interactVirtualButton = Guid(newbutton);

		RemoveComponentMenu(ID);
	}
#endif
}

void PlayerControllerComponent::KeyNotify(int key, int action)
{
#ifdef _WIN32
	if (action == GLFW_REPEAT)
		return; //ignore
	for (auto& vec : m_XKeyMovement)
	{
		if (key == vec.m_bind)
			vec.m_isDown = action == GLFW_PRESS;
	}
	for (auto& vec : m_YKeyMovement)
	{
		if (key == vec.m_bind)
			vec.m_isDown = action == GLFW_PRESS;
	}
	for (auto& vec : m_interactKey)
	{
		if (key == vec.m_bind)
			vec.m_isDown = action == GLFW_PRESS;
	}
#endif
}

//added by Ben
void PlayerControllerComponent::CursorNotify(double xPos, double yPos)
{
#ifdef _WIN32
	m_cursorXPos = xPos;
	m_cursorYPos = yPos;
#endif
}

//added by Ben
void PlayerControllerComponent::MouseButtonNotify(int mouseButton, int action)
{
#ifdef _WIN32
	if (m_clickToMove)
	{
		if (action == GLFW_REPEAT)
			return;

		for (auto& vec : m_mouseButtons)
		{
			if (mouseButton == vec.m_bind)
			{
				vec.m_isDown == action == GLFW_PRESS;

				m_parent->GetTransform()->m_position.x = m_cursorXPos - (Application::m_windowWidth / 2);
				m_parent->GetTransform()->m_position.y = m_cursorYPos - (Application::m_windowHeight / 2);
			}
		}
	}
#endif
}

#ifdef _WIN32
void PlayerControllerComponent::ControllerNotify(int cid, GLFWgamepadstate* state, GLFWgamepadstate* prevState)
{
	
	for (auto& vec : m_XGamepadMovement)
	{
		vec.m_isDown = state->buttons[vec.m_bind] == GLFW_PRESS;
	}
	for (auto& vec : m_YGamepadMovement)
	{
		vec.m_isDown = state->buttons[vec.m_bind] == GLFW_PRESS;
	}
	for (auto& vec : m_interactGamepad)
	{
		vec.m_isDown = state->buttons[vec.m_bind] == GLFW_PRESS;
	}

	for (auto& vec : m_XGamepadAxisMovement)
	{
		vec.m_isDown = true;
		if (state->axes[vec.m_bind] <= m_analogueDeadzone && state->axes[vec.m_bind] >= -m_analogueDeadzone)
			vec.m_dir = 0;
		else
			vec.m_dir = state->axes[vec.m_bind];
	}
	for (auto& vec : m_YGamepadAxisMovement)
	{
		vec.m_isDown = true;
		if (state->axes[vec.m_bind] <= m_analogueDeadzone && state->axes[vec.m_bind] >= -m_analogueDeadzone)
			vec.m_dir = 0;
		else
			vec.m_dir = state->axes[vec.m_bind];
	}
}
#endif