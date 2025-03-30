#include "RenderComponent.h"
#include "GameObject.h"
#include "Scene.h"
#include "ColliderRenderComponent.h"
#include "UIWidget.h"
#include "UIScene.h"

void RenderComponent::Init()
{
	if (m_parent != nullptr)
	{
		m_transform = m_parent->GetComponentOfType<TransformComponent>();
		if (m_camera == nullptr) //default if not manually set
			m_camera = m_parent->GetScene()->GetComponentOfType<CameraComponent>();
	}
	else if (m_UIparent != nullptr)
	{
		m_transform = m_UIparent->GetComponentOfType<TransformComponent>();
		if (m_camera == nullptr)
			m_camera = m_UIparent->GetScene()->GetComponentOfType<CameraComponent>();
	}
}

void RenderComponent::Render()
{
	if (m_camera != nullptr)
	{
		//6if (&m_camera->m_transformMatrix != nullptr)
			m_buffer.view = m_camera->GetTransformMatrix();
		
	}
	if (m_transform != nullptr)
		m_buffer.transform = m_transform->GetTransformMatrix();

	if (m_renderData)
		m_renderData->Render(m_buffer);
}

void RenderComponent::Cleanup()
{
	if (m_renderData)
	{
		m_renderData->Cleanup(); //remove later
		delete m_renderData;
		m_renderData = nullptr;
	}
}

void RenderComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	std::string label = "Renderer Component##" + std::to_string(ID);
	if (ImGui::CollapsingHeader(label.c_str()))
	{

		DebuggerMenus::SpriteRendererMenu();
		if (m_renderData->m_texture != nullptr) ImGui::Image((void*)(intptr_t)m_renderData->m_texture->m_textureID, ImVec2(256, 256));
		ImGui::ColorEdit4("Color", glm::value_ptr(m_buffer.color));


		if (ImGui::TreeNode("Renderer Data"))
		{
			DebuggerFunctions::TextOutput("BufferType: ", std::to_string(m_renderData->m_bufferType));
			DebuggerFunctions::TextOutput("Render Mode: ", std::to_string(m_renderData->m_renderMode));
			DebuggerFunctions::TextOutput("Vertex Buffer: ", std::to_string(m_renderData->m_vertexBuffer));
			DebuggerFunctions::TextOutput("Index Buffer: ", std::to_string(m_renderData->m_indexBuffer));
			DebuggerFunctions::TextOutput("Vertex Array: ", std::to_string(m_renderData->m_vertexArray));
			DebuggerFunctions::TextOutput("Vertex Count: ", std::to_string(m_renderData->m_vertexCount));
			DebuggerFunctions::TextOutput("Shader Program: ", std::to_string(m_renderData->m_shaderProgram));
			if (m_renderData->m_texture != nullptr)DebuggerFunctions::TextOutput("Texture ID: ", std::to_string(m_renderData->m_texture->m_textureID));

			ImGui::TreePop();
		}

		ImGui::SetCursorPosX(0);

		RemoveComponentMenu(ID);
	}
#endif
}

