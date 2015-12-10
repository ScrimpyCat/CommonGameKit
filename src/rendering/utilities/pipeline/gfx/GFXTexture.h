/*
 *  Copyright (c) 2015, Stefan Johnson
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

#ifndef Blob_Game_GFXTexture_h
#define Blob_Game_GFXTexture_h

#include <CommonC/Common.h>
#include "PixelData.h"

typedef enum {
    GFXTextureHintMask = 3,
    GFXTextureHintDimension1D = 0,
    GFXTextureHintDimension2D,
    GFXTextureHintDimension3D,
    
    GFXTextureHintAddressModeMask = 3,
    GFXTextureHintAddressModeClampToEdge = 0,
    GFXTextureHintAddressModeRepeat,
    GFXTextureHintAddressModeRepeatMirror,
    
    GFXTextureHintFilterModeMask = 1,
    GFXTextureHintFilterModeNearest = 0,
    GFXTextureHintFilterModeLinear,
    
    GFXTextureHintFilterMin = 2,
    GFXTextureHintFilterMag = 3,
    GFXTextureHintAddress_S = 4,
    GFXTextureHintAddress_T = 6,
    GFXTextureHintAddress_R = 8
} GFXTextureHint;


typedef struct GFXTexture *GFXTexture;


GFXTexture GFXTextureCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
void GFXTextureDestroy(GFXTexture Texture);
GFXTextureHint GFXTextureGetHints(GFXTexture Texture);
void GFXTextureGetSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static inline GFXTextureHint GFXTextureGetFilterMode(GFXTexture Texture, GFXTextureHint FilterType);
void GFXTextureSetFilterMode(GFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);
static inline GFXTextureHint GFXTextureGetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate);
void GFXTextureSetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);
void GFXTextureInvalidate(GFXTexture Texture);



static inline GFXTextureHint GFXTextureGetFilterMode(GFXTexture Texture, GFXTextureHint FilterType)
{
    return (GFXTextureGetHints(Texture) >> FilterType) & GFXTextureHintFilterModeMask;
}

static inline GFXTextureHint GFXTextureGetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate)
{
    return (GFXTextureGetHints(Texture) >> Coordinate) & GFXTextureHintAddressModeMask;
}

#endif
