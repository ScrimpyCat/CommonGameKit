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

#include "GLTexture.h"
#include "PixelDataStatic.h"

static GLTexture GLTextureConstructor(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static GLTexture GLTextureSubConstructor(CCAllocatorType Allocator, GFXTexture Root, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static void GLTextureDestructor(GLTexture Texture);
static GFXTexture GLTextureGetParent(GLTexture Texture);
static GFXTextureHint GLTextureGetHint(GLTexture Texture);
static CCPixelData GLTextureGetData(GLTexture Texture);
static void GLTextureGetOffset(GLTexture Texture, size_t *X, size_t *Y, size_t *Z);
static void GLTextureGetInternalOffset(GLTexture Texture, size_t *X, size_t *Y, size_t *Z);
static void GLTextureGetSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static void GLTextureGetInternalSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static void GLTextureSetFilterMode(GLTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);
static void GLTextureSetAddressMode(GLTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);
static CCPixelData GLTextureRead(GLTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);
static void GLTextureWrite(GLTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static void *GLTextureGetStreamData(GLTexture Texture);
static void GLTextureSetStreamData(GLTexture Texture, void *Stream);


const GFXTextureInterface GLTextureInterface = {
    .create = (GFXTextureConstructorCallback)GLTextureConstructor,
    .createSub = (GFXTextureSubConstructorCallback)GLTextureSubConstructor,
    .destroy = (GFXTextureDestructorCallback)GLTextureDestructor,
    .parent = (GFXTextureGetParentCallback)GLTextureGetParent,
    .hints = (GFXTextureGetHintCallback)GLTextureGetHint,
    .data = (GFXTextureGetDataCallback)GLTextureGetData,
    .offset = (GFXTextureGetOffsetCallback)GLTextureGetOffset,
    .size = (GFXTextureGetSizeCallback)GLTextureGetSize,
    .setFilterMode = (GFXTextureSetFilterModeCallback)GLTextureSetFilterMode,
    .setAddressMode = (GFXTextureSetAddressModeCallback)GLTextureSetAddressMode,
    .read = (GFXTextureReadCallback)GLTextureRead,
    .write = (GFXTextureWriteCallback)GLTextureWrite,
    .getStream = (GFXTextureGetStreamDataCallback)GLTextureGetStreamData,
    .setStream = (GFXTextureSetStreamDataCallback)GLTextureSetStreamData,
    .optional = {
        //TODO: Add invalidation .invalidate = (GFXTextureInvalidateCallback)GLTextureInvalidate
        .internalOffset = (GFXTextureGetInternalOffsetCallback)GLTextureGetInternalOffset,
        .internalSize = (GFXTextureGetInternalSizeCallback)GLTextureGetInternalSize
    }
};


CC_CONSTANT_FUNCTION GLenum GLTextureTarget(GFXTextureHint Hint)
{
    return (GLenum[3]){
        GL_TEXTURE_1D,
        GL_TEXTURE_2D,
        GL_TEXTURE_3D
    }[(Hint & GFXTextureHintDimensionMask)];
}

static inline CC_CONSTANT_FUNCTION GLenum GLTextureFilter(GFXTextureHint Hint, GFXTextureHint FilterType)
{
    return (GLenum[2]){
        GL_NEAREST,
        GL_LINEAR
    }[(Hint >> FilterType) & GFXTextureHintFilterModeMask];
}

static inline CC_CONSTANT_FUNCTION GLenum GLTextureAddressMode(GFXTextureHint Hint, GFXTextureHint Coordinate)
{
    return (GLenum[3]){
        GL_CLAMP_TO_EDGE,
        GL_REPEAT,
        GL_MIRRORED_REPEAT
    }[(Hint >> Coordinate) & GFXTextureHintAddressModeMask];
}

static CC_CONSTANT_FUNCTION GLenum GLTextureInputFormat(CCColourFormat Format)
{
    switch (Format)
    {
        case CCColourFormatRGB8Unorm:
        case CCColourFormatRGB8Unorm_sRGB:
            return GL_RGB;
            
        case CCColourFormatRGBA8Unorm:
        case CCColourFormatRGBA8Unorm_sRGB:
            return GL_RGBA;
    }
    
    CCAssertLog(0, "Unsupported format");
}

static CC_CONSTANT_FUNCTION GLenum GLTextureInputFormatType(CCColourFormat Format)
{
    switch (Format)
    {
        case CCColourFormatRGB8Unorm:
        case CCColourFormatRGBA8Unorm:
        case CCColourFormatRGB8Unorm_sRGB:
        case CCColourFormatRGBA8Unorm_sRGB:
            return GL_UNSIGNED_BYTE;
    }
    
    CCAssertLog(0, "Unsupported format");
}

static CC_CONSTANT_FUNCTION GLenum GLTextureInternalFormat(CCColourFormat Format)
{
    switch (Format)
    {
        case CCColourFormatRGB8Unorm:
            return GL_RGB8;
            
        case CCColourFormatRGBA8Unorm:
            return GL_RGBA8;
            
        case CCColourFormatRGB8Unorm_sRGB:
            return GL_SRGB8;
            
        case CCColourFormatRGBA8Unorm_sRGB:
            return GL_SRGB8_ALPHA8;
    }
    
    CCAssertLog(0, "Unsupported format");
}

static void GLTextureDestroy(GLTexture Texture)
{
    if (Texture->data) CCPixelDataDestroy(Texture->data);
    if (Texture->isRoot)
    {
        glDeleteTextures(1, &Texture->root.texture); CC_GL_CHECK();
        
#if CC_GL_STATE_TEXTURE
        GLuint *BoundTextureState = NULL;
        
        switch (GLTextureTarget(Texture->root.hint))
        {
#if CC_GL_STATE_TEXTURE_1D
            case GL_TEXTURE_1D:
                BoundTextureState = &CC_GL_CURRENT_STATE->bindTexture[0]._GL_TEXTURE_1D;
                break;
#endif
            
#if CC_GL_STATE_TEXTURE_2D
            case GL_TEXTURE_2D:
                BoundTextureState = &CC_GL_CURRENT_STATE->bindTexture[0]._GL_TEXTURE_2D;
                break;
#endif
            
#if CC_GL_STATE_TEXTURE_3D
            case GL_TEXTURE_3D:
                BoundTextureState = &CC_GL_CURRENT_STATE->bindTexture[0]._GL_TEXTURE_3D;
                break;
#endif
            
            default:
                return;
        }
        
#if CC_GL_STATE_TEXTURE_MAX
        const GLint Count = CC_GL_STATE_TEXTURE_MAX;
#else
        GLint Count;
        CC_GL_VERSION_ACTIVE(1_1, 1_5, 1_0, 1_1, glGetIntegerv(GL_MAX_TEXTURE_UNITS, &Count); CC_GL_CHECK());
        CC_GL_VERSION_ACTIVE(2_0, NA, 2_0, NA, glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Count); CC_GL_CHECK());
#endif
        
        for (int Loop = 0; Loop < Count; Loop++)
        {
            GLuint *BoundTexture = (GLuint*)((typeof(*CC_GL_CURRENT_STATE->bindTexture)*)BoundTextureState + Loop);
            if (*BoundTexture == Texture->root.texture) *BoundTexture = 0;
        }
#endif
    }
    
    else GFXTextureDestroy(Texture->sub.parent);
}

static GLTexture GLTextureConstructor(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    GLTexture Texture = CCMalloc(Allocator, sizeof(GLTextureInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Texture)
    {
        CCMemorySetDestructor(Texture, (CCMemoryDestructorCallback)GLTextureDestroy);
        
        Texture->isRoot = TRUE;
        Texture->root.hint = Hint;
        Texture->stream = NULL;
        Texture->data = Data;
        Texture->root.format = Format;
        Texture->width = Width;
        Texture->height = Height;
        Texture->depth = Depth;
        
        const GLenum Target = GLTextureTarget(Hint);
        
        glGenTextures(1, &Texture->root.texture); CC_GL_CHECK();
        CC_GL_BIND_TEXTURE_TARGET(Target, Texture->root.texture);
        
        glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GLTextureFilter(Hint, GFXTextureHintFilterMin)); CC_GL_CHECK();
        glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GLTextureFilter(Hint, GFXTextureHintFilterMag)); CC_GL_CHECK();
        
        switch (Target)
        {
            case GL_TEXTURE_3D:
                glTexParameterf(Target, GL_TEXTURE_WRAP_R, GLTextureAddressMode(Hint, GFXTextureHintAddress_R)); CC_GL_CHECK();
            case GL_TEXTURE_2D:
                glTexParameterf(Target, GL_TEXTURE_WRAP_T, GLTextureAddressMode(Hint, GFXTextureHintAddress_T)); CC_GL_CHECK();
            case GL_TEXTURE_1D:
                glTexParameterf(Target, GL_TEXTURE_WRAP_S, GLTextureAddressMode(Hint, GFXTextureHintAddress_S)); CC_GL_CHECK();
                break;
        }
        
        const CCColourFormat PixelFormat = Data ? Data->format : CCColourFormatRGB8Unorm;
        const GLenum InputFormat = GLTextureInputFormat(PixelFormat), InputType = GLTextureInputFormatType(PixelFormat), InternalFormat = GLTextureInternalFormat(Format);
        
        _Bool FreePixels = FALSE;
        const void *Pixels = Data ? CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0) : NULL;
        if ((!Pixels) && (Data))
        {
            FreePixels = TRUE;
            
            const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
            CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                           CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                           GLTextureDestructor(Texture);
                           return NULL;
                           );
            
            CCPixelDataGetPackedData(Data, 0, 0, 0, Width, Height, Depth, (void*)Pixels); //TODO: Or use the conversion variant, so we handle the conversion instead of GL (slower, but can support many more formats)
        }
        
        switch (Target)
        {
            case GL_TEXTURE_3D:
                glTexImage3D(GL_TEXTURE_3D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, (GLsizei)Depth, 0, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
                
            case GL_TEXTURE_2D:
                glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, (GLsizei)Width, (GLsizei)Height, 0, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
                
            case GL_TEXTURE_1D:
                glTexImage1D(GL_TEXTURE_1D, 0, InternalFormat, (GLsizei)Width, 0, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
        }
        
        if (FreePixels) CCFree((void*)Pixels);
    }
    
    return Texture;
}

static GLTexture GLTextureSubConstructor(CCAllocatorType Allocator, GFXTexture Root, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    GLTexture Texture = CCMalloc(Allocator, sizeof(GLTextureInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Texture)
    {
        CCMemorySetDestructor(Texture, (CCMemoryDestructorCallback)GLTextureDestroy);
        
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
        
        GLTexture Root = (GLTexture)Texture->sub.parent;
        for (; !Root->isRoot; Root = (GLTexture)Root->sub.parent)
        {
            Texture->sub.internal.x += Root->sub.x;
            Texture->sub.internal.y += Root->sub.y;
            Texture->sub.internal.z += Root->sub.z;
        }
        
        Texture->sub.internal.root = (GFXTexture)Root;
        
        if (Data)
        {
            const GLuint ID = Root->root.texture;
            const GLenum Target = GLTextureTarget(Root->root.hint);
            CC_GL_BIND_TEXTURE_TARGET(Target, ID);
            
            const CCColourFormat PixelFormat = Data ? Data->format : CCColourFormatRGB8Unorm;
            const GLenum InputFormat = GLTextureInputFormat(PixelFormat), InputType = GLTextureInputFormatType(PixelFormat);
            
            _Bool FreePixels = FALSE;
            const void *Pixels = Data ? CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0) : NULL;
            if (!Pixels)
            {
                FreePixels = TRUE;
                
                const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
                CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                               CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                               GLTextureDestructor(Texture);
                               return NULL;
                               );
                
                CCPixelDataGetPackedData(Data, 0, 0, 0, Width, Height, Depth, (void*)Pixels); //TODO: Or use the conversion variant, so we handle the conversion instead of GL (slower, but can support many more formats)
            }
            
            switch (Target)
            {
                case GL_TEXTURE_3D:
                    glTexSubImage3D(GL_TEXTURE_3D, 0, (GLint)Texture->sub.internal.x, (GLint)Texture->sub.internal.y, (GLint)Texture->sub.internal.z, (GLsizei)Width, (GLsizei)Height, (GLsizei)Depth, InputFormat, InputType, Pixels); CC_GL_CHECK();
                    break;
                    
                case GL_TEXTURE_2D:
                    glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)Texture->sub.internal.x, (GLint)Texture->sub.internal.y, (GLsizei)Width, (GLsizei)Height, InputFormat, InputType, Pixels); CC_GL_CHECK();
                    break;
                    
                case GL_TEXTURE_1D:
                    glTexSubImage1D(GL_TEXTURE_1D, 0, (GLint)Texture->sub.internal.x, (GLsizei)Width, InputFormat, InputType, Pixels); CC_GL_CHECK();
                    break;
            }
            
            if (FreePixels) CCFree((void*)Pixels);
        }
    }
    
    return Texture;
}

