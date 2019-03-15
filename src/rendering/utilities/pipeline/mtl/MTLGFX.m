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

#include "MTLGFX_Private.h"


static MTLInternal MTLInfo = {
    .device = nil,
    .commandQueue = nil,
    .samplers = {nil}
};

static GFXMainInfo MTLGFXInfo = {
    .internal = &MTLInfo,
};

GFXMainInfo * const MTLGFX = &MTLGFXInfo;

static inline CC_CONSTANT_FUNCTION MTLSamplerMinMagFilter SamplerFilter(GFXTextureHint Hint, GFXTextureHint FilterType)
{
    return (MTLSamplerMinMagFilter[2]){
        MTLSamplerMinMagFilterNearest,
        MTLSamplerMinMagFilterLinear
    }[(Hint >> FilterType) & GFXTextureHintFilterModeMask];
}

static inline CC_CONSTANT_FUNCTION MTLSamplerAddressMode SamplerAddressMode(GFXTextureHint Hint, GFXTextureHint Coordinate)
{
    return (MTLSamplerAddressMode[3]){
        MTLSamplerAddressModeClampToEdge,
        MTLSamplerAddressModeRepeat,
        MTLSamplerAddressModeMirrorRepeat
    }[(Hint >> Coordinate) & GFXTextureHintAddressModeMask];
}

_Static_assert(sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)) == (((GFXTextureHintFilterModeMask << GFXTextureHintFilterMin) | (GFXTextureHintFilterModeMask << GFXTextureHintFilterMag) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_S) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_T) | (GFXTextureHintAddressModeMask << GFXTextureHintAddress_R)) >> 2), "Sampler count needs to be updated");
_Static_assert(GFXTextureHintFilterMin == 2, "Smallest mask index was changed");

id <MTLSamplerState>MTLGFXGetSampler(GFXTextureHint Hint)
{
    const size_t Index = Hint >> 2;
    
    CCAssertLog(Index <= sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)), "Unsupported hint");
    
    id <MTLSamplerState>Sampler = MTLInfo.samplers[Index];
    
    CCAssertLog(Sampler, "No sampler for hint");
    
    return Sampler;
}

void MTLGFXSetup(void)
{
    GFXMain = MTLGFX;
    
    MTLInfo.device = (__bridge id<MTLDevice>)((__bridge_retained CFTypeRef)MTLCreateSystemDefaultDevice()); // TODO: Setup notifications to manage devices
    MTLInfo.commandQueue = (__bridge id<MTLCommandQueue>)((__bridge_retained CFTypeRef)[MTLInfo.device newCommandQueue]);
    
    //TODO: Add config to specify which samplers to cache
    MTLSamplerDescriptor *SamplerDescriptor = [MTLSamplerDescriptor new];
    for (size_t Loop = 0; Loop < sizeof(MTLInfo.samplers) / sizeof(typeof(*MTLInfo.samplers)); Loop++)
    {
        GFXTextureHint Hint = (GFXTextureHint)(Loop << 2);
        SamplerDescriptor.sAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_S);
        SamplerDescriptor.tAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_T);
        SamplerDescriptor.rAddressMode = SamplerAddressMode(Hint, GFXTextureHintAddress_R);
        SamplerDescriptor.minFilter = SamplerFilter(Hint, GFXTextureHintFilterMin);
        SamplerDescriptor.magFilter = SamplerFilter(Hint, GFXTextureHintFilterMag);
        
        MTLInfo.samplers[Loop] = (__bridge id<MTLSamplerState>)((__bridge_retained CFTypeRef)[MTLInfo.device newSamplerStateWithDescriptor: SamplerDescriptor]);
    }
}

