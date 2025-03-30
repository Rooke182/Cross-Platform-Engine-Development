#include "PostProcessRenderComponent.h"
#include "Application.h"

void PostProcessRenderComponent::Init()
{
	m_renderData = new RenderData();
	std::vector<Vertex> vertices =
	{
		Vertex{ -1.0, -1.0, 0.0, 0.0, 0.0 },
		Vertex{ -1.0, 1.0, 0.0, 0.0, 1.0 },
		Vertex{ 1.0, -1.0, 0.0, 1.0, 0.0 },
		Vertex{ 1.0, 1.0, 0.0, 1.0, 1.0 }
	};
	std::vector<unsigned int> indices = { 0,1,2,2,1,3 };
	m_renderData->GenerateBuffers();
	m_renderData->BindVertexData(vertices, indices);
	m_lastWidth = Application::m_windowWidth;
	m_lastHeight = Application::m_windowHeight;
}

void PostProcessRenderComponent::PreRender()
{
	if (!m_postProcesses.empty())
	{
		if (m_lastWidth != Application::m_windowWidth || m_lastHeight != Application::m_windowHeight)
		{
			m_lastWidth = Application::m_windowWidth;
			m_lastHeight = Application::m_windowHeight;

			if (m_lastWidth > 0 && m_lastHeight > 0)
			{
				//need to update buffers to match window size
				for (int i = 0; i < m_postProcesses.size(); i++)
				{
					m_postProcesses[i]->Resize(Application::m_windowWidth, Application::m_windowHeight);
				}
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_postProcesses[0]->m_frameBuffer);
		glViewport(0, 0, Application::m_windowWidth, Application::m_windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void PostProcessRenderComponent::PostRender()
{
	if (!m_postProcesses.empty())
	{
		for (int i = 0; i < m_postProcesses.size(); i++)
		{
			auto currentPP = m_postProcesses[i];
			
			if (i == m_postProcesses.size() - 1) //last post process
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0); //draw to screen buffer since its the last
				glViewport(0, 0, Application::m_windowWidth, Application::m_windowHeight);
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_postProcesses[i+1]->m_frameBuffer);
				glViewport(0, 0, Application::m_windowWidth, Application::m_windowHeight);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			m_buffer.transform = glm::mat4(1.0f);
			m_buffer.view = glm::mat4(1.0f);

			m_renderData->m_shaderProgram = currentPP->m_shaderProgram;
			m_renderData->m_texture = nullptr;

			//bind shader and texture
			glUseProgram(currentPP->m_shaderProgram);
			glUniform1i(glGetUniformLocation(currentPP->m_shaderProgram, "texture0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, currentPP->m_texture);

			for (auto& value : currentPP->m_values)
				value->ApplyValueToShader(currentPP->m_shaderProgram);

			if (m_renderData)
				m_renderData->Render(m_buffer);
		}
	}
}

void PostProcessRenderComponent::Cleanup()
{
	for (int i = 0; i < m_postProcesses.size(); i++)
	{
		m_postProcesses[i]->Cleanup();
		delete m_postProcesses[i];
	}
	RenderComponent::Cleanup();
}

void PostProcessRenderComponent::AddPostProcess(std::string name, std::string vertexPath, std::string fragmentPath, std::vector<IPostProcessValue*> postProcessValues)
{
	PostProcess* pp = new PostProcess();
	pp->name = name;
	pp->m_shaderProgram = Resources::GetShaderProgram(vertexPath, fragmentPath);
	pp->Init();
	pp->m_values = postProcessValues;

	m_postProcesses.push_back(pp);
}

PostProcess* PostProcessRenderComponent::GetPostProcess(std::string name)
{
	for (auto& pp : m_postProcesses)
		if (pp->name == name)
			return pp;
	return nullptr;
}

