#version 330 core

uniform float radius;
const vec2 circleCenter = vec2(0.5, 0.5);
const float dist = 0.997;
uniform vec2 scale;
uniform float outline;

in vec4 colour;
in vec4 outlineColour;
in vec2 coord;

out vec4 fragColour;

void main()
{
    vec2 test = vec2(lessThan(coord, vec2(outline))) * vec2(greaterThan(coord, vec2(1.0 - outline)));
	float inside = test.x * test.y;

    vec2 shift = (1.0 - scale) * radius;

    vec2 pos = abs(coord - 0.5) + radius - (1.0 - dist);
    float edgeDistance = distance(pos*scale, circleCenter*scale-shift) - radius;
    edgeDistance = smoothstep(dist, 1.0, edgeDistance + 1.0);
    edgeDistance = 1.0 - edgeDistance;

    pos += 1.0 - outline;
    float innerEdgeDistance = distance(pos*scale, circleCenter*scale-shift) - radius;
    innerEdgeDistance = smoothstep(dist, 1.0, innerEdgeDistance + 1.0);
    innerEdgeDistance = 1.0 - innerEdgeDistance;

    pos = abs(coord - 0.5);
    float edgeDistance2 = distance(pos*scale, circleCenter*scale) - radius;
    edgeDistance2 = smoothstep(1.0, 1.0, edgeDistance2 + 1.0);

    pos += 1.0 - outline;
    float innerEdgeDistance2 = inside * distance(pos*scale, circleCenter*scale) - radius;
    innerEdgeDistance2 = smoothstep(1.0, 1.0, innerEdgeDistance2 + 1.0);

    float edge = min(edgeDistance2 + edgeDistance, 1.0);
    float innerEdge = min(innerEdgeDistance2 + innerEdgeDistance, 1.0);
    fragColour = mix(vec4(outlineColour.rgb, outlineColour.a * edge), vec4(colour.rgb, colour.a * innerEdge), innerEdge);
}
