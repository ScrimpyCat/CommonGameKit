#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColour;
layout (location = 2) in vec2 vTexCoord;

out vec4 colour;
out vec2 texCoord;

uniform mat4 modelViewProjectionMatrix;

void main()
{
	colour = vColour;
    texCoord = vTexCoord;
    gl_Position = modelViewProjectionMatrix * vPosition;
}
