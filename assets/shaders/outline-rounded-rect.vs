#version 330 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColour;
layout (location = 2) in vec4 vColourOutline;
layout (location = 3) in vec2 vCoord;

out vec4 colour;
out vec4 outlineColour;
out vec2 coord;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    coord = vCoord;
    colour = vColour;
    outlineColour = vColourOutline;
    gl_Position = modelViewProjectionMatrix * vPosition;
}
