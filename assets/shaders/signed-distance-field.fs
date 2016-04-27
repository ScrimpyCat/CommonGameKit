#version 330 core

uniform sampler2D tex;
uniform vec4 colour;
uniform float width;
uniform float edge;

in vec2 texCoord;

out vec4 fragColour;

void main()
{
    float dist = 1.0 - texture(tex, texCoord).a;
    float a = 1.0 - smoothstep(width, width + edge, dist);

    fragColour = vec4(colour.rgb, a);
}
