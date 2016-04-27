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

#include "PixelData.h"
#include <CommonC/Common.h>

static void CCPixelDataDestructor(CCPixelData Pixels)
{
    Pixels->interface->destroy(Pixels->internal);
    Pixels->internal = NULL;
}

CCPixelData CCPixelDataCreate(CCAllocatorType Allocator, CCColourFormat Format, const CCPixelDataInterface *Interface)
{
    CCAssertLog(Interface, "Interface must not be null");
    
    CCPixelData Pixels = CCMalloc(Allocator, sizeof(CCPixelDataInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Pixels)
    {
        *Pixels = (CCPixelDataInfo){
            .interface = Interface,
            .allocator = Allocator,
            .format = Format,
            .internal = Interface->create(Allocator)
        };
        
        if (!Pixels->internal)
        {
            CC_LOG_ERROR("Failed to create pixel data: Implementation failure (%p)", Interface);
            CCFree(Pixels);
            Pixels = NULL;
        }
        
        else CCMemorySetDestructor(Pixels, (CCMemoryDestructorCallback)CCPixelDataDestructor);
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create pixel data: Failed to allocate memory of size (%zu)", sizeof(CCPixelDataInfo));
    }
    
    return Pixels;
}

void CCPixelDataDestroy(CCPixelData Pixels)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    CCFree(Pixels);
}

CCColour CCPixelDataGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    return Pixels->interface->colour(Pixels, x, y, z);
}

_Bool CCPixelDataHasSize(CCPixelData Pixels)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    return Pixels->interface->optional.size;
}

void CCPixelDataGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    if (Pixels->interface->optional.size) Pixels->interface->optional.size(Pixels, Width, Height, Depth);
}

void CCPixelDataGetPackedData(CCPixelData Pixels, size_t Width, size_t Height, size_t Depth, void *Data)
{
    CCPixelDataGetPackedDataWithFormat(Pixels, Pixels->format, Width, Height, Depth, Data);
}

void CCPixelDataGetPackedDataWithFormat(CCPixelData Pixels, CCColourFormat Type, size_t Width, size_t Height, size_t Depth, void *Data)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    if ((!Pixels->interface->optional.packedData) || (!Pixels->interface->optional.packedData(Pixels, Type, Width, Height, Depth, Data)))
    {
        for (size_t LoopZ = 0; LoopZ < Depth; LoopZ++)
        {
            for (size_t LoopY = 0; LoopY < Height; LoopY++)
            {
                for (size_t LoopX = 0; LoopX < Width; LoopX++)
                {
                    CCColour Pixel = CCColourConversion(CCPixelDataGetColour(Pixels, LoopX, LoopY, LoopZ), Type);
                    
                    Data += CCColourPackIntoBuffer(Pixel, Data);
                }
            }
        }
    }
}

_Bool CCPixelDataInsideBounds(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    if (CCPixelDataHasSize(Pixels))
    {
        size_t Width, Height, Depth;
        CCPixelDataGetSize(Pixels, &Width, &Height, &Depth);
        
        return x < Width
            && y < Height
            && z < Depth;
    }
    
    return TRUE;
}

unsigned int CCPixelDataGetPlaneCount(CCPixelData Pixels)
{
    CCAssertLog(Pixels, "Pixel data must not be null");
    
    return CCColourFormatPlaneCount(Pixels->format);
}