static void GLTextureDestructor(GLTexture Texture)
{
    CC_SAFE_Free(Texture);
}

static GFXTexture GLTextureGetParent(GLTexture Texture)
{
    return Texture->isRoot ? NULL : Texture->sub.parent;
}

static GFXTextureHint GLTextureGetHint(GLTexture Texture)
{
    return Texture->isRoot ? Texture->root.hint : ((GLTexture)Texture->sub.internal.root)->root.hint;
}

static CCColourFormat GLTextureGetFormat(GLTexture Texture)
{
    return Texture->isRoot ? Texture->root.format : ((GLTexture)Texture->sub.internal.root)->root.format;
}

static CCPixelData GLTextureGetData(GLTexture Texture)
{
    return Texture->data;
}

static void GLTextureGetOffset(GLTexture Texture, size_t *X, size_t *Y, size_t *Z)
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

static void GLTextureGetInternalOffset(GLTexture Texture, size_t *X, size_t *Y, size_t *Z)
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

static void GLTextureGetSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = Texture->width;
    if (Height) *Height = Texture->height;
    if (Depth) *Depth = Texture->depth;
}

static void GLTextureGetInternalSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Texture->isRoot)
    {
        if (Width) *Width = Texture->width;
        if (Height) *Height = Texture->height;
        if (Depth) *Depth = Texture->depth;
    }
    
    else
    {
        if (Width) *Width = ((GLTexture)Texture->sub.internal.root)->width;
        if (Height) *Height = ((GLTexture)Texture->sub.internal.root)->height;
        if (Depth) *Depth = ((GLTexture)Texture->sub.internal.root)->depth;
    }
}

