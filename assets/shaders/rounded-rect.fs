#version 330 core
#include <core>

uniform float radius;
const vec2 circleCenter = vec2(0.5, 0.5);
const float dist = 0.997;
uniform vec2 scale;

in vec4 colour;
in vec2 coord;

out vec4 fragColour;

void main()
{
    vec2 shift = (1.0 - scale) * radius;

    vec2 pos = abs(coord - 0.5) + radius - (1.0 - dist);
    float edgeDistance = distance(pos*scale, circleCenter*scale-shift) - radius;

    edgeDistance = smoothstep(dist, 1.0, edgeDistance + 1.0);
    edgeDistance = 1.0 - edgeDistance;

    pos = abs(coord - 0.5);
    float edgeDistance2 = distance(pos*scale, circleCenter*scale) - radius;

    edgeDistance2 = smoothstep(1.0, 1.0, edgeDistance2 + 1.0);

    float edge = min(edgeDistance2 + edgeDistance, 1.0);
    fragColour = premultiply(vec4(colour.rgb, colour.a * edge));
}
