#include "CameraComponent.h"
#include "GameObject.h"
#include "Application.h"

void CameraComponent::PreRender()
{
	UpdateCameraTransform();
}

glm::mat4 CameraComponent::GetTransformMatrix()
{
	return m_transformMatrix;
}


void CameraComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
    std::string label = "Camera Component##" + std::to_string(ID);
    if (ImGui::CollapsingHeader(label.c_str()))
    {
        glm::vec3 _temp = { m_x, m_y,0 };
        _temp = DebuggerFunctions::Vec3Input("Position: ", "##Camera Position", _temp, true, true, false);
        m_x = _temp.x; m_y = _temp.y;

        _temp = { m_width, m_height,0 };
        _temp = DebuggerFunctions::Vec3Input("Dimensions: ", "##Camera Dimensions", _temp, true, true, false);
        m_width = _temp.x; m_height = _temp.y;

        m_angle = DebuggerFunctions::FloatInput("Angle: ", "##Camera Angle", m_angle);

        m_zoom = DebuggerFunctions::FloatInput("Zoom: ", "##Camera Zoom", m_zoom);

        //RemoveComponentMenu(ID);
    }
#endif
}

glm::mat4 CameraComponent::GetScreenMatrix()
{
    auto aspectScale = glm::vec3((float)Application::m_windowWidth / m_width, (float)Application::m_windowHeight / m_height, 1.0f);
    auto aspect = glm::scale(glm::mat4(1.0), aspectScale);
    return aspect * GetViewMatrix();
}
