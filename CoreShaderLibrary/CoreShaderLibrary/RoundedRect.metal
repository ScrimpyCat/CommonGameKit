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

#include <metal_stdlib>
#include "Core.metal"

using namespace metal;

typedef struct {
    float2 position [[attribute(0)]];
    float4 colour [[attribute(1)]];
    float2 coord [[attribute(2)]];
} VertexData;

typedef struct {
    float4 position [[position]];
    float4 colour;
    float2 coord;
} VertexOut;

vertex VertexOut rounded_rect_vs(VertexData Vertices [[stage_in]], constant float4x4 &ModelViewProjection [[buffer(1)]])
{
    VertexOut out;
    out.position = ModelViewProjection * float4(Vertices.position, 0.0, 1.0);
    out.colour = Vertices.colour;
    out.coord = Vertices.coord;
    return out;
}

fragment float4 rounded_rect_fs(VertexOut in [[stage_in]], constant float &Radius [[buffer(0)]], constant float2 &Scale [[buffer(1)]])
{
    const float2 circleCenter = float2(0.5, 0.5);
    const float dist = 0.997;
    
    const float2 shift = (1.0 - Scale) * Radius;
    
    float2 pos = abs(in.coord - 0.5) + Radius - (1.0 - dist);
    float edgeDistance = distance(pos * Scale, circleCenter * Scale - shift) - Radius;
    
    edgeDistance = smoothstep(dist, 1.0, edgeDistance + 1.0);
    edgeDistance = 1.0 - edgeDistance;
    
    pos = abs(in.coord - 0.5);
    float edgeDistance2 = distance(pos * Scale, circleCenter * Scale) - Radius;
    
    edgeDistance2 = smoothstep(1.0, 1.0, edgeDistance2 + 1.0);
    
    float edge = min(edgeDistance2 + edgeDistance, 1.0);
    return core::premultiply(float4(in.colour.rgb, in.colour.a * edge));
}
