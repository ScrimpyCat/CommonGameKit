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

#ifndef CommonGameKit_GFXInterface_h
#define CommonGameKit_GFXInterface_h

#include <CommonC/Allocator.h>
#include <CommonGameKit/GFXBuffer.h>
#include <CommonGameKit/GFXTexture.h>
#include <CommonGameKit/GFXFramebuffer.h>
#include <CommonGameKit/GFXShaderLibrary.h>
#include <CommonGameKit/GFXShader.h>
#include <CommonGameKit/GFXDraw.h>
#include <CommonGameKit/GFXBlit.h>
#include <CommonGameKit/GFXViewport.h>

#pragma mark - Required Buffer Callbacks
typedef CC_NEW GFXBuffer (*GFXBufferConstructorCallback)(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data);
typedef void (*GFXBufferDestructorCallback)(GFXBuffer CC_DESTROY(Internal));
typedef GFXBufferHint (*GFXBufferGetHintCallback)(GFXBuffer Internal);
typedef size_t (*GFXBufferGetSizeCallback)(GFXBuffer Internal);
typedef _Bool (*GFXBufferResizeCallback)(GFXBuffer Buffer, size_t Size);
typedef size_t (*GFXBufferReadBufferCallback)(GFXBuffer Internal, ptrdiff_t Offset, size_t Size, void *Data);
typedef size_t (*GFXBufferWriteBufferCallback)(GFXBuffer Internal, ptrdiff_t Offset, size_t Size, const void *Data);

#pragma mark Required Texture Callbacks
typedef CC_NEW GFXTexture (*GFXTextureConstructorCallback)(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData CC_OWN(Data));
typedef CC_NEW GFXTexture (*GFXTextureSubConstructorCallback)(CCAllocatorType Allocator, GFXTexture CC_OWN(Root), size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData CC_OWN(Data));
typedef void (*GFXTextureDestructorCallback)(GFXTexture CC_DESTROY(Texture));
typedef GFXTexture (*GFXTextureGetParentCallback)(GFXTexture Texture);
typedef GFXTextureHint (*GFXTextureGetHintCallback)(GFXTexture Texture);
typedef CCPixelData (*GFXTextureGetDataCallback)(GFXTexture Texture);
typedef void (*GFXTextureGetOffsetCallback)(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z);
typedef void (*GFXTextureGetSizeCallback)(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);
typedef void (*GFXTextureSetFilterModeCallback)(GFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);
typedef void (*GFXTextureSetAddressModeCallback)(GFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);
typedef CC_NEW CCPixelData (*GFXTextureReadCallback)(GFXTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);
typedef void (*GFXTextureWriteCallback)(GFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);
typedef void *(*GFXTextureGetStreamDataCallback)(GFXTexture Texture);
typedef void (*GFXTextureSetStreamDataCallback)(GFXTexture Texture, void *StreamData);

#pragma mark Required Framebuffer Callbacks
typedef CC_NEW GFXFramebuffer (*GFXFramebufferConstructorCallback)(CCAllocatorType Allocator, GFXFramebufferAttachment *CC_OWN(Attachments), size_t Count);
typedef void (*GFXFramebufferDestructorCallback)(GFXFramebuffer CC_DESTROY(Framebuffer));
typedef GFXFramebuffer (*GFXFramebufferDefaultCallback)(void);
typedef GFXFramebufferAttachment *(*GFXFramebufferGetAttachmentCallback)(GFXFramebuffer Framebuffer, size_t Index);

#pragma mark Required Shader Library Callbacks
typedef CC_NEW GFXShaderLibrary (*GFXShaderLibraryConstructorCallback)(CCAllocatorType Allocator);
typedef CC_NEW GFXShaderLibrary (*GFXShaderLibraryPrecompiledConstructorCallback)(CCAllocatorType Allocator, const void *Data);
typedef void (*GFXShaderLibraryDestructorCallback)(GFXShaderLibrary CC_DESTROY(Library));
typedef const GFXShaderSource (*GFXShaderLibraryCompileCallback)(GFXShaderLibrary Library, GFXShaderSourceType Type, CCString CC_COPY(Name), const char *Source);
typedef const GFXShaderSource (*GFXShaderLibraryGetSourceCallback)(GFXShaderLibrary Library, CCString Name);

#pragma mark Required Shader Callbacks
typedef CC_NEW GFXShader (*GFXShaderConstructorCallback)(CCAllocatorType Allocator, GFXShaderSource Vertex, GFXShaderSource Fragment);
typedef void (*GFXShaderDestructorCallback)(GFXShader CC_DESTROY(Shader));
typedef GFXShaderInput (*GFXShaderGetInputCallback)(GFXShader Shader, CCString Name);

#pragma mark Required Draw Callbacks
typedef void (*GFXDrawSubmitCallback)(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);
typedef void (*GFXDrawSubmitIndexedCallback)(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);

#pragma mark Required Blit Callbacks
typedef void (*GFXBlitSubmitCallback)(GFXBlit Blit);



#pragma mark - Optional Buffer Callbacks
typedef void (*GFXBufferInvalidateCallback)(GFXBuffer Internal);
typedef size_t (*GFXBufferCopyBufferCallback)(GFXBuffer SrcInternal, ptrdiff_t SrcOffset, size_t Size, GFXBuffer DstInternal, ptrdiff_t DstOffset);
typedef size_t (*GFXBufferFillBufferCallback)(GFXBuffer Internal, ptrdiff_t Offset, size_t Size, uint8_t Fill);