static void GLTextureSetFilterMode(GLTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode)
{
    if (Texture->isRoot)
    {
        //TODO: Later set a copy of the Texture->hint and only apply the changes when required
        const GLenum Target = GLTextureTarget(Texture->root.hint);
        
        CC_GL_BIND_TEXTURE_TARGET(Target, Texture->root.texture);
        glTexParameteri(Target, FilterType == GFXTextureHintFilterMin ? GL_TEXTURE_MIN_FILTER : GL_TEXTURE_MAG_FILTER, GLTextureFilter(FilterMode, FilterType)); CC_GL_CHECK();
    }
    //TODO: Likely replace with sampler objects (ARB_sampler_objects) so sub textures and root textures can retain their own sampling preferences
    else CCAssertLog(0, "Cannot set a sub texture's filtering mode"); //GLTextureSetFilterMode((GLTexture)Texture->sub.parent, FilterType, FilterMode);
}

static void GLTextureSetAddressMode(GLTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode)
{
    if (Texture->isRoot)
    {
        //TODO: Later set a copy of the Texture->hint and only apply the changes when required
        const GLenum Target = GLTextureTarget(Texture->root.hint);
        
        CC_GL_BIND_TEXTURE_TARGET(Target, Texture->root.texture);
        glTexParameterf(Target, (GLenum[3]){ GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R }[Coordinate], GLTextureAddressMode(AddressMode, Coordinate)); CC_GL_CHECK();
    }
    //TODO: Likely replace with sampler objects (ARB_sampler_objects) so sub textures and root textures can retain their own sampling preferences
    else CCAssertLog(0, "Cannot set a sub texture's filtering mode"); //GLTextureSetAddressMode((GLTexture)Texture->sub.parent, Coordinate, AddressMode);
}

