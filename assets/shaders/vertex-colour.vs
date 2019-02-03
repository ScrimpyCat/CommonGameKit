#version 330 core
#include <core>

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColour;

out vec4 colour;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    colour = premultiply(vColour);
    gl_Position = modelViewProjectionMatrix * vPosition;
}
