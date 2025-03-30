#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ConstantBuffer
{
	glm::mat4 transform; //transform of the object
	glm::mat4 view; //camera transform
	glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0 }; //rgb + alpha
	glm::vec4 frameUV = { 0.0f, 0.0f, 1.0f, 1.0f }; //xy, width, height
};

