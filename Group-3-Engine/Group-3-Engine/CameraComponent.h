#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BaseComponent.h"
class Application;
class GameObject;

class CameraComponent : public BaseComponent
{
public: 
	float m_x, m_y, m_angle = 0.0f;
	float m_zoom = 1.0f;

	float m_width = 1280.0f;
	float m_height = 720.0f;

	CreateComponentID(CameraComponent);

	virtual glm::mat4 GetTransformMatrix();
	virtual void PreRender() override;

    virtual glm::mat4 GetScreenMatrix();


	std::string GetName() override { return "CameraComponent"; };


private:
	glm::mat4 m_transformMatrix = glm::mat4(1.0);

	void UpdateCameraTransform()
	{
		auto projection = glm::ortho(0.0f, m_width, m_height, 0.0f, -10000.0f, 10000.0f);
		auto view = GetViewMatrix();

		m_transformMatrix = projection * view;
	}

	void RenderComponentMenu(int ID) override;
    inline glm::mat4 GetViewMatrix()
    {
        return glm::translate(glm::mat4(1.0), glm::vec3(m_width*0.5f, m_height*0.5f, 0.0)) //center the camera
               * glm::scale(glm::mat4(1.0), glm::vec3(m_zoom, m_zoom, m_zoom)) //zoom
               * glm::rotate(glm::mat4(1.0), glm::radians(m_angle), glm::vec3(0.0f, 0.0f, 1.0f)) //rotate
               * glm::translate(glm::mat4(1.0), glm::vec3(m_x, m_y, 0.0)); //translate
    }

};

