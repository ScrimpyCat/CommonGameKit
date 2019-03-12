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

#import "MTLTexture.h"
#import "PixelDataStatic.h"
#import <objc/objc.h>
#import <objc/runtime.h>

const GFXTextureInterface MTLTextureInterface = {
};


CC_CONSTANT_FUNCTION MTLTextureType TextureType(GFXTextureHint Hint)
{
    return (MTLTextureType[3]){
        MTLTextureType1D,
        MTLTextureType2D,
        MTLTextureType3D
    }[(Hint & GFXTextureHintDimensionMask)];
}

static CC_CONSTANT_FUNCTION MTLPixelFormat TextureInternalFormat(CCColourFormat Format)
{
    switch (Format)
    {
        case CCColourFormatRGBA8Unorm:
            return MTLPixelFormatRGBA8Unorm;

        case CCColourFormatRGBA8Unorm_sRGB:
            return MTLPixelFormatRGBA8Unorm_sRGB;
    }

    CCAssertLog(0, "Unsupported format");
}

static void TextureDestroy(MTLGFXTexture Texture)
{
    if (Texture->data) CCPixelDataDestroy(Texture->data);
    if (Texture->isRoot)
    {
        CFRelease((__bridge CFTypeRef)Texture->root.texture);
    }
    
    else GFXTextureDestroy(Texture->sub.parent);
}

static MTLGFXTexture TextureConstructor(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    MTLGFXTexture Texture = CCMalloc(Allocator, sizeof(MTLGFXTextureInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Texture)
    {
        CCMemorySetDestructor(Texture, (CCMemoryDestructorCallback)TextureDestroy);
        
        Texture->isRoot = TRUE;
        Texture->root.hint = Hint;
        Texture->stream = NULL;
        Texture->data = Data;
        Texture->root.format = Format;
        Texture->width = Width;
        Texture->height = Height;
        Texture->depth = Depth;
        
        _Bool FreePixels = FALSE;
        const void *Pixels = (Data) && (Data->format == Format) && (CCPixelDataGetPlaneCount(Data) == 1) ? CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0) : NULL;
        if ((!Pixels) && (Data))
        {
            FreePixels = TRUE;
            
            const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
            CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                           CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                           return nil;
                           );
            
            CCPixelDataGetPackedDataWithFormat(Data, Format, 0, 0, 0, Width, Height, Depth, (void*)Pixels);
        }
        
        MTLTextureDescriptor *TextureDescriptor = [MTLTextureDescriptor new];
        TextureDescriptor.textureType = TextureType(Hint);
        TextureDescriptor.pixelFormat = TextureInternalFormat(Format);
        TextureDescriptor.width = Width;
        TextureDescriptor.height = Height;
        TextureDescriptor.depth = Depth;
        
        Texture->root.texture = (__bridge id<MTLTexture>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->device newTextureWithDescriptor: TextureDescriptor]);
        
        if (Pixels)
        {
            [Texture->root.texture replaceRegion: MTLRegionMake3D(0, 0, 0, Width, Height, Depth) mipmapLevel: 0 withBytes: Pixels bytesPerRow: CCColourFormatSampleSizeForPlanar(Format, CCColourFormatChannelPlanarIndex0) * Width];
            
            if (FreePixels) CCFree((void*)Pixels);
        }
    }
    
    return Texture;
}

static MTLGFXTexture TextureSubConstructor(CCAllocatorType Allocator, GFXTexture Root, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    MTLGFXTexture Texture = CCMalloc(Allocator, sizeof(MTLGFXTextureInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Texture)
    {
        CCMemorySetDestructor(Texture, (CCMemoryDestructorCallback)TextureDestroy);
        
        Texture->isRoot = FALSE;
        Texture->sub.parent = Root;
        Texture->sub.x = X;
        Texture->sub.y = Y;
        Texture->sub.z = Z;
        Texture->stream = NULL;
        Texture->data = Data;
        Texture->width = Width;
        Texture->height = Height;
        Texture->depth = Depth;
        
        Texture->sub.internal.x = X;
        Texture->sub.internal.y = Y;
        Texture->sub.internal.z = Z;
        
        MTLGFXTexture Root = (MTLGFXTexture)Texture->sub.parent;
        for (; !Root->isRoot; Root = (MTLGFXTexture)Root->sub.parent)
        {
            Texture->sub.internal.x += Root->sub.x;
            Texture->sub.internal.y += Root->sub.y;
            Texture->sub.internal.z += Root->sub.z;
        }
        
        Texture->sub.internal.root = (GFXTexture)Root;
        
        if (Data)
        {
            _Bool FreePixels = FALSE;
            const void *Pixels = (Data) && (Data->format == Root->root.format) && (CCPixelDataGetPlaneCount(Data) == 1) ? CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0) : NULL;
            if ((!Pixels) && (Data))
            {
                FreePixels = TRUE;
                
                const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
                CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                               CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                               return nil;
                               );
                
                CCPixelDataGetPackedDataWithFormat(Data, Root->root.format, 0, 0, 0, Width, Height, Depth, (void*)Pixels);
            }
            
            if (Pixels)
            {
                [Root->root.texture replaceRegion: MTLRegionMake3D(0, 0, 0, Width, Height, Depth) mipmapLevel: 0 withBytes: Pixels bytesPerRow: CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0) * Width];
                
                if (FreePixels) CCFree((void*)Pixels);
            }
        }
    }
    
    return Texture;
}

static void TextureDestructor(MTLGFXTexture Texture)
{
    CC_SAFE_Free(Texture);
}
