/*
 *  Copyright (c) 2020 Stefan Johnson
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

#define CC_QUICK_COMPILE
#include "PixelDataComposite.h"

typedef struct {
    CCArray(CCPixelDataCompositeReference) references;
    size_t width, height, depth;
} CCPixelDataCompositeInternal;

static void *CCPixelDataCompositeConstructor(CCAllocatorType Allocator);
static void CCPixelDataCompositeDestructor(CCPixelDataCompositeInternal *Internal);
static CCColour CCPixelDataCompositeGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z);
static void CCPixelDataCompositeGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth);

const CCPixelDataInterface CCPixelDataCompositeInterface = {
    .create = CCPixelDataCompositeConstructor,
    .destroy = (CCPixelDataDestructorCallback)CCPixelDataCompositeDestructor,
    .colour = CCPixelDataCompositeGetColour,
    .optional = {
        .size = CCPixelDataCompositeGetSize
    }
};

const CCPixelDataInterface * const CCPixelDataComposite = &CCPixelDataCompositeInterface;


static void *CCPixelDataCompositeConstructor(CCAllocatorType Allocator)
{
    CCPixelDataCompositeInternal *Internal = CCMalloc(Allocator, sizeof(CCPixelDataCompositeInternal), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Internal)
    {
        *Internal = (CCPixelDataCompositeInternal){
            .references = NULL,
            .width = 0,
            .height = 0,
            .depth = 0
        };
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create composite pixel data: Failed to allocate memory of size (%zu)", sizeof(CCPixelDataCompositeInternal));
    }
    
    return Internal;
}

static void CCPixelDataCompositeDestructor(CCPixelDataCompositeInternal *Internal)
{
    if (Internal->references)
    {
        for (size_t Loop = 0, Count = CCArrayGetCount(Internal->references); Loop < Count; Loop++)
        {
            CCPixelDataCompositeReference *Reference = CCArrayGetElementAtIndex(Internal->references, Loop);
            
            if (Reference->pixel.data[0]) CCPixelDataDestroy(Reference->pixel.data[0]);
            if (Reference->pixel.data[1]) CCPixelDataDestroy(Reference->pixel.data[1]);
            if (Reference->pixel.data[2]) CCPixelDataDestroy(Reference->pixel.data[2]);
            if (Reference->pixel.data[3]) CCPixelDataDestroy(Reference->pixel.data[3]);
        }
        
        CCArrayDestroy(Internal->references);
    }
    
    CCFree(Internal);
}

static CCColour CCPixelDataCompositeGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    if (!CCPixelDataInsideBounds(Pixels, x, y, z)) return (CCColour){ .type = 0 };
    
    if (((CCPixelDataCompositeInternal*)Pixels->internal)->references)
    {
        for (size_t Loop = 0, Count = CCArrayGetCount(((CCPixelDataCompositeInternal*)Pixels->internal)->references); Loop < Count; Loop++)
        {
            const CCPixelDataCompositeReference *Reference = CCArrayGetElementAtIndex(((CCPixelDataCompositeInternal*)Pixels->internal)->references, Loop);
            
            if ((Reference->x <= x) && (Reference->y <= y) && (Reference->z <= z) && (x <= (Reference->x + Reference->pixel.region.width)) && (y <= (Reference->y + Reference->pixel.region.height)) && (z <= (Reference->z + Reference->pixel.region.depth)))
            {
                const CCColourFormat ChannelMask = CCColourFormatChannelIndexMask | CCColourFormatChannelBitSizeMask;
                
                CCColourFormat Format = 0;
                uint8_t Data[64];
                size_t Size = 0;
                for (size_t Loop = 0, ChannelCount = 0; Loop < 4; Loop++)
                {
                    if (Reference->pixel.data[Loop])
                    {
                        for (size_t Loop2 = 0; Loop2 < 4; Loop2++)
                        {
                            const CCColourFormat Offset = CCColourFormatLiteralIndexToChannelOffset(Loop2);
                            const CCColourFormat Channel = (Reference->pixel.data[Loop]->format & (ChannelMask << Offset)) >> Offset;
                            
                            if (Channel) Format |= (Channel | CCColourFormatChannelPlanarIndex0) << CCColourFormatLiteralIndexToChannelOffset(ChannelCount++);
                        }
                        
                        Format |= Reference->pixel.data[Loop]->format & CCColourFormatMask;
                        
                        CCPixelDataGetPackedDataWithFormat(Reference->pixel.data[Loop], Reference->pixel.data[Loop]->format, (x - Reference->x) + Reference->pixel.region.x, (y - Reference->y) + Reference->pixel.region.y, (z - Reference->z) + Reference->pixel.region.z, 1, 1, 1, Data + Size);
                        
                        Size += CCColourFormatSampleSizeForPlanar(Reference->pixel.data[Loop]->format, CCColourFormatChannelPlanarIndex0);
                        Size += CCColourFormatSampleSizeForPlanar(Reference->pixel.data[Loop]->format, CCColourFormatChannelPlanarIndex1);
                        Size += CCColourFormatSampleSizeForPlanar(Reference->pixel.data[Loop]->format, CCColourFormatChannelPlanarIndex2);
                        Size += CCColourFormatSampleSizeForPlanar(Reference->pixel.data[Loop]->format, CCColourFormatChannelPlanarIndex3);
                    }
                }
                
                if (Reference->reinterpret)
                {
                    return CCColourUnpackFromBuffer(Pixels->format, (const void*[4]){ Data, NULL, NULL, NULL });
                }
                
                else
                {
                    CCColour Colour = CCColourUnpackFromBuffer(Format, (const void*[4]){ Data, NULL, NULL, NULL });
                    return CCColourConversion(Colour, Pixels->format);
                }
            }
        }
    }
    
    return CCColourConversion(CCColourCreateFromRGB((CCColourRGB){ 0.0f, 0.0f, 0.0f }), Pixels->format);
}

static void CCPixelDataCompositeGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = ((CCPixelDataCompositeInternal*)Pixels->internal)->width;
    if (Height) *Height = ((CCPixelDataCompositeInternal*)Pixels->internal)->height;
    if (Depth) *Depth = ((CCPixelDataCompositeInternal*)Pixels->internal)->depth;
}

CCPixelData CCPixelDataCompositeCreate(CCAllocatorType Allocator, CCPixelDataCompositeReference *References, size_t Count, CCColourFormat Format, size_t Width, size_t Height, size_t Depth)
{
    CCPixelData Pixels = CCPixelDataCreate(Allocator, Format, CCPixelDataComposite);
    
    ((CCPixelDataCompositeInternal*)Pixels->internal)->references = CCArrayCreate(Allocator, sizeof(CCPixelDataCompositeReference), Count);
    CCArrayAppendElements(((CCPixelDataCompositeInternal*)Pixels->internal)->references, References, Count);
    
    ((CCPixelDataCompositeInternal*)Pixels->internal)->width = Width;
    ((CCPixelDataCompositeInternal*)Pixels->internal)->height = Height;
    ((CCPixelDataCompositeInternal*)Pixels->internal)->depth = Depth;
    
    return Pixels;
}
