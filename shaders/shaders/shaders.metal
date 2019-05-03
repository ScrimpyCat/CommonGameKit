//
//  shaders.metal
//  shaders
//
//  Created by Stefan Johnson on 3/5/19.
//  Copyright © 2019 Stefan Johnson. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

inline float4 premultiply(float4 colour)
{
    return float4(colour.rgb * colour.a, colour.a);
}