static CCPixelData GLTextureRead(GLTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth)
{
    const GLuint ID = GLTextureGetID(Texture);
    const GLenum Target = GLTextureTarget(GLTextureGetHint(Texture));
    CC_GL_BIND_TEXTURE_TARGET(Target, ID);
    
    const CCColourFormat PixelFormat = GLTextureGetFormat(Texture);
    const GLenum InternalFormat = GLTextureInputFormat(PixelFormat), InternalType = GLTextureInputFormatType(PixelFormat);
    
    GLTexture Root = Texture->isRoot ? Texture : (GLTexture)Texture->sub.internal.root;
    
    void *Pixels;
    const size_t InternalSampleSize = CCColourFormatSampleSizeForPlanar(PixelFormat, CCColourFormatChannelPlanarIndex0);
    CC_SAFE_Malloc(Pixels, InternalSampleSize * Root->width * Root->height * Root->depth,
                   CC_LOG_ERROR("Failed to allocate memory for texture receive. Allocation size (%zu)", InternalSampleSize * Width * Height * Depth);
                   return NULL;
                   );
    
    glGetTexImage(Target, 0, InternalFormat, InternalType, Pixels); CC_GL_CHECK();
    
    CCPixelData CopiedData = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataBufferHintFree, InternalSampleSize * Root->width * Root->height * Root->depth, Pixels, NULL, NULL), PixelFormat, Root->width, Root->height, Root->depth);
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    GLTextureGetInternalOffset(Texture, &RealX, &RealY, &RealZ);
    
    const size_t SampleSize = CCColourFormatSampleSizeForPlanar(PixelFormat, CCColourFormatChannelPlanarIndex0);
    if (!(Pixels = CCMalloc(Allocator, SampleSize * Width * Height * Depth, NULL, CC_DEFAULT_ERROR_CALLBACK)))
    {
        CC_LOG_ERROR("Failed to allocate memory for texture receive. Allocation size (%zu)", SampleSize * Width * Height * Depth);
        CCPixelDataDestroy(CopiedData);
        return NULL;
    }
    
    CCPixelDataGetPackedDataWithFormat(CopiedData, Format, RealX + X, RealY + Y, RealZ + Z, Width, Height, Depth, Pixels);
    CCPixelDataDestroy(CopiedData);
    
    return CCPixelDataStaticCreate(Allocator, CCDataBufferCreate(Allocator, CCDataBufferHintFree, SampleSize * Width * Height * Depth, Pixels, NULL, NULL), Format, Width, Height, Depth);
}