#pragma mark Optional Texture Callbacks
typedef void (*GFXTextureInvalidateCallback)(GFXTexture Texture);
typedef void (*GFXTextureGetInternalOffsetCallback)(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z);
typedef void (*GFXTextureGetInternalSizeCallback)(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);

#pragma mark Optional Draw Callbacks
typedef void (*GFXDrawConstructorCallback)(CCAllocatorType Allocator, GFXDraw Draw);
typedef void (*GFXDrawDestructorCallback)(GFXDraw Draw);
typedef void (*GFXDrawSetShaderCallback)(GFXDraw Draw, GFXShader Shader);
typedef void (*GFXDrawSetFramebufferCallback)(GFXDraw Draw, GFXDrawDestination *Destination);
typedef void (*GFXDrawSetIndexBufferCallback)(GFXDraw Draw, GFXDrawIndexBuffer *IndexBuffer);
typedef void (*GFXDrawSetVertexBufferCallback)(GFXDraw Draw, GFXDrawInputVertexBuffer *VertexBuffer);
typedef void (*GFXDrawSetBufferCallback)(GFXDraw Draw, GFXDrawInputBuffer *Buffer);
typedef void (*GFXDrawSetTextureCallback)(GFXDraw Draw, GFXDrawInputTexture *Texture);
typedef void (*GFXDrawSetBlendingCallback)(GFXDraw Draw, GFXBlend BlendMask);
typedef void (*GFXDrawSetViewportCallback)(GFXDraw Draw, GFXViewport Viewport);

#pragma mark Optional Blit Callbacks
typedef void (*GFXBlitConstructorCallback)(CCAllocatorType Allocator, GFXBlit Blit);
typedef void (*GFXBlitDestructorCallback)(GFXBlit Blit);
typedef void (*GFXBlitSetSourceCallback)(GFXBlit Blit, GFXBlitFramebuffer *Source);
typedef void (*GFXBlitSetDestinationCallback)(GFXBlit Blit, GFXBlitFramebuffer *Destination);
typedef void (*GFXBlitSetFilterModeCallback)(GFXBlit Draw, GFXTextureHint FilterMode);


#pragma mark -

typedef struct {
    GFXBufferConstructorCallback create;
    GFXBufferDestructorCallback destroy;
    GFXBufferGetHintCallback hints;
    GFXBufferGetSizeCallback size;
    GFXBufferResizeCallback resize;
    GFXBufferReadBufferCallback read;
    GFXBufferWriteBufferCallback write;
    struct {
        GFXBufferInvalidateCallback invalidate;
        GFXBufferCopyBufferCallback copy;
        GFXBufferFillBufferCallback fill;
    } optional;
} GFXBufferInterface;

typedef struct {
    GFXTextureConstructorCallback create;
    GFXTextureSubConstructorCallback createSub;
    GFXTextureDestructorCallback destroy;
    GFXTextureGetParentCallback parent;
    GFXTextureGetHintCallback hints;
    GFXTextureGetDataCallback data;
    GFXTextureGetOffsetCallback offset;
    GFXTextureGetSizeCallback size;
    GFXTextureSetFilterModeCallback setFilterMode;
    GFXTextureSetAddressModeCallback setAddressMode;
    GFXTextureReadCallback read;
    GFXTextureWriteCallback write;
    GFXTextureGetStreamDataCallback getStream;
    GFXTextureSetStreamDataCallback setStream;
    struct {
        GFXTextureInvalidateCallback invalidate;
        GFXTextureGetInternalOffsetCallback internalOffset;
        GFXTextureGetInternalSizeCallback internalSize;
    } optional;
} GFXTextureInterface;

typedef struct {
    GFXFramebufferConstructorCallback create;
    GFXFramebufferDestructorCallback destroy;
    GFXFramebufferDefaultCallback getDefault;
    GFXFramebufferGetAttachmentCallback attachment;
} GFXFramebufferInterface;

typedef struct {
    GFXShaderLibraryConstructorCallback create;
    GFXShaderLibraryPrecompiledConstructorCallback createPrecompiled;
    GFXShaderLibraryDestructorCallback destroy;
    GFXShaderLibraryCompileCallback compile;
    GFXShaderLibraryGetSourceCallback source;
} GFXShaderLibraryInterface;

typedef struct {
    GFXShaderConstructorCallback create;
    GFXShaderDestructorCallback destroy;
    GFXShaderGetInputCallback input;
} GFXShaderInterface;

typedef struct {
    GFXDrawSubmitCallback submit;
    GFXDrawSubmitIndexedCallback indexedSubmit;
    struct {
        GFXDrawConstructorCallback create;
        GFXDrawDestructorCallback destroy;
        GFXDrawSetShaderCallback setShader;
        GFXDrawSetFramebufferCallback setFramebuffer;
        GFXDrawSetIndexBufferCallback setIndexBuffer;
        GFXDrawSetVertexBufferCallback setVertexBuffer;
        GFXDrawSetBufferCallback setBuffer;
        GFXDrawSetTextureCallback setTexture;
        GFXDrawSetBlendingCallback setBlend;
        GFXDrawSetViewportCallback setViewport;
    } optional;
} GFXDrawInterface;

typedef struct {
    GFXBlitSubmitCallback submit;
    struct {
        GFXBlitConstructorCallback create;
        GFXBlitDestructorCallback destroy;
        GFXBlitSetSourceCallback setSource;
        GFXBlitSetDestinationCallback setDestination;
        GFXBlitSetFilterModeCallback setFilterMode;
    } optional;
} GFXBlitInterface;

#endif
