#include "SpriteRenderComponent.h"
#include "GameObject.h"
#include "UIWidget.h"

void SpriteRenderComponent::Init()
{
	
	m_renderData = new RenderData();
	std::vector<Vertex> vertices =
	{
		Vertex{ 0.0, 0.0, 0.0, 0.0, 0.0 },
		Vertex{ 0.0, 1.0, 0.0, 0.0, 1.0 },
		Vertex{ 1.0, 0.0, 0.0, 1.0, 0.0 },
		Vertex{ 1.0, 1.0, 0.0, 1.0, 1.0 }
	};
	std::vector<unsigned int> indices = { 0,1,2,2,1,3 };
	m_renderData->GenerateBuffers();
	m_renderData->BindVertexData(vertices, indices);
	#ifdef OPENGL
	m_renderData->m_shaderProgram = Resources::GetShaderProgram("assets/shaders/base", "assets/shaders/base");
	#endif

	if (m_parent != nullptr && m_parent->GetComponentOfType<TransformComponent>() != nullptr)
	{
		if (m_texturePath != "")
		{
			m_renderData->m_texture = Resources::GetTexture(m_texturePath);
			m_parent->GetTransform()->m_width = (float)m_renderData->m_texture->m_width;
			m_parent->GetTransform()->m_height = (float)m_renderData->m_texture->m_height;
		}

		if (m_parent->GetComponentOfType<AnimatorComponent>() != nullptr)
			m_animator = m_parent->GetComponentOfType<AnimatorComponent>();
	}
	else if (m_UIparent != nullptr)
	{
		if (m_texturePath != "")
		{
			m_renderData->m_texture = Resources::GetTexture(m_texturePath);
			m_UIparent->GetTransformComponent()->m_width = (float)m_renderData->m_texture->m_width;
			m_UIparent->GetTransformComponent()->m_height = (float)m_renderData->m_texture->m_height;

			if (m_UIparent->GetComponentOfType<AnimatorComponent>() != nullptr)
				m_animator = m_UIparent->GetComponentOfType<AnimatorComponent>();
		}
	}

	RenderComponent::Init();
}

void SpriteRenderComponent::Render()
{
	if (m_animator != nullptr)
	{		
		if (m_animator->IsCurrentAnimationValid())
		{
			auto tex = m_animator->GetFrameTexture();
			auto frame = m_animator->GetCurrentFrame();
			float uvX = frame.m_x / (float)tex->m_width;
			float uvY = frame.m_y / (float)tex->m_height;
			float uvWidth = (frame.m_width) / (float)tex->m_width;
			float uvHeight = (frame.m_height) / (float)tex->m_height;

			m_renderData->m_texture = tex;

			if (m_parent != nullptr)
			{
				m_parent->GetTransform()->m_width = frame.m_width;
				m_parent->GetTransform()->m_height = frame.m_height;
			}
			else if (m_UIparent != nullptr)
			{
				m_UIparent->GetTransformComponent()->m_width = frame.m_width;
				m_UIparent->GetTransformComponent()->m_height = frame.m_height;
			}

			m_buffer.frameUV = { uvX, uvY, uvWidth, uvHeight };
		}

	}
	RenderComponent::Render();
}


void SpriteRenderComponent::UpdateTexturePath(std::string texturePath)
{
	if (texturePath != "")
	{
		m_texturePath = texturePath;
		m_renderData->m_texture = Resources::GetTexture(m_texturePath);

		if (m_parent != nullptr)
		{
			m_parent->GetTransform()->m_width = (float)m_renderData->m_texture->m_width;
			m_parent->GetTransform()->m_height = (float)m_renderData->m_texture->m_height;
		}
		else if (m_UIparent != nullptr)
		{
			m_UIparent->GetTransformComponent()->m_width = (float)m_renderData->m_texture->m_width;
			m_UIparent->GetTransformComponent()->m_height = (float)m_renderData->m_texture->m_height;
		}
	}
}
