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
    uint flags [[flat]];
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

namespace poxel
{
    class depth
    {
        union
        {
            uint value;
            struct
            {
                uint blocks : 24;
                uint : 4;
                uint mode : 3;
                uint transparent : 1;
            };
        };
        
    public:
        inline depth()
        {
            *this = 0;
        }
        
        inline depth(uint v)
        {
            value = v;
        }
        
        inline depth operator=(uint rhs)
        {
            value = rhs;
            
            return *this;
        }
        
        inline bool empty() const
        {
            return transparent;
        }
        
        inline bool multi() const
        {
            return mode >= 3;
        }
        
        inline bool block(uint i) const
        {
            if (empty()) return false;
            
            switch (mode)
            {
                case 0:
                case 3:
                    return i >= blocks;
                    
                case 1:
                case 4:
                {
                    uint block = 0;
                    for (int index = 16; index > -1; index -= 8)
                    {
                        uint accum = (blocks >> index) & 0xff;
                        block += accum >> 4;
                        
                        if (i < block) return false;
                        
                        block += (accum & 0xf);
                        
                        if (i < block) return true;
                    }
                    
                    return false;
                }
                    
                case 2:
                case 5:
                    return (blocks >> i) & 1;
                    
                default:
                    return false;
            }
        }
    };
    
    class palette
    {
        texture2d<uint> texture;
        float2 base;
        uint size;
        uint count;
        
    public:
        
        inline palette(texture2d<uint> paletteTexture, float2 paletteCoord, uint paletteSize, uint paletteCount)
        {
            texture = paletteTexture;
            base = paletteCoord;
            size = paletteSize;
            count = paletteCount;
        }
        
        inline uint lookup(float2 coord, uint index) const
        {
            constexpr sampler nearestSampler(mip_filter::nearest,
                                             mag_filter::nearest,
                                             min_filter::nearest,
                                             address::repeat);
            
            const uint4 lut = texture.sample(nearestSampler, base + coord);
            const uint C = ctz(count);
            const uint indexesPerPattern = (size * 8) / C;
            index = index % indexesPerPattern;
            
            const uint component = (index * C) / 32;
            index = (index * C) % 32;
            
            //TODO: reorder palette indexes to avoid shuffling (byte order stays the same, index bit order changes from msb->lsb to lsb->msb)
            const uint mask = (count - 1) << ((32 - C) - index);
            return ((lut[component] << (32 - (size * 8))) & mask) >> ((32 - C) - index);
        }
    };
    
    class colour
    {
        texture2d<float> texture;
        float2 base;
        
    public:
        
        inline colour(texture2d<float> colourTexture, float2 colourCoord)
        {
            texture = colourTexture;
            base = colourCoord;
        }
        
        inline float2 coord(uint index) const
        {
            return float2(index / texture.get_width(), 0);
        }
        
        inline float4 sample(float2 coord) const
        {
            constexpr sampler nearestSampler(mip_filter::nearest,
                                             mag_filter::nearest,
                                             min_filter::nearest,
                                             address::repeat);
            
            return texture.sample(nearestSampler, base + coord);
        }
    };
};

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
    out.scale = as_type<float3>(as_type<uint3>(instance[index].scale) & 0x7fffffff);
    out.depth = as_type<float2>(as_type<uint2>(instance[index].coords[in.vFace].depth) & 0x3fffffff);
    out.colour = as_type<float2>(as_type<uint2>(instance[index].coords[in.vFace].colour) & 0x3fffffff);
    out.palette = as_type<float2>(as_type<uint2>(instance[index].coords[in.vFace].palette) & 0x3fffffff);
    
    const uint3 scale = (as_type<uint3>(instance[index].scale) & 0x80000000) >> uint3(17, 18, 19);
    const uint2 depth = (as_type<uint2>(instance[index].coords[in.vFace].depth) & 0xc0000000) >> uint2(20, 22);
    const uint2 colour = (as_type<uint2>(instance[index].coords[in.vFace].colour) & 0xc0000000) >> uint2(24, 26);
    const uint2 palette = (as_type<uint2>(instance[index].coords[in.vFace].palette) & 0xc0000000) >> uint2(28, 30);
    
    const uint flags = scale.x | scale.y | scale.z | depth.x | depth.y | colour.x | colour.y | palette.x | palette.y;
    out.flags = flags;
    
    return out;
}

fragment float4 poxel_fs(VertexOut in [[stage_in]], constant GeometryData &geometry [[buffer(0)]], constant LightData *lights[[buffer(2)]])
{
    constexpr sampler nearestSampler(mip_filter::nearest,
                                     mag_filter::nearest,
                                     min_filter::nearest,
                                     address::repeat);
    
    const uint paletteType = in.flags & 3;
    
    const uint sizes[4] = { 0, 1, 4, 16 };
    const uint paletteCount = 2 << (((in.flags & (3 << 2)) * 2) + 1); //4 16 (unused:64) 256
    
    texture2d<uint> paletteIndexTextures[4] = { geometry.colour.paletteIndex.R8Uint, geometry.colour.paletteIndex.R8Uint, geometry.colour.paletteIndex.R32Uint, geometry.colour.paletteIndex.RGBA32Uint };
    
    const poxel::palette palette = poxel::palette(paletteIndexTextures[paletteType], in.palette, sizes[paletteType], paletteCount);
    const poxel::colour colour = poxel::colour(geometry.colour.RGBA8Unorm_sRGB, in.colour);
    
    const float3 origin = in.near.xyz;
    const float3 normal = faces[in.face].normal;
    const float3 dir = normalize(in.far.xyz - origin);
    
    const float planeD = dot(origin, normal);
    const float NL = length(normal);
    
    const float2 ts = float2(
        (in.scale.x * faces[in.face].planarX.x) + (in.scale.z * faces[in.face].planarX.y),
        (in.scale.y * faces[in.face].planarY.x) + (in.scale.z * faces[in.face].planarY.y)
    );
    const float2 hts = ts / 2;
    
    const float2 depthSize = float2(geometry.depth.get_width(), geometry.depth.get_height());
    
    for (int steps = 0; ; steps++)
    {
        const float3 p = origin + (dir * steps);
        const float d = abs(planeD - dot(p, normal)) / NL;
        
        const float3 planeP = p - (d * normal);
        
        float2 t = float2(
            (planeP.x * faces[in.face].planarX.x) + (planeP.z * faces[in.face].planarX.y),
            (planeP.y * faces[in.face].planarY.x) + (planeP.z * faces[in.face].planarY.y)
        );
        const float2 o = faces[in.face].order;
        
        if ((t.x < -hts.x) || (t.x > hts.x) || (t.y < -hts.y) || (t.y > hts.y)) break;
        
        t = (t + hts) / ts;
        t = abs(o - t);
        
        t = (t * ts) / depthSize;
    }
    
    return float4(0);
}

#endif

#endif
