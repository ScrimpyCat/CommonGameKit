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
    CCData buffer[4];
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
    if (Internal->buffer[0]) CCDataDestroy(Internal->buffer[0]);
    if (Internal->buffer[1]) CCDataDestroy(Internal->buffer[1]);
    if (Internal->buffer[2]) CCDataDestroy(Internal->buffer[2]);
    if (Internal->buffer[3]) CCDataDestroy(Internal->buffer[3]);
    
    CCFree(Internal);
}

static CCColour CCPixelDataStaticGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    if (!CCPixelDataInsideBounds(Pixels, x, y, z)) return (CCColour){ .type = 0 }; //Or should we have proper sampling behaviour?
    
    CCData *Buffer = ((CCPixelDataStaticInternal*)Pixels->internal)->buffer;
    const size_t Width = ((CCPixelDataStaticInternal*)Pixels->internal)->width, Height = ((CCPixelDataStaticInternal*)Pixels->internal)->height;
    const CCColourFormat Format = Pixels->format;
    
    return CCColourUnpackFromBuffer(Pixels->format, (const void*[4]){
        (Buffer[0] ? CCDataGetBuffer(Buffer[0]) : NULL) + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex0) * (x + (Width * y) + (Height * z))),
        (Buffer[1] ? CCDataGetBuffer(Buffer[1]) : NULL) + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex1) * (x + (Width * y) + (Height * z))),
        (Buffer[2] ? CCDataGetBuffer(Buffer[2]) : NULL) + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex2) * (x + (Width * y) + (Height * z))),
        (Buffer[3] ? CCDataGetBuffer(Buffer[3]) : NULL) + (CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex3) * (x + (Width * y) + (Height * z)))
    });
}

static void CCPixelDataStaticGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = ((CCPixelDataStaticInternal*)Pixels->internal)->width;
    if (Height) *Height = ((CCPixelDataStaticInternal*)Pixels->internal)->height;
    if (Depth) *Depth = ((CCPixelDataStaticInternal*)Pixels->internal)->depth;
}

CCPixelData CCPixelDataStaticCreate(CCAllocatorType Allocator, CCData Data, CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
{
    return CCPixelDataStaticCreateWithMultiPlanar(Allocator, (CCData[4]){ Data, NULL, NULL, NULL }, Format, Width, Height, Depth);
}

CCPixelData CCPixelDataStaticCreateWithMultiPlanar(CCAllocatorType Allocator, CCData Data[4], CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
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
