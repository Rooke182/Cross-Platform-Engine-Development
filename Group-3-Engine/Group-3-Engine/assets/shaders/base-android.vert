#version 300 es
layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec2 vertUV;

out vec2 fragUV;

uniform mat4 transform;
uniform mat4 view;
uniform vec4 frameUV;

void main()
{
	gl_Position = view * transform * vec4(vertPos, 1.0);
	fragUV = (vertUV * frameUV.zw) + frameUV.xy; //xy are offsets, zw will multiply (to scale down to an area)
}
