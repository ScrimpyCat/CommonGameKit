/*
 *  Copyright (c) 2016 Stefan Johnson
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
#include "PixelDataFile.h"
#include "PixelDataStatic.h"
#include <png.h>

typedef struct {
    CCPixelData data;
    FSPath path;
} CCPixelDataFileInternal;

static void *CCPixelDataFileConstructor(CCAllocatorType Allocator);
static void CCPixelDataFileDestructor(CCPixelDataFileInternal *Internal);
static CCColour CCPixelDataFileGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z);
static void CCPixelDataFileGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth);
static _Bool CCPixelDataFileGetPackedData(CCPixelData Pixels, CCColourFormat Type, size_t x, size_t y, size_t z, size_t Width, size_t Height, size_t Depth, void *Data);
static const void *CCPixelDataFileGetBuffer(CCPixelData Pixels, CCColourFormat PlanarIndex);

const CCPixelDataInterface CCPixelDataFileInterface = {
    .create = CCPixelDataFileConstructor,
    .destroy = (CCPixelDataDestructorCallback)CCPixelDataFileDestructor,
    .colour = CCPixelDataFileGetColour,
    .optional = {
        .size = CCPixelDataFileGetSize,
        .packedData = CCPixelDataFileGetPackedData,
        .buffer = CCPixelDataFileGetBuffer
    }
};

const CCPixelDataInterface * const CCPixelDataFile = &CCPixelDataFileInterface;


static void *CCPixelDataFileConstructor(CCAllocatorType Allocator)
{
    CCPixelDataFileInternal *Internal = CCMalloc(Allocator, sizeof(CCPixelDataFileInternal), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Internal)
    {
        *Internal = (CCPixelDataFileInternal){
            .data = NULL,
            .path = NULL
        };
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create file pixel data: Failed to allocate memory of size (%zu)", sizeof(CCPixelDataFileInternal));
    }
    
    return Internal;
}

static void CCPixelDataFileDestructor(CCPixelDataFileInternal *Internal)
{
    CCPixelDataDestroy(Internal->data);
    FSPathDestroy(Internal->path);
    
    CCFree(Internal);
}

static CCColour CCPixelDataFileGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z)
{
    return CCPixelDataGetColour(((CCPixelDataFileInternal*)Pixels->internal)->data, x, y, z);
}

static void CCPixelDataFileGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth)
{
    CCPixelDataGetSize(((CCPixelDataFileInternal*)Pixels->internal)->data, Width, Height, Depth);
}

static _Bool CCPixelDataFileGetPackedData(CCPixelData Pixels, CCColourFormat Type, size_t x, size_t y, size_t z, size_t Width, size_t Height, size_t Depth, void *Data)
{
    CCPixelDataGetPackedDataWithFormat(((CCPixelDataFileInternal*)Pixels->internal)->data, Type, x, y, z, Width, Height, Depth, Data);
    return TRUE;
}

static const void *CCPixelDataFileGetBuffer(CCPixelData Pixels, CCColourFormat PlanarIndex)
{
    return CCPixelDataGetBuffer(((CCPixelDataFileInternal*)Pixels->internal)->data, PlanarIndex);
}

CCPixelData CCPixelDataFileCreate(CCAllocatorType Allocator, FSPath Path)
{
    CCAssertLog(Path, "Path must not be null");
    
    CCPixelData Data = NULL;
    FSHandle Handle;
    if (FSHandleOpen(Path, FSHandleTypeRead, &Handle) == FSOperationSuccess)
    {
        size_t Size = FSManagerGetSize(Path);
        uint8_t *ImageData;
        CC_SAFE_Malloc(ImageData, sizeof(uint8_t) * Size,
                       CC_LOG_ERROR("Failed to read image data due to allocation failure. Allocation size (%zu)", sizeof(uint8_t) * Size);
                       return NULL;
                       );
        
        FSHandleRead(Handle, &Size, ImageData, FSBehaviourDefault);
        FSHandleClose(Handle);
        
        if (png_check_sig(ImageData, Size < 8 ? Size : 8))
        {
            png_image Image = { .version = PNG_IMAGE_VERSION };
            if (png_image_begin_read_from_memory(&Image, ImageData, Size))
            {
                if (Image.format != PNG_FORMAT_RGB) Image.format = PNG_FORMAT_RGBA;
                
                const size_t BufferSize = PNG_IMAGE_SIZE(Image);
                uint8_t *Buffer;
                CC_SAFE_Malloc(Buffer, BufferSize,
                               CC_LOG_ERROR("Failed to create buffer due to allocation failure. Allocation size (%zu)", BufferSize);
                               png_image_free(&Image);
                               CC_SAFE_Free(ImageData);
                               return NULL;
                               );
                
                if (png_image_finish_read(&Image, NULL, Buffer, -PNG_IMAGE_ROW_STRIDE(Image), NULL))
                {
                    CCColourFormat Format;
                    switch (Image.format)
                    {
                        case PNG_FORMAT_RGB:
                            Format = CCColourFormatRGB8Unorm_sRGB;
                            break;
                            
                        case PNG_FORMAT_RGBA:
                            Format = CCColourFormatRGBA8Unorm_sRGB;
                            break;
                            
                        default:
                            CCAssertLog(0, "Unsupported PNG format");
                            break;
                    }
                    
                    Data = CCPixelDataStaticCreate(Allocator, CCDataBufferCreate(Allocator, CCDataBufferHintFree | CCDataHintRead, BufferSize, Buffer, NULL, NULL), Format, Image.width, Image.height, 1);
                }
                
                else CC_SAFE_Free(Buffer);
                
                png_image_free(&Image);
            }
            
            if (!Data) CC_LOG_ERROR("Failed to load PNG image: %s", FSPathGetPathString(Path));
            
            CC_SAFE_Free(ImageData);
        }
        
        else CC_LOG_ERROR("Does not support image file: %s", FSPathGetPathString(Path));
    }
    
    else CC_LOG_ERROR("Failed to load image due to error opening file: %s", FSPathGetFullPathString(Path));
    
    
    CCPixelData Pixels = NULL;
    if (Data)
    {
        Pixels = CCPixelDataCreate(Allocator, Data->format, CCPixelDataFile);
        
        ((CCPixelDataFileInternal*)Pixels->internal)->data = Data;
        ((CCPixelDataFileInternal*)Pixels->internal)->path = FSPathCopy(Path);
    }
    
    return Pixels;
}

void CCPixelDataFileWrite(CCPixelData Pixels, size_t x, size_t y, size_t z, size_t Width, size_t Height, size_t Depth, FSPath Path)
{
    CCAssertLog((png_uint_32)Width == Width, "Width exceeds max png width size");
    CCAssertLog((png_uint_32)Height == Height, "Height exceeds max png height size");
    CCAssertLog(Depth == 1, "3D textures are currently unsupported");
    
    png_image Image = {
        .version = PNG_IMAGE_VERSION,
        .width = (png_uint_32)Width,
        .height = (png_uint_32)Height
    };
    
    CCColourFormat Format;
    if ((Pixels->format & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_sRGB)
    {
        Format = CCColourFormatRGB8Unorm_sRGB;
        Image.flags = 0;
    }
    
    else
    {
        Format = CCColourFormatRGB8Unorm;
        Image.flags = PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB;
    }
    
    if (CCColourFormatHasChannel(Pixels->format, CCColourFormatChannelAlpha))
    {
        Format |= CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelAlpha, 8, CCColourFormatChannelOffset3);
        Image.format = PNG_FORMAT_RGBA;
    }
    
    else
    {
        Image.format = PNG_FORMAT_RGB;
    }
    
    const size_t RowSize = CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex0) * Width;
    size_t Size = RowSize * Height;
    void *Buffer;
    CC_SAFE_Malloc(Buffer, Size,
                   CC_LOG_ERROR("Failed to write image data due to allocation failure. Allocation size (%zu)", Size);
                   return;
                   );
    
    for (size_t Y = 0; Y < Height; Y++)
    {
        CCPixelDataGetPackedDataWithFormat(Pixels, Format, x, (Height - Y) - 1, z, Width, 1, 1, Buffer + (RowSize * Y));
    }
    
    uint8_t *ImageData;
    if (Size < PNG_IMAGE_DATA_SIZE(Image)) Size = PNG_IMAGE_DATA_SIZE(Image);
    CC_SAFE_Malloc(ImageData, Size,
                   CC_LOG_ERROR("Failed to write image data due to allocation failure. Allocation size (%zu)", Size);
                   CC_SAFE_Free(Buffer);
                   return;
                   );
    
    size_t SizeRequired = Size;
    if (!png_image_write_to_memory(&Image, ImageData, &SizeRequired, 0, Buffer, 0, NULL))
    {
        _Bool Success = FALSE;
        if (SizeRequired > Size)
        {
            CC_SAFE_Realloc(ImageData, SizeRequired,
                            CC_LOG_ERROR("Failed to write image data due to allocation failure. Allocation size (%zu)", Size);
                            CC_SAFE_Free(Buffer);
                            CC_SAFE_Free(ImageData);
                            return;
                            );
            
            Success = png_image_write_to_memory(&Image, ImageData, &SizeRequired, 0, Buffer, 0, NULL);
        }
        
        if (!Success)
        {
            CC_LOG_ERROR("Failed to write image data due to png_image_write_to_memory failure");
            
            CC_SAFE_Free(Buffer);
            CC_SAFE_Free(ImageData);
            
            return;
        }
    }
    
    if ((Format & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_RGB)
    {
        uint8_t *Chunks = ImageData + 8;
        
        while (Chunks < ImageData + SizeRequired)
        {
#if CC_HARDWARE_ENDIAN_LITTLE
            const size_t Length = (Chunks[0] << 24) | (Chunks[1] << 16) | (Chunks[2] << 8) | Chunks[3];
#elif CC_HARDWARE_ENDIAN_BIG
            const size_t Length = *(uint32_t*)Chunks;
#endif
            
#if CC_HARDWARE_ENDIAN_LITTLE
            if (*(uint32_t*)(Chunks + 4) == 'AMAg')
#elif CC_HARDWARE_ENDIAN_BIG
            if (*(uint32_t*)(Chunks + 4) == 'gAMA')
#endif
            {
                *(uint32_t*)(Chunks + 8) = 0;
#if CC_HARDWARE_ENDIAN_LITTLE
                *(uint32_t*)(Chunks + 12) = 0x4d60258b;
#elif CC_HARDWARE_ENDIAN_BIG
                *(uint32_t*)(Chunks + 12) = 0x8b25604d;
#endif
                break;
            }
            
            Chunks += Length + 12;
        }
    }
    
    if (FSManagerCreate(Path, TRUE) == FSOperationSuccess)
    {
        FSHandle Handle;
        if (FSHandleOpen(Path, FSHandleTypeWrite, &Handle) != FSOperationSuccess)
        {
            CC_LOG_ERROR("Failed to write image to file at path: %s", FSPathGetFullPathString(Path));
            
            CC_SAFE_Free(Buffer);
            CC_SAFE_Free(ImageData);
            
            return;
        }
        
        FSHandleRemove(Handle, SIZE_MAX, FSBehaviourDefault);
        FSHandleWrite(Handle, SizeRequired, ImageData, FSBehaviourDefault);
        FSHandleClose(Handle);
    }
    
    else CC_LOG_ERROR("Failed to write image to file at path: %s", FSPathGetFullPathString(Path));
    
    CC_SAFE_Free(Buffer);
    CC_SAFE_Free(ImageData);
    
    return;
}
