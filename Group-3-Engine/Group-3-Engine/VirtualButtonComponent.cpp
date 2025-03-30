#include "VirtualButtonComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "Application.h"


void VirtualButtonComponent::MouseButtonNotify(int mouseButton, int action)
{
#ifdef _WIN32
	if (mouseButton == 0)
	{
		//std::cout << "check" << std::endl;
		if (m_isDown)
		{
			if (action == GLFW_RELEASE)
			{
				m_isDown = false;
				Release();
			}
		}
		else
		{
			if (action == GLFW_PRESS && CheckOverlap())
			{
				m_isDown = true;
				Press();
			}
		}
	}
#endif
}

#ifdef ANDROID
void VirtualButtonComponent::AndroidTouchNotify(bool down, float x, float y, int touchIndex)
{
	m_mouseX = x; //update for overlap checks
	m_mouseY = y;
	if (m_isDown)
	{
		if (!down && touchIndex == m_touchIndex) //only release if the same touch
		{
			m_isDown = false;
			Release();
		}
	}
	else
	{
		if (down && CheckOverlap())
		{
			m_isDown = true;
			m_touchIndex = touchIndex;
			Press();
		}
	}
}
#endif

void VirtualButtonComponent::Init()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->AddObserver(this, "InputObserverComponent");

	m_parent->GetRenderer()->SetColor(glm::vec4{ 1.0, 1.0, 1.0, 0.5 });
}

void VirtualButtonComponent::Cleanup()
{
	auto input = m_parent->GetScene()->GetInput();
	if (input != nullptr)
		input->RemoveObserver(this);
}

void VirtualButtonComponent::Update(Time time)
{

}

bool VirtualButtonComponent::CheckOverlap()
{
	auto camera = m_parent->GetRenderer()->GetCamera();
	auto transform = m_parent->GetTransform();
	auto aspectScale = glm::vec3((float)Application::m_windowWidth / camera->m_width, (float)Application::m_windowHeight / camera->m_height, 1.0f);
	auto aspect = glm::scale(glm::mat4(1.0), aspectScale);

	auto mousePos = glm::vec4(m_mouseX, m_mouseY, 1.0, 1.0) / aspect;
	mousePos -= glm::vec4{camera->m_width * 0.5f, camera->m_height * 0.5f, 0.0f, 0.0f};


	auto position = transform->m_position;
	auto scale = transform->m_scale * glm::vec3{ transform->m_width, transform->m_height, 1.0 };
	
	return (mousePos.x >= position.x && mousePos.x < position.x+scale.x) && (mousePos.y >= position.y && mousePos.y < position.y + scale.y);
}

inline void VirtualButtonComponent::Press()
{
	//std::cout << "press" << std::endl;
	m_defaultPosition = m_parent->GetTransform()->m_position;
	UpdateJoystick();
	m_parent->GetRenderer()->SetColor(glm::vec4{ 0.5, 0.5, 0.5, 0.5 }); //just to test
}

inline void VirtualButtonComponent::Release()
{
	//std::cout << "release" << std::endl;
	m_joystickVector = { 0.0f, 0.0f , 0.0f};
	m_parent->GetTransform()->m_position = m_defaultPosition;
	m_parent->GetRenderer()->SetColor(glm::vec4{ 1.0, 1.0, 1.0, 0.5 });
}

inline void VirtualButtonComponent::UpdateJoystick()
{
	if (!m_isJoystick || !m_isDown)
		return;

	auto camera = m_parent->GetRenderer()->GetCamera();
	auto aspectScale = glm::vec3((float)Application::m_windowWidth / camera->m_width, (float)Application::m_windowHeight / camera->m_height, 1.0f);
	auto mousePos = glm::vec3(m_mouseX, m_mouseY, 0.0) / aspectScale;
	mousePos -= glm::vec3{camera->m_width * 0.5f, camera->m_height * 0.5f, 0.0f};

	auto scale = m_parent->GetTransform()->m_scale * glm::vec3{ m_parent->GetTransform()->m_width, m_parent->GetTransform()->m_height, 1.0 };
	auto centerPosition = m_defaultPosition + (scale * 0.5f);
	//auto newPos = ;

	m_joystickVector = glm::normalize(centerPosition - mousePos);
	/*auto maxLength = (scale.x);
	if (glm::length(m_joystickVector) > maxLength)
	{
		m_joystickVector = glm::normalize(m_joystickVector) * (scale.x);
	}
	auto l = glm::length(m_joystickVector);*/
	//m_joystickVector = glm::normalize((centerPosition - mousePos) * (scale * 0.25f));
	/*auto max = (glm::normalize(m_joystickVector) * (scale * 0.5f));
	if (m_joystickVector.length() > max.length())
	{
		m_joystickVector = max;
	}*/
	//std::cout << std::to_string(m_joystickVector.x) << ":" << std::to_string(m_joystickVector.y) << std::endl;
	

	m_parent->GetTransform()->m_position = (centerPosition - (m_joystickVector*scale * 0.25f)) - (scale * 0.5f);

}
