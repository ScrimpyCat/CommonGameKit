#version 330 core
#include <core>

uniform sampler2D tex;
uniform float width;
uniform float edge;

in vec4 colour;
in vec2 texCoord;

out vec4 fragColour;

void main()
{
    float dist = 1.0 - texture(tex, texCoord).a;
    float a = 1.0 - smoothstep(width, width + edge, dist);

    fragColour = premultiply(vec4(colour.rgb * colour.a * a, colour.a * a));
}
