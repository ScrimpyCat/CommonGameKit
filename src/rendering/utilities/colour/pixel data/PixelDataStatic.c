/*
 *  Copyright (c) 2015 Stefan Johnson
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

#include "PixelDataStatic.h"
#include <CommonC/Common.h>

typedef struct {
    void *buffer[4]; //TODO: Create generic data type
    size_t width, height, depth;
} CCPixelDataStaticInternal;

static void *CCPixelDataStaticConstructor(CCAllocatorType Allocator);
static void CCPixelDataStaticDestructor(CCPixelDataStaticInternal *Internal);
static CCColour CCPixelDataStaticGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z);
static void CCPixelDataStaticGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth);

const CCPixelDataInterface CCPixelDataStaticInterface = {
    .create = CCPixelDataStaticConstructor,
    .destroy = (CCPixelDataDestructorCallback)CCPixelDataStaticDestructor,
    .colour = CCPixelDataStaticGetColour,
    .optional = {
        .size = CCPixelDataStaticGetSize,
        .packedData = NULL
    }
};

const CCPixelDataInterface * const CCPixelDataStatic = &CCPixelDataStaticInterface;


static void *CCPixelDataStaticConstructor(CCAllocatorType Allocator)
{
    CCPixelDataStaticInternal *Internal = CCMalloc(Allocator, sizeof(CCPixelDataStaticInternal), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Internal)
    {
        *Internal = (CCPixelDataStaticInternal){
            .buffer = { NULL, NULL, NULL, NULL },
            .width = 0,
            .height = 0,
            .depth = 0
        };
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create static pixel data: Failed to allocate memory of size (%zu)", sizeof(CCPixelDataStaticInternal));
    }
    
    return Internal;
}

static void CCPixelDataStaticDestructor(CCPixelDataStaticInternal *Internal)
{
//    CC_SAFE_Free(Internal->buffer[0]);
//    CC_SAFE_Free(Internal->buffer[1]);
//    CC_SAFE_Free(Internal->buffer[2]);
//    CC_SAFE_Free(Internal->buffer[3]);
    
    CCFree(Internal);
}

static CCColour CCPixelDataStaticGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    if (!CCPixelDataInsideBounds(Pixels, x, y, z)) return (CCColour){ .type = 0 }; //Or should we have proper sampling behaviour?
    
    void **Buffer = ((CCPixelDataStaticInternal*)Pixels->internal)->buffer;
    const size_t Width = ((CCPixelDataStaticInternal*)Pixels->internal)->width, Height = ((CCPixelDataStaticInternal*)Pixels->internal)->height;
    const CCColourFormat Format = Pixels->format;
    
    return CCColourUnpackFromBuffer(Pixels->format, (const void*[4]){
        Buffer[0] + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex0) * (x + (Width * y) + (Height * z))),
        Buffer[1] + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex1) * (x + (Width * y) + (Height * z))),
        Buffer[2] + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex2) * (x + (Width * y) + (Height * z))),
        Buffer[3] + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex3) * (x + (Width * y) + (Height * z)))
    });
}

static void CCPixelDataStaticGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = ((CCPixelDataStaticInternal*)Pixels->internal)->width;
    if (Height) *Height = ((CCPixelDataStaticInternal*)Pixels->internal)->height;
    if (Depth) *Depth = ((CCPixelDataStaticInternal*)Pixels->internal)->depth;
}

CCPixelData CCPixelDataStaticCreate(CCAllocatorType Allocator, void *Data, CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
{
    return CCPixelDataStaticCreateWithMultiPlanar(Allocator, (void*[4]){ Data, NULL, NULL, NULL }, Format, Width, Height, Depth);
}

CCPixelData CCPixelDataStaticCreateWithMultiPlanar(CCAllocatorType Allocator, void *Data[4], CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
{
    CCPixelData Pixels = CCPixelDataCreate(Allocator, Format, CCPixelDataStatic);
    
    ((CCPixelDataStaticInternal*)Pixels->internal)->buffer[0] = Data[0];
    ((CCPixelDataStaticInternal*)Pixels->internal)->buffer[1] = Data[1];
    ((CCPixelDataStaticInternal*)Pixels->internal)->buffer[2] = Data[2];
    ((CCPixelDataStaticInternal*)Pixels->internal)->buffer[3] = Data[3];
    ((CCPixelDataStaticInternal*)Pixels->internal)->width = Width;
    ((CCPixelDataStaticInternal*)Pixels->internal)->height = Height;
    ((CCPixelDataStaticInternal*)Pixels->internal)->depth = Depth;
    
    return Pixels;
}
