#pragma once
#include "Defines.h"
#include "ConstantBuffer.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>
class RenderData
{
public:
	#ifdef OPENGL
	unsigned int m_vertexArray = 0;
	unsigned int m_vertexBuffer = 0;
	unsigned int m_indexBuffer = 0;
	unsigned int m_renderMode = GL_TRIANGLES;
	unsigned int m_bufferType = GL_STATIC_DRAW;
	unsigned int m_blendSource = GL_SRC_ALPHA;
	unsigned int m_blendDest = GL_ONE_MINUS_SRC_ALPHA;
	#endif
	ShaderProgram m_shaderProgram = 0;
	bool m_useIndices = true;
	unsigned int m_vertexCount = 0;
	std::shared_ptr<Texture> m_texture = nullptr;
	bool m_blendEnabled = true;

	//store bounds
	float m_vertexMinX = 0.0f;
	float m_vertexMaxX = 1.0f;
	float m_vertexMinY = 0.0f;
	float m_vertexMaxY = 1.0f;

	void GenerateBuffers();

	void BindVertexData(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	void Render(ConstantBuffer& buffer);

	void Cleanup();
};

