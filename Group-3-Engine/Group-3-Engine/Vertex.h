#pragma once
#include "Defines.h"
struct Vertex
{
	float x, y, z, uvX, uvY = 0;

	static void SetAttributeData()
	{
		#ifdef OPENGL
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		#endif
	}
};

