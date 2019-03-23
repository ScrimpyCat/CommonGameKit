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

#ifndef CommonGameKit_MTLTexture_h
#define CommonGameKit_MTLTexture_h

#include "MTLGFX_Private.h"


typedef struct {
    void *stream;
    CCPixelData data;
    size_t width;
    size_t height;
    size_t depth;
    union {
        struct {
            GFXTextureHint hint;
            CCColourFormat format;
            __unsafe_unretained id <MTLTexture>texture;
        } root;
        struct {
            size_t x;
            size_t y;
            size_t z;
            struct {
                size_t x;
                size_t y;
                size_t z;
                GFXTexture root;
            } internal;
            GFXTexture parent;
        } sub;
    };
    _Bool isRoot;
} MTLGFXTextureInfo, *MTLGFXTexture;

extern const GFXTextureInterface MTLTextureInterface;

CC_NEW GFXTexture MTLGFXTextureCreate(CCAllocatorType Allocator, id <MTLTexture>Data);

static inline id <MTLTexture>MTLGFXTextureGetTexture(MTLGFXTexture Texture);


static inline id <MTLTexture>MTLGFXTextureGetTexture(MTLGFXTexture Texture)
{
    return Texture->isRoot ? Texture->root.texture : ((MTLGFXTexture)Texture->sub.internal.root)->root.texture;
}

#endif
