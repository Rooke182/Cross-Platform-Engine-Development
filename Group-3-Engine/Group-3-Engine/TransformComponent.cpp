#include "TransformComponent.h"
#include "GameObject.h"

glm::mat4 TransformComponent::GetTransformMatrix()
{
	auto scale = m_scale * glm::vec3{ m_width, m_height, 1.0 };
	//translate pivot, rotate and then translate back
	auto rotation = glm::translate(glm::mat4(1.0), m_pivot * scale) * glm::mat4(glm::quat(glm::radians(m_angle))) * glm::inverse(glm::translate(glm::mat4(1.0), m_pivot * scale));

	auto mat = glm::translate(glm::mat4(1.0), m_position) * rotation * glm::scale(glm::mat4(1.0), scale);
	if (m_parent != nullptr && m_parent->GetParent() != nullptr && m_parent->GetParent()->GetTransform() != nullptr)
		return m_parent->GetParent()->GetTransform()->GetTransformMatrix() * mat; //apply parent matrix

	return mat;
}


void TransformComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Transform Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{
		m_position = DebuggerFunctions::Vec3Input("Position:", "##Position", m_position, true, true, true);
		m_position = DebuggerFunctions::Vec3Input("Depth:", "##Depth", m_position, false, false, true);
		m_scale = DebuggerFunctions::Vec3Input("Scale:", "##Scale", m_scale, true, true, false);
		m_angle = DebuggerFunctions::Vec3Input("Angle:", "##Angle", m_angle, false, false, true);
		m_pivot = DebuggerFunctions::Vec3Input("Pivot:", "##Pivot", m_pivot, true, true, false);
		//RemoveComponentMenu(ID);
	}
#endif DEBUGGER
}
