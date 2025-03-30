#version 330 core
out vec4 FragColor;

in vec2 fragUV;

uniform sampler2D texture0;
uniform vec4 flashColor;
uniform float flashPercent;

void main()
{
	FragColor = mix(texture(texture0, fragUV), flashColor, flashPercent);
}