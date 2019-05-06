/*
 *  Copyright (c) 2019, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OutlineRoundedRect_metal
#define OutlineRoundedRect_metal

#include "Core.metal"

typedef struct {
    float2 vPosition [[attribute(0)]];
    float4 vColour [[attribute(1)]];
    float4 vColourOutline [[attribute(2)]];
    float2 vCoord [[attribute(3)]];
} VertexData;

typedef struct {
    float4 position [[position]];
    float4 colour;
    float4 outlineColour;
    float2 coord;
} VertexOut;

#ifndef OutlineRoundedRect_header
vertex VertexOut outline_rounded_rect_vs(VertexData in [[stage_in]], constant float4x4 &modelViewProjectionMatrix [[buffer(1)]])
{
    VertexOut out;
    out.position = modelViewProjectionMatrix * float4(in.vPosition, 0.0, 1.0);
    out.colour = in.vColour;
    out.outlineColour = in.vColourOutline;
    out.coord = in.vCoord;
    return out;
}

fragment float4 outline_rounded_rect_fs(VertexOut in [[stage_in]], constant float &radius [[buffer(0)]], constant float2 &scale [[buffer(1)]], constant float2 &outline [[buffer(2)]])
{
    const float2 circleCenter = float2(0.5, 0.5);
    const float dist = 0.997;
    
    float2 test = float2(lessThan(in.coord, float2(1.0 - outline))) * float2(greaterThan(in.coord, float2(outline)));
    float inside = test.x * test.y;
    
    float2 shift = (1.0 - scale) * radius;
    
    float2 pos = abs(in.coord - 0.5) + radius - (1.0 - dist);
    float edgeDistance = distance(pos * scale, circleCenter * scale - shift) - radius;
    edgeDistance = smoothstep(dist, 1.0, edgeDistance + 1.0);
    edgeDistance = 1.0 - edgeDistance;
    
    pos += outline;
    float innerEdgeDistance = distance(pos * scale, circleCenter * scale - shift) - radius;
    innerEdgeDistance = smoothstep(dist, 1.0, innerEdgeDistance + 1.0);
    innerEdgeDistance = 1.0 - innerEdgeDistance;
    
    pos = abs(in.coord - 0.5);
    float edgeDistance2 = distance(pos * scale, circleCenter * scale) - radius;
    edgeDistance2 = smoothstep(1.0, 1.0, edgeDistance2 + 1.0);
    
    pos += outline;
    float innerEdgeDistance2 = inside * distance(pos * scale, circleCenter * scale) - radius;
    innerEdgeDistance2 = smoothstep(1.0, 1.0, innerEdgeDistance2 + 1.0);
    
    float edge = min(edgeDistance2 + edgeDistance, 1.0);
    float innerEdge = min(innerEdgeDistance2 + innerEdgeDistance, 1.0);
    return core::premultiply(mix(float4(in.outlineColour.rgb, in.outlineColour.a * edge), float4(in.colour.rgb, in.colour.a * innerEdge), innerEdge));
}
#endif

#endif
