vec4 premultiply(vec4 colour)
{
    return vec4(colour.rgb * colour.a, colour.a);
}
