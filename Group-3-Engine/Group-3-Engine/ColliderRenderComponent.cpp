#include "ColliderRenderComponent.h"
#include "Collider.h"
#include "GameObject.h"
#include "Scene.h"

void ColliderRenderComponent::Init()
{
	m_collider = m_parent->GetComponentOfType<Collider>();
	if (m_collider == nullptr) return;
	m_renderData = new RenderData();
	std::vector<Vertex> vertices =
	{
		Vertex{ 0.0, 0.0, 0.0, 0.0, 0.0 },
		Vertex{ 0.0, 1.0, 0.0, 0.0, 1.0 },
		Vertex{ 1.0, 0.0, 0.0, 1.0, 0.0 },
		Vertex{ 1.0, 1.0, 0.0, 1.0, 1.0 }
	};
	std::vector<unsigned int> indices = { 0,1,2,2,1,3 };

	if (m_collider->GetType() == CIRCLE) //generate circle vertices
	{
		vertices.clear();
		indices.clear();

		for (int i = 0; i < 45; i++)
		{
			float angle = 8.0f * i;
			float angleRad = angle * (3.141592f / 180.0f);
			float x = cos(angleRad)*0.5f;
			float y = sin(angleRad)*0.5f;
			vertices.push_back({ x + 0.5f, y + 0.5f, 0.0f, 0.0f, 0.0f });
			vertices.push_back({ 0.5f, 0.5f, 0.0f, 0.0f, 0.0f });

			float angleNext = 8.0f * (i+1);
			float angleRadNext = angleNext * (3.141592f / 180.0f);
			float xNext = cos(angleRadNext) * 0.5f;
			float yNext = sin(angleRadNext) * 0.5f;
			vertices.push_back({ xNext + 0.5f, yNext + 0.5f, 0.0f, 0.0f, 0.0f });
		}
		m_renderData->m_useIndices = false;
	}

	m_renderData->GenerateBuffers();
	m_renderData->BindVertexData(vertices, indices);
#ifdef OPENGL
	m_renderData->m_shaderProgram = Resources::GetShaderProgram("assets/shaders/base", "assets/shaders/collider");
#endif



	RenderComponent::Init();
}

void ColliderRenderComponent::Render()
{
#ifdef OPENGL
#ifndef ANDROID //apparently wireframe is not supported on opengl es
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
#endif
	m_transform->m_position = m_parent->GetTransform()->m_position;
	//m_transform->m_position.z = 5000;

	m_transform->m_angle = m_parent->GetTransform()->m_angle;
	m_transform->m_pivot = m_parent->GetTransform()->m_pivot;

	//update scale for each collider to match
	if (m_collider->GetType() == AABB || m_collider->GetType() == OBB)
	{
		//m_transform->m_scale = m_collider->GetExtents()*2.0f;
		if (m_collider->GetType() != OBB)
			m_transform->m_angle = { 0.0f, 0.0f, 0.0f };
	}
	if (m_collider->GetType() == CIRCLE)
	{
		//m_transform->m_scale = m_collider->GetExtents() * 2.0f;
	}
	if (m_collider->GetType() == PLANE)
	{
		m_transform->m_scale.x = 0;
	}
	

	m_buffer.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	if (m_collider->isColliding)
	{
		m_buffer.color = { 0.0f, 1.0f, 0.0f, 1.0f };
		m_collider->isColliding = false;
	}
#ifdef DEBUGGER
	RenderComponent::Render();
#endif
#ifdef OPENGL
#ifndef ANDROID
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
#endif
}

void ColliderRenderComponent::RenderComponentMenu(int ID)
{
#ifdef DEBUGGER
	ColliderRenderComponent* collider = dynamic_cast<ColliderRenderComponent*>(this);

	if (collider == nullptr) return;

	std::string label = "sprite Renderer Component##" + std::to_string(ID);
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
