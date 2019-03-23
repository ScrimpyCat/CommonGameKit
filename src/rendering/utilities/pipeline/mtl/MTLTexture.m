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

static MTLGFXTexture TextureConstructor(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static MTLGFXTexture TextureSubConstructor(CCAllocatorType Allocator, GFXTexture Root, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static void TextureDestructor(MTLGFXTexture Texture);
static GFXTexture TextureGetParent(MTLGFXTexture Texture);
static GFXTextureHint TextureGetHint(MTLGFXTexture Texture);
static CCPixelData TextureGetData(MTLGFXTexture Texture);
static void TextureGetOffset(MTLGFXTexture Texture, size_t *X, size_t *Y, size_t *Z);
static void TextureGetInternalOffset(MTLGFXTexture Texture, size_t *X, size_t *Y, size_t *Z);
static void TextureGetSize(MTLGFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static void TextureGetInternalSize(MTLGFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static void TextureSetFilterMode(MTLGFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);
static void TextureSetAddressMode(MTLGFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);
static CCPixelData TextureRead(MTLGFXTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);
static void TextureWrite(MTLGFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static void *TextureGetStreamData(MTLGFXTexture Texture);
static void TextureSetStreamData(MTLGFXTexture Texture, void *Stream);


const GFXTextureInterface MTLTextureInterface = {
    .create = (GFXTextureConstructorCallback)TextureConstructor,
    .createSub = (GFXTextureSubConstructorCallback)TextureSubConstructor,
    .destroy = (GFXTextureDestructorCallback)TextureDestructor,
    .parent = (GFXTextureGetParentCallback)TextureGetParent,
    .hints = (GFXTextureGetHintCallback)TextureGetHint,
    .data = (GFXTextureGetDataCallback)TextureGetData,
    .offset = (GFXTextureGetOffsetCallback)TextureGetOffset,
    .size = (GFXTextureGetSizeCallback)TextureGetSize,
    .setFilterMode = (GFXTextureSetFilterModeCallback)TextureSetFilterMode,
    .setAddressMode = (GFXTextureSetAddressModeCallback)TextureSetAddressMode,
    .read = (GFXTextureReadCallback)TextureRead,
    .write = (GFXTextureWriteCallback)TextureWrite,
    .getStream = (GFXTextureGetStreamDataCallback)TextureGetStreamData,
    .setStream = (GFXTextureSetStreamDataCallback)TextureSetStreamData,
    .optional = {
        .internalOffset = (GFXTextureGetInternalOffsetCallback)TextureGetInternalOffset,
        .internalSize = (GFXTextureGetInternalSizeCallback)TextureGetInternalSize
    }
};


static CC_CONSTANT_FUNCTION MTLTextureType TextureType(GFXTextureHint Hint)
{
    return (MTLTextureType[3]){
        MTLTextureType1D,
        MTLTextureType2D,
        MTLTextureType3D
    }[(Hint & GFXTextureHintDimensionMask)];
}

static CC_CONSTANT_FUNCTION GFXTextureHint TextureTypeReverse(MTLTextureType Type)
{
    switch (Type)
    {
        case MTLTextureType1D:
            return GFXTextureHintDimension1D;
            
        case MTLTextureType2D:
            return GFXTextureHintDimension2D;
            
        case MTLTextureType3D:
            return GFXTextureHintDimension3D;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported type");
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

static CC_CONSTANT_FUNCTION CCColourFormat TextureInternalFormatReverse(MTLPixelFormat Format)
{
    switch (Format)
    {
        case MTLPixelFormatRGBA8Unorm:
            return CCColourFormatRGBA8Unorm;
            
        case MTLPixelFormatRGBA8Unorm_sRGB:
            return CCColourFormatRGBA8Unorm_sRGB;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported format");
}

static CC_CONSTANT_FUNCTION MTLTextureUsage TextureUsage(GFXTextureHint Hint)
{
    MTLTextureUsage Usage = (Hint & GFXTextureHintUsageShaderRead ? MTLTextureUsageShaderRead : 0) |
                            (Hint & GFXTextureHintUsageShaderWrite ? MTLTextureUsageShaderWrite : 0) |
                            (Hint & GFXTextureHintUsageRenderTarget ? MTLTextureUsageRenderTarget : 0) |
                            (Hint & GFXTextureHintUsageShaderRead ? MTLTextureUsageShaderRead : 0);
    
    return Usage ? Usage : MTLTextureUsageUnknown;
}

static CC_CONSTANT_FUNCTION GFXTextureHint TextureUsageReverse(MTLTextureUsage Usage)
{
    GFXTextureHint Hint = (Usage & MTLTextureUsageShaderRead ? GFXTextureHintUsageShaderRead : 0) |
                          (Usage & MTLTextureUsageShaderWrite ? GFXTextureHintUsageShaderWrite : 0) |
                          (Usage & MTLTextureUsageRenderTarget ? GFXTextureHintUsageRenderTarget : 0) |
                          (Usage & MTLTextureUsageShaderRead ? GFXTextureHintUsageShaderRead : 0);
    
    return Hint ? Hint : GFXTextureHintUsageGeneral;
}

static CC_CONSTANT_FUNCTION MTLStorageMode TextureStorage(GFXTextureHint Hint)
{
#if CC_PLATFORM_IOS
#define MTL_GFX_STORAGE_DEFAULT MTLStorageModeShared
#define MTL_GFX_STORAGE_WRITE_ONLY MTLStorageModeMemoryless
#elif CC_PLATFORM_OS_X
#define MTL_GFX_STORAGE_DEFAULT MTLStorageModeManaged
#define MTL_GFX_STORAGE_WRITE_ONLY MTLStorageModePrivate
#endif
    
    static const MTLStorageMode Storage[2][2][2][2] = {
        // GFXTextureHintCPUReadNone
        {
            // GFXTextureHintCPUWriteNone
            {
                // GFXTextureHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_WRITE_ONLY  // GFXTextureHintGPUWrite*
                },
                // GFXTextureHintGPURead*
                {
                    MTLStorageModePrivate,      // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_WRITE_ONLY  // GFXTextureHintGPUWrite*
                }
            },
            // GFXTextureHintCPUWrite*
            {
                // GFXTextureHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                },
                // GFXTextureHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                }
            }
        },
        // GFXTextureHintCPURead*
        {
            // GFXTextureHintCPUWriteNone
            {
                // GFXTextureHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                },
                // GFXTextureHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                }
            },
            // GFXTextureHintCPUWrite*
            {
                // GFXTextureHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                },
                // GFXTextureHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXTextureHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXTextureHintGPUWrite*
                }
            }
        }
    };
    
    const _Bool CPUWrite = (Hint >> GFXTextureHintCPUWrite) & GFXTextureHintAccessMask;
    const _Bool GPUWrite = (Hint >> GFXTextureHintGPUWrite) & GFXTextureHintAccessMask;
    const _Bool CPURead = (Hint >> GFXTextureHintCPURead) & GFXTextureHintAccessMask;
    const _Bool GPURead = (Hint >> GFXTextureHintGPURead) & GFXTextureHintAccessMask;
    
    const MTLStorageMode Mode = Storage[CPURead][CPUWrite][GPURead][GPUWrite];
    
#if CC_PLATFORM_IOS
    if (Mode == MTLStorageModeMemoryless)
    {
        if ((Hint & GFXTextureHintUsageMask) != GFXTextureHintUsageRenderTarget) return MTLStorageModePrivate;
    }
#endif
    
    return Mode;
}

static CC_CONSTANT_FUNCTION BOOL TextureGPUOptimized(GFXTextureHint Hint)
{
    const size_t CPUUsage = ((Hint >> GFXTextureHintCPUWrite) & GFXTextureHintAccessMask) + ((Hint >> GFXTextureHintCPURead) & GFXTextureHintAccessMask);
    const size_t GPUUsage = ((Hint >> GFXTextureHintGPUWrite) & GFXTextureHintAccessMask) + ((Hint >> GFXTextureHintGPURead) & GFXTextureHintAccessMask);
    
    return (CPUUsage <= GFXTextureHintAccessOnce) && (GPUUsage > GFXTextureHintAccessOnce);
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

GFXTexture MTLGFXTextureCreate(CCAllocatorType Allocator, id <MTLTexture>Data)
{
    MTLGFXTexture Texture = CCMalloc(Allocator, sizeof(MTLGFXTextureInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Texture)
    {
        CCMemorySetDestructor(Texture, (CCMemoryDestructorCallback)TextureDestroy);
        
        Texture->isRoot = TRUE;
        Texture->root.hint = TextureTypeReverse(Data.textureType) | TextureUsageReverse(Data.usage) | GFXTextureHintAccessUnknown;
        Texture->stream = NULL;
        Texture->data = NULL;
        Texture->root.format = TextureInternalFormatReverse(Data.pixelFormat);
        Texture->width = Data.width;
        Texture->height = Data.height;
        Texture->depth = Data.depth;
        
        Texture->root.texture = (__bridge id<MTLTexture>)((__bridge_retained CFTypeRef)Data);
    }
    
    return (GFXTexture)Texture;
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
        TextureDescriptor.usage = TextureUsage(Hint);
        TextureDescriptor.storageMode = TextureStorage(Hint);
        
#if CC_PLATFORM_APPLE_VERSION_MIN_REQUIRED(CC_PLATFORM_MAC_10_14, CC_PLATFORM_IOS_12_0)
        if (((MTLInternal*)MTLGFX->internal)->support->MTLTextureDescriptor.allowGPUOptimizedContents) TextureDescriptor.allowGPUOptimizedContents = TextureGPUOptimized(Hint);
#endif
        
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
                [Root->root.texture replaceRegion: MTLRegionMake3D(Texture->sub.internal.x, Texture->sub.internal.y, Texture->sub.internal.z, Width, Height, Depth) mipmapLevel: 0 withBytes: Pixels bytesPerRow: CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0) * Width];
                
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

static GFXTexture TextureGetParent(MTLGFXTexture Texture)
{
    return Texture->isRoot ? NULL : Texture->sub.parent;
}

static GFXTextureHint TextureGetHint(MTLGFXTexture Texture)
{
    return Texture->isRoot ? Texture->root.hint : ((MTLGFXTexture)Texture->sub.internal.root)->root.hint;
}

static CCPixelData TextureGetData(MTLGFXTexture Texture)
{
    return Texture->data;
}

static void TextureGetOffset(MTLGFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    if (Texture->isRoot)
    {
        if (X) *X = 0;
        if (Y) *Y = 0;
        if (Z) *Z = 0;
    }

    else
    {
        if (X) *X = Texture->sub.x;
        if (Y) *Y = Texture->sub.y;
        if (Z) *Z = Texture->sub.z;
    }
}

static void TextureGetInternalOffset(MTLGFXTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    if (Texture->isRoot)
    {
        if (X) *X = 0;
        if (Y) *Y = 0;
        if (Z) *Z = 0;
    }

    else
    {
        if (X) *X = Texture->sub.internal.x;
        if (Y) *Y = Texture->sub.internal.y;
        if (Z) *Z = Texture->sub.internal.z;
    }
}

static void TextureGetSize(MTLGFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = Texture->width;
    if (Height) *Height = Texture->height;
    if (Depth) *Depth = Texture->depth;
}

static void TextureGetInternalSize(MTLGFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Texture->isRoot)
    {
        if (Width) *Width = Texture->width;
        if (Height) *Height = Texture->height;
        if (Depth) *Depth = Texture->depth;
    }

    else
    {
        if (Width) *Width = ((MTLGFXTexture)Texture->sub.internal.root)->width;
        if (Height) *Height = ((MTLGFXTexture)Texture->sub.internal.root)->height;
        if (Depth) *Depth = ((MTLGFXTexture)Texture->sub.internal.root)->depth;
    }
}

static void TextureSetFilterMode(MTLGFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode)
{
    if (Texture->isRoot)
    {
        Texture->root.hint = (Texture->root.hint & ~(GFXTextureHintFilterModeMask << FilterType));
    }

    else CCAssertLog(0, "Cannot set a sub texture's filtering mode");
}

static void TextureSetAddressMode(MTLGFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode)
{
    if (Texture->isRoot)
    {
        Texture->root.hint = (Texture->root.hint & ~(GFXTextureHintAddressModeMask << Coordinate));
    }
    
    else CCAssertLog(0, "Cannot set a sub texture's filtering mode");
}

static CCPixelData TextureRead(MTLGFXTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth)
{
    MTLGFXTexture Root = Texture->isRoot ? Texture : (MTLGFXTexture)Texture->sub.internal.root;
    
    void *Pixels;
    const size_t InternalSampleSize = CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0);
    CC_SAFE_Malloc(Pixels, InternalSampleSize * Root->width * Root->height * Root->depth,
                   CC_LOG_ERROR("Failed to allocate memory for texture receive. Allocation size (%zu)", InternalSampleSize * Width * Height * Depth);
                   return NULL;
                   );
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    TextureGetInternalOffset(Texture, &RealX, &RealY, &RealZ);
    
    [Root->root.texture getBytes: Pixels bytesPerRow: CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0) * Width fromRegion: MTLRegionMake3D(RealX + X, RealY + Y, RealZ + Z, Width, Height, Depth) mipmapLevel: 0];
    
    CCPixelData CopiedData = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataBufferHintFree, InternalSampleSize * Root->width * Root->height * Root->depth, Pixels, NULL, NULL), Root->root.format, Root->width, Root->height, Root->depth);
    
    const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0);
    if (!(Pixels = CCMalloc(Allocator, SampleSize * Width * Height * Depth, NULL, CC_DEFAULT_ERROR_CALLBACK)))
    {
        CC_LOG_ERROR("Failed to allocate memory for texture receive. Allocation size (%zu)", SampleSize * Width * Height * Depth);
        CCPixelDataDestroy(CopiedData);
        return NULL;
    }
    
    CCPixelDataGetPackedDataWithFormat(CopiedData, Format, 0, 0, 0, Width, Height, Depth, Pixels);
    CCPixelDataDestroy(CopiedData);
    
    return CCPixelDataStaticCreate(Allocator, CCDataBufferCreate(Allocator, CCDataBufferHintFree, SampleSize * Width * Height * Depth, Pixels, NULL, NULL), Format, Width, Height, Depth);
}

static void TextureWrite(MTLGFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    MTLGFXTexture Root = Texture->isRoot ? Texture : (MTLGFXTexture)Texture->sub.internal.root;
    
    _Bool FreePixels = FALSE;
    const void *Pixels = (Data) && (Data->format == Root->root.format) && (CCPixelDataGetPlaneCount(Data) == 1) ? CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0) : NULL;
    if ((!Pixels) && (Data))
    {
        FreePixels = TRUE;
        
        const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
        CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                       CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                       return;
                       );
        
        CCPixelDataGetPackedDataWithFormat(Data, Root->root.format, 0, 0, 0, Width, Height, Depth, (void*)Pixels);
    }
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    TextureGetInternalOffset(Texture, &RealX, &RealY, &RealZ);
    
    [Root->root.texture replaceRegion: MTLRegionMake3D(RealX + X, RealY + Y, RealZ + Z, Width, Height, Depth) mipmapLevel: 0 withBytes: Pixels bytesPerRow: CCColourFormatSampleSizeForPlanar(Root->root.format, CCColourFormatChannelPlanarIndex0) * Width];
    
    if (FreePixels) CCFree((void*)Pixels);
}

static void *TextureGetStreamData(MTLGFXTexture Texture)
{
    return Texture->stream;
}

static void TextureSetStreamData(MTLGFXTexture Texture, void *Stream)
{
    Texture->stream = Stream;
}
