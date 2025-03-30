#version 330 core
out vec4 FragColor;

in vec2 fragUV;

uniform sampler2D texture0;
uniform vec4 color;

void main()
{
	FragColor = vec4(texture(texture0, fragUV).r) * color;
	if (FragColor.a <= 0.0)
		discard;
}