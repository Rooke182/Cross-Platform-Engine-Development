#include "RenderData.h"

void RenderData::GenerateBuffers()
{
#ifdef OPENGL
	glGenVertexArrays(1, &m_vertexArray);
	glGenBuffers(1, &m_vertexBuffer);
	glGenBuffers(1, &m_indexBuffer);
#endif
}

void RenderData::BindVertexData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	for (auto& vert : vertices)
	{
		if (vert.x < m_vertexMinX)
			m_vertexMinX = vert.x;
		if (vert.y < m_vertexMinY)
			m_vertexMinY = vert.y;

		if (vert.x > m_vertexMaxX)
			m_vertexMaxX = vert.x;
		if (vert.y > m_vertexMaxY)
			m_vertexMaxY = vert.y;
	}
#ifdef OPENGL
	glBindVertexArray(m_vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), m_bufferType);
	if (m_useIndices)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), m_bufferType);
		m_vertexCount = (unsigned int)indices.size();
	}
	else
	{
		m_vertexCount = (unsigned int)vertices.size();
	}

	Vertex::SetAttributeData();

	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void RenderData::Render(ConstantBuffer& buffer)
{
#ifdef OPENGL

	if (m_blendEnabled)
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_blendSource, m_blendDest);
	}
	else
	{
		glDisable(GL_BLEND);
	}


	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vertexArray);
	if (m_texture)
		m_texture->BindToShader(m_shaderProgram);

	//apply buffer
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(buffer.transform));
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(buffer.view));
	glUniform4f(glGetUniformLocation(m_shaderProgram, "color"), buffer.color.x, buffer.color.y, buffer.color.z, buffer.color.w);
	glUniform4f(glGetUniformLocation(m_shaderProgram, "frameUV"), buffer.frameUV.x, buffer.frameUV.y, buffer.frameUV.z, buffer.frameUV.w);


	if (m_useIndices)
		glDrawElements(m_renderMode, m_vertexCount, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(m_renderMode, 0, m_vertexCount);
#endif	
}

void RenderData::Cleanup()
{
#ifdef OPENGL
	glDeleteVertexArrays(1, &m_vertexArray);
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
	m_texture = nullptr;
#endif
}
