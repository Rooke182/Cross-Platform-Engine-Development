#pragma once
#include "BaseComponent.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameObject;

class TransformComponent :public BaseComponent
{
public:
	glm::vec3 m_position = {0.0, 0.0, 0.0};
	glm::vec3 m_scale = { 1.0, 1.0, 1.0 };
	glm::vec3 m_angle = { 0.0, 0.0, 0.0 };
	glm::vec3 m_pivot = { 0.5f, 0.5f, 0.5f };
	float m_width = 1.0f;
	float m_height = 1.0f;

	glm::mat4 GetTransformMatrix();
	CreateComponentID(TransformComponent);


	std::string GetName() override { return "TransformComponent"; };

protected:
	void RenderComponentMenu(int ID) override;
	friend class RenderComponent;
	friend class Debugger;
	friend class DebuggerFunctions;
	friend class DebuggerMenus;
};