static void GLTextureWrite(GLTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data)
{
    const GLuint ID = GLTextureGetID(Texture);
    const GLenum Target = GLTextureTarget(GLTextureGetHint(Texture));
    CC_GL_BIND_TEXTURE_TARGET(Target, ID);
    
    const CCColourFormat PixelFormat = Data ? Data->format : CCColourFormatRGB8Unorm;
    const GLenum InputFormat = GLTextureInputFormat(PixelFormat), InputType = GLTextureInputFormatType(PixelFormat);
    
    _Bool FreePixels = FALSE;
    const void *Pixels = CCPixelDataGetBuffer(Data, CCColourFormatChannelPlanarIndex0);
    if (!Pixels)
    {
        FreePixels = TRUE;
        
        const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
        CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                       CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                       return;
                       );
        
        CCPixelDataGetPackedData(Data, 0, 0, 0, Width, Height, Depth, (void*)Pixels); //TODO: Or use the conversion variant, so we handle the conversion instead of GL (slower, but can support many more formats)
    }
    
    size_t RealX = 0, RealY = 0, RealZ = 0;
    GLTextureGetInternalOffset(Texture, &RealX, &RealY, &RealZ);
    
    switch (Target)
    {
        case GL_TEXTURE_3D:
            glTexSubImage3D(GL_TEXTURE_3D, 0, (GLint)(RealX + X), (GLint)(RealY + Y), (GLint)(RealZ + Z), (GLsizei)Width, (GLsizei)Height, (GLsizei)Depth, InputFormat, InputType, Pixels); CC_GL_CHECK();
            break;
            
        case GL_TEXTURE_2D:
            glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)(RealX + X), (GLint)(RealY + Y), (GLsizei)Width, (GLsizei)Height, InputFormat, InputType, Pixels); CC_GL_CHECK();
            break;
            
        case GL_TEXTURE_1D:
            glTexSubImage1D(GL_TEXTURE_1D, 0, (GLint)(RealX + X), (GLsizei)Width, InputFormat, InputType, Pixels); CC_GL_CHECK();
            break;
    }
    
    if (FreePixels) CCFree((void*)Pixels);
}

static void *GLTextureGetStreamData(GLTexture Texture)
{
    return Texture->stream;
}

static void GLTextureSetStreamData(GLTexture Texture, void *Stream)
{
    Texture->stream = Stream;
}
