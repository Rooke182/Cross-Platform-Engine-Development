#version 300 es
precision mediump float;
out vec4 FragColor;

in vec2 fragUV;

uniform sampler2D texture0;
uniform vec4 color;

void main()
{
	FragColor = color;
}