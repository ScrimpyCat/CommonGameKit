/*
 *  Copyright (c) 2020, Stefan Johnson
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

#ifndef Poxel_metal
#define Poxel_metal

#include "Core.metal"

typedef struct {
    float2 vPosition [[attribute(0)]]; //-1 to 1
    float2 vTexCoord [[attribute(1)]]; //0 to 1
    uint vFace [[attribute(2)]]; //0 to 5
} VertexData;

typedef struct {
    float4 position [[position]];
    float4 near;
    float4 far;
    float2 texCoord;
//    float3 normal [[flat]];
    uint face [[flat]];
    float3 scale [[flat]];
    float2 depth [[flat]];
    float2 colour [[flat]];
    float2 palette [[flat]];
} VertexOut;

typedef struct {
    // will change
    float4x4 modelMatrix; //maybe could be 3x4 matrices
    float4x4 inverseModelMatrix;
    // constant
    float3 scale;
    struct {
        float2 depth;
        float2 colour;
        float2 palette; //maybe if NaN or +INF or sign bit or something could indicate non-palettalized colour, sign-bit set could indicate a higher precision palette or something, sign and most significant exponent bit (0x40000000) can be used to inform of the palette mode.
        /*
         If using additional bits to control logic, possibly group instance data so blocks with the same config will be executed together (hopefully
         avoding fewer stalls in simd groups).
         
         
         Additional bits:
         as_type<uint3>(scale) & 0x80000000 (sign bit unused) = 3 free bits
         as_type<uint2>(coords[i].depth) & 0xc0000000 (sign and exponent msb unused) = 4 bits
         as_type<uint2>(coords[i].color) & 0xc0000000 (sign and exponent msb unused) = 4 bits
         as_type<uint>(coords[i].palette) & 0xc0000000 (sign and exponent msb unused) = 2 bits
         
         sssddddccccpp
         
         Sampling modes:
         000ddddccccpp:
            sampled depth
            sampled colour
         
         001ddddxxxxxx: (recolourize objects : debug)
            sampled depth
            no sampled colour, colour (RGB) = coords[i].color.x, coords[i].color.y, coords[i].palette
         
         010xxxxxxxxxx: (cubes : debug)
            no sampled depth (depth) = coords[i].depth.x (single depth - single face)
            sampled colour
         
         011xxxxxxxxxx: (recolourize cubes : debug)
             no sampled depth (depth) = coords[i].depth.x (single depth - single face)
             no sampled colour, colour (RGBA) = coords[i].color.x, coords[i].color.y, coords[i].palette, coords[i].depth.y
         
         sssddddccccpp:
            sampled depth
            sampled colour
            colour map is R8Uint (1x 256-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is R8Uint (2x 16-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is R8Uint (4x 4-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is RGBA8Unorm_sRGB (no palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is R32Uint8/RGBA8Uint (4x 256-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is R32Uint8/RGBA8Uint (8x 16-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is R32Uint8/RGBA8Uint (16x 4-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is RGBA32Uint (16x 256-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is RGBA32Uint (32x 16-palette)
         
         sssddddccccpp:
             sampled depth
             sampled colour
             colour map is RGBA32Uint (64x 4-palette)
         */
    } coords[6];
} InstancedData; //instance uniformed

typedef struct {
    float4x4 viewProjectionMatrix;
    float4x4 inverseViewProjectionMatrix;
} UniformData;

typedef struct {
    texture2d<uint> depth;
    struct {
        texture2d<float> RGBA8Unorm_sRGB;
        struct {
            texture2d<uint> R8Uint;
            texture2d<uint> R32Uint;
            texture2d<uint> RGBA32Uint;
        } paletteIndex;
    } colour;
} GeometryData;

//typedef struct {
//} FilterData;

namespace lighting
{
    enum source
    {
        point,
        directed_point,
        directional,
        ray
    };
    
    struct light
    {
        enum source source;
        float3 position;
        float3 direction;
        float3 colour;
        float intensity;
        uint rayIndex; //starting segment in ray buffer to store rays from this light
        uint3 raySize;
    };
}

typedef struct {
    constant lighting::light *lights;
    uint count;
} LightData;

typedef struct {
    
} TranslucencyData;

typedef struct {
    //rays
    //frag_rays
} RayData;

//separate uniforms/single buffers for viewProjectionMatrix, inverseViewProjectionMatrix, rayPrecisionMethod (or should link another frag function?)

#ifndef Poxel_header

constant struct {
    float3 normal;
    float2 order;
    float2 planarX;
    float2 planarY;
} faces[6] = {
    { .normal = float3( 1, 0, 0), .order = float2(1, 1), .planarX = float2(0, 1), .planarY = float2(1, 0) },
    { .normal = float3(-1, 0, 0), .order = float2(0, 1), .planarX = float2(0, 1), .planarY = float2(1, 0) },
    { .normal = float3( 0, 1, 0), .order = float2(0, 0), .planarX = float2(1, 0), .planarY = float2(0, 1) },
    { .normal = float3( 0,-1, 0), .order = float2(0, 1), .planarX = float2(1, 0), .planarY = float2(0, 1) },
    { .normal = float3( 0, 0, 1), .order = float2(0, 1), .planarX = float2(1, 0), .planarY = float2(1, 0) },
    { .normal = float3( 0, 0,-1), .order = float2(1, 1), .planarX = float2(1, 0), .planarY = float2(1, 0) }
};

vertex VertexOut poxel_vs(VertexData in [[stage_in]], constant InstancedData *instance [[buffer(0)]], constant UniformData &uniforms [[buffer(1)]], const uint index [[instance_id]])
{
    const float4 position = float4(in.vPosition, 1.0);
    const float4 projectedPosition = uniforms.viewProjectionMatrix * instance[index].modelMatrix * position;
    
    VertexOut out;
    out.position = projectedPosition;
    out.near = position;
    out.far = instance[index].inverseModelMatrix * uniforms.inverseViewProjectionMatrix * float4(projectedPosition.xy, 1, 1);
    out.face = in.vFace;
    out.texCoord = in.vTexCoord;
    out.scale = instance[index].scale;
    out.depth = instance[index].coords[in.vFace].depth;
    out.colour = instance[index].coords[in.vFace].colour;
    out.palette = instance[index].coords[in.vFace].palette;
    
    return out;
}

fragment float4 poxel_fs(VertexOut in [[stage_in]], constant GeometryData &geometry [[buffer(0)]], constant LightData *lights[[buffer(2)]])
{
    return float4(0);
}

#endif

#endif
