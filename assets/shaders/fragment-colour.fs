#version 330 core
#include <core>

uniform vec4 colour;

out vec4 fragColour;

void main()
{
    fragColour = premultiply(colour);
}
