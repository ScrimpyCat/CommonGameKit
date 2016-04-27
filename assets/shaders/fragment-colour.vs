#version 330 core

layout (location = 0) in vec4 vPosition;

uniform mat4 modelViewProjectionMatrix;

void main()
{
    gl_Position = modelViewProjectionMatrix * vPosition;
}
