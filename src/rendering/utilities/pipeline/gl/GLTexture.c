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

static GLTexture GLTextureConstructor(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static GLTexture GLTextureSubConstructor(CCAllocatorType Allocator, GFXTexture Root, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
static void GLTextureDestructor(GLTexture Texture);
static GFXTextureHint GLTextureGetHint(GLTexture Texture);
static CCPixelData GLTextureGetData(GLTexture Texture);
static void GLTextureGetOffset(GLTexture Texture, size_t *X, size_t *Y, size_t *Z);
static void GLTextureGetSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
static void GLTextureSetFilterMode(GLTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);
static void GLTextureSetAddressMode(GLTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);


const GFXTextureInterface GLTextureInterface = {
    .create = (GFXTextureConstructorCallback)GLTextureConstructor,
    .createSub = (GFXTextureSubConstructorCallback)GLTextureSubConstructor,
    .destroy = (GFXTextureDestructorCallback)GLTextureDestructor,
    .hints = (GFXTextureGetHintCallback)GLTextureGetHint,
    .data = (GFXTextureGetDataCallback)GLTextureGetData,
    .offset = (GFXTextureGetOffsetCallback)GLTextureGetOffset,
    .size = (GFXTextureGetSizeCallback)GLTextureGetSize,
    .setFilterMode = (GFXTextureSetFilterModeCallback)GLTextureSetFilterMode,
    .setAddressMode = (GFXTextureSetAddressModeCallback)GLTextureSetAddressMode,
    .optional = {
        //TODO: Add invalidation .invalidate = (GFXTextureInvalidateCallback)GLTextureInvalidate
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
        void *Pixels = NULL; //TODO: Add optional internal buffer access
        if ((!Pixels) && (Data))
        {
            FreePixels = TRUE;
            
            const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
            CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                           CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                           GLTextureDestructor(Texture);
                           return NULL;
                           );
            
            CCPixelDataGetPackedData(Data, Width, Height, Depth, Pixels); //TODO: Or use the conversion variant, so we handle the conversion instead of GL (slower, but can support many more formats)
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
        
        if (FreePixels) CC_SAFE_Free(Pixels);
    }
    
    return Texture;
}

static void GLTextureGetRootCoordinates(GLTexture Texture, size_t *X, size_t *Y, size_t *Z)
{
    if (X) *X = 0;
    if (Y) *Y = 0;
    if (Z) *Z = 0;
    
    while (!Texture->isRoot)
    {
        if (X) *X += Texture->sub.x;
        if (Y) *Y += Texture->sub.y;
        if (Z) *Z += Texture->sub.z;
        
        Texture = (GLTexture)Texture->sub.parent;
    }
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
        Texture->data = Data;
        Texture->width = Width;
        Texture->height = Height;
        Texture->depth = Depth;
        
        const GLuint ID = GLTextureGetID(Texture);
        const GLenum Target = GLTextureGetHint(Texture);
        CC_GL_BIND_TEXTURE_TARGET(Target, ID);
        
        const CCColourFormat PixelFormat = Data ? Data->format : CCColourFormatRGB8Unorm;
        const GLenum InputFormat = GLTextureInputFormat(PixelFormat), InputType = GLTextureInputFormatType(PixelFormat);
        
        _Bool FreePixels = FALSE;
        void *Pixels = NULL; //TODO: Add optional internal buffer access
        if ((!Pixels) && (Data))
        {
            FreePixels = TRUE;
            
            const size_t SampleSize = CCColourFormatSampleSizeForPlanar(Data->format, CCColourFormatChannelPlanarIndex0);
            CC_SAFE_Malloc(Pixels, SampleSize * Width * Height * Depth,
                           CC_LOG_ERROR("Failed to allocate memory for texture transfer. Allocation size (%zu)", SampleSize * Width * Height * Depth);
                           GLTextureDestructor(Texture);
                           return NULL;
                           );
            
            CCPixelDataGetPackedData(Data, Width, Height, Depth, Pixels); //TODO: Or use the conversion variant, so we handle the conversion instead of GL (slower, but can support many more formats)
        }
        
        GLTextureGetRootCoordinates(Texture, &X, &Y, &Z);
        
        switch (Target)
        {
            case GL_TEXTURE_3D:
                glTexSubImage3D(GL_TEXTURE_3D, 0, (GLint)X, (GLint)Y, (GLint)Z, (GLsizei)Width, (GLsizei)Height, (GLsizei)Depth, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
                
            case GL_TEXTURE_2D:
                glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)X, (GLint)Y, (GLsizei)Width, (GLsizei)Height, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
                
            case GL_TEXTURE_1D:
                glTexSubImage1D(GL_TEXTURE_1D, 0, (GLint)X, (GLsizei)Width, InputFormat, InputType, Pixels); CC_GL_CHECK();
                break;
        }
        
        if (FreePixels) CC_SAFE_Free(Pixels);
    }
    
    return Texture;
}

static void GLTextureDestructor(GLTexture Texture)
{
    CC_SAFE_Free(Texture);
}

static GFXTextureHint GLTextureGetHint(GLTexture Texture)
{
    return Texture->isRoot ? Texture->root.hint : GLTextureGetHint((GLTexture)Texture->sub.parent);
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

static void GLTextureGetSize(GLTexture Texture, size_t *Width, size_t *Height, size_t *Depth)
{
    if (Width) *Width = Texture->width;
    if (Height) *Height = Texture->height;
    if (Depth) *Depth = Texture->depth;
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
