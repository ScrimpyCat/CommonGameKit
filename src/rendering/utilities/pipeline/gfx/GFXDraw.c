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

#define CC_QUICK_COMPILE
#include "GFXDraw.h"
#include "GFXMain.h"
#include "Window.h"

static void GFXDrawInputBufferElementDestructor(CCCollection(GFXDrawInputBuffer) Collection, GFXDrawInputBuffer *Element)
{
    GFXBufferDestroy(Element->buffer);
    CCStringDestroy(Element->input.name);
}

static void GFXDrawInputTextureElementDestructor(CCCollection(GFXDrawInputTexture) Collection, GFXDrawInputTexture *Element)
{
    GFXTextureDestroy(Element->texture);
    CCStringDestroy(Element->input.name);
}

static void GFXDrawDestructor(GFXDraw Draw)
{
    if (GFXMain->draw->optional.destroy) GFXMain->draw->optional.destroy(Draw);
    
    CCCollectionDestroy(Draw->vertexBuffers);
    CCCollectionDestroy(Draw->buffers);
    CCCollectionDestroy(Draw->textures);
    
    CC_SAFE_Free(Draw->shader);
    CC_SAFE_Free(Draw->destination.framebuffer);
    CC_SAFE_Free(Draw->index.buffer);
}

GFXDraw GFXDrawCreate(CCAllocatorType Allocator)
{
    GFXDraw Draw = CCMalloc(Allocator, sizeof(GFXDrawInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Draw)
    {
        CCMemorySetDestructor(Draw, (CCMemoryDestructorCallback)GFXDrawDestructor);
        
        *Draw = (GFXDrawInfo){
            .internal = NULL,
            .shader = NULL,
            .vertexBuffers = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall, sizeof(GFXDrawInputVertexBuffer), (CCCollectionElementDestructor)GFXDrawInputBufferElementDestructor),
            .buffers = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall, sizeof(GFXDrawInputBuffer), (CCCollectionElementDestructor)GFXDrawInputBufferElementDestructor),
            .textures = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall, sizeof(GFXDrawInputTexture), (CCCollectionElementDestructor)GFXDrawInputTextureElementDestructor),
            .destination = {
                .framebuffer = NULL,
                .index = 0
            },
            .index = {
                .buffer = NULL,
                .format = 0
            },
            .blending = GFXBlendOpaque,
            .viewport = { .bounds = GFXViewport2D(0.0f, 0.0f, 1.0f, 1.0f), .normalized = TRUE }
        };
        
        if (GFXMain->draw->optional.create) GFXMain->draw->optional.create(Allocator, Draw);
    }
    
    else CC_LOG_ERROR("Failed to create draw command due to allocation failure. Allocation size (%zu)", sizeof(GFXDrawInfo));
    
    return Draw;
}

void GFXDrawDestroy(GFXDraw Draw)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    CC_SAFE_Free(Draw);
}

void GFXDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count)
{
    GFXDrawSubmitInstances(Draw, Primitive, Offset, Count, 1);
}

void GFXDrawSubmitInstances(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXMain->draw->submit(Draw, Primitive, Offset, Count, Instances);
}

void GFXDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count)
{    
    GFXDrawSubmitIndexedInstances(Draw, Primitive, Offset, Count, 1);
}

void GFXDrawSubmitIndexedInstances(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXMain->draw->indexedSubmit(Draw, Primitive, Offset, Count, Instances);
}

static void GFXDrawCacheShaderInputs(CCCollection(GFXDrawInput) Collection, GFXShader Shader)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Collection, &Enumerator);
    
    for (GFXDrawInput *Current = CCCollectionEnumeratorGetCurrent(&Enumerator); Current; Current = CCCollectionEnumeratorNext(&Enumerator))
    {
        Current->shaderInput = GFXShaderGetInput(Shader, Current->name);
    }
}

void GFXDrawSetShader(GFXDraw Draw, GFXShader Shader)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    CC_SAFE_Free(Draw->shader);
    Draw->shader = CCRetain(Shader);
    
    if  (Shader)
    {
        GFXDrawCacheShaderInputs(Draw->vertexBuffers, Shader);
        GFXDrawCacheShaderInputs(Draw->buffers, Shader);
        GFXDrawCacheShaderInputs(Draw->textures, Shader);
    }
    
    if (GFXMain->draw->optional.setShader) GFXMain->draw->optional.setShader(Draw, Shader);
}

void GFXDrawSetFramebuffer(GFXDraw Draw, GFXFramebuffer Framebuffer, size_t Index)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    CC_SAFE_Free(Draw->destination.framebuffer);
    Draw->destination = (GFXDrawDestination){
        .framebuffer = CCRetain(Framebuffer),
        .index = Index
    };
    
    if (GFXMain->draw->optional.setFramebuffer) GFXMain->draw->optional.setFramebuffer(Draw, &Draw->destination);
    if (GFXMain->draw->optional.setViewport) GFXMain->draw->optional.setViewport(Draw, GFXDrawGetViewport(Draw));
}

void GFXDrawSetIndexBuffer(GFXDraw Draw, GFXBuffer Indexes, GFXBufferFormat Format)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    CC_SAFE_Free(Draw->index.buffer);
    Draw->index = (GFXDrawIndexBuffer){
        .buffer = CCRetain(Indexes),
        .format = Format
    };
    
    if (GFXMain->draw->optional.setIndexBuffer) GFXMain->draw->optional.setIndexBuffer(Draw, &Draw->index);
}

static CCComparisonResult GFXDrawFindInput(const GFXDrawInput *left, const GFXDrawInput *right)
{
    return CCStringEqual(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

void GFXDrawSetVertexBuffer(GFXDraw Draw, CCString Input, GFXBuffer Buffer, GFXBufferFormat Format, size_t Stride, ptrdiff_t Offset)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXDrawInputVertexBuffer *VertexBuffer = CCCollectionGetElement(Draw->vertexBuffers, CCCollectionFindElement(Draw->vertexBuffers, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
    if (VertexBuffer)
    {
        CC_SAFE_Free(VertexBuffer->buffer);
        VertexBuffer->buffer = CCRetain(Buffer);
        VertexBuffer->offset = Offset;
        VertexBuffer->stride = Stride;
        VertexBuffer->format = Format;
    }
    
    else
    {
        VertexBuffer = CCCollectionGetElement(Draw->vertexBuffers, CCCollectionInsertElement(Draw->vertexBuffers, &(GFXDrawInputVertexBuffer){
            .input = {
                .name = CCStringCopy(Input),
                .shaderInput = Draw->shader ? GFXShaderGetInput(Draw->shader, Input) : NULL
            },
            .buffer = CCRetain(Buffer),
            .offset = Offset,
            .stride = Stride,
            .format = Format
        }));
    }
    
    if (GFXMain->draw->optional.setVertexBuffer) GFXMain->draw->optional.setVertexBuffer(Draw, VertexBuffer);
}

void GFXDrawSetBuffer(GFXDraw Draw, CCString Input, GFXBuffer Buffer)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXDrawInputBuffer *UniformBuffer = CCCollectionGetElement(Draw->buffers, CCCollectionFindElement(Draw->buffers, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
    if (UniformBuffer)
    {
        CC_SAFE_Free(UniformBuffer->buffer);
        UniformBuffer->buffer = CCRetain(Buffer);
    }
    
    else
    {
        UniformBuffer = CCCollectionGetElement(Draw->buffers, CCCollectionInsertElement(Draw->buffers, &(GFXDrawInputBuffer){
            .input = {
                .name = CCStringCopy(Input),
                .shaderInput = Draw->shader ? GFXShaderGetInput(Draw->shader, Input) : NULL
            },
            .buffer = CCRetain(Buffer)
        }));
    }
    
    if (GFXMain->draw->optional.setBuffer) GFXMain->draw->optional.setBuffer(Draw, UniformBuffer);
}

void GFXDrawSetTexture(GFXDraw Draw, CCString Input, GFXTexture Texture)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXDrawInputTexture *UniformTexture = CCCollectionGetElement(Draw->textures, CCCollectionFindElement(Draw->textures, &(GFXDrawInput){ .name = Input }, (CCComparator)GFXDrawFindInput));
    if (UniformTexture)
    {
        CC_SAFE_Free(UniformTexture->texture);
        UniformTexture->texture = CCRetain(Texture);
    }
    
    else
    {
        UniformTexture = CCCollectionGetElement(Draw->textures, CCCollectionInsertElement(Draw->textures, &(GFXDrawInputTexture){
            .input = {
                .name = CCStringCopy(Input),
                .shaderInput = Draw->shader ? GFXShaderGetInput(Draw->shader, Input) : NULL
            },
            .texture = CCRetain(Texture)
        }));
    }
    
    if (GFXMain->draw->optional.setTexture) GFXMain->draw->optional.setTexture(Draw, UniformTexture);
}

void GFXDrawSetBlending(GFXDraw Draw, GFXBlend BlendMask)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    Draw->blending = BlendMask;
    
    if (GFXMain->draw->optional.setBlend) GFXMain->draw->optional.setBlend(Draw, BlendMask);
}

void GFXDrawSetViewport(GFXDraw Draw, GFXViewport Viewport)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    Draw->viewport.bounds = Viewport;
    Draw->viewport.normalized = FALSE;
    
    if (GFXMain->draw->optional.setViewport) GFXMain->draw->optional.setViewport(Draw, Viewport);
}

void GFXDrawSetNormalizedViewport(GFXDraw Draw, GFXViewport Viewport)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    Draw->viewport.bounds = Viewport;
    Draw->viewport.normalized = TRUE;
    
    if (GFXMain->draw->optional.setViewport) GFXMain->draw->optional.setViewport(Draw, GFXDrawGetViewport(Draw));
}

GFXViewport GFXDrawGetViewport(GFXDraw Draw)
{
    CCAssertLog(Draw, "Draw must not be null");
    
    GFXViewport Viewport = Draw->viewport.bounds;
    if (Draw->viewport.normalized)
    {
        if (Draw->destination.framebuffer == GFXFramebufferDefault())
        {
            CCVector2Di FrameSize = CCWindowGetFrameSize();
            Viewport = GFXViewport3D(FrameSize.x * Draw->viewport.bounds.x, FrameSize.y * Draw->viewport.bounds.y, FrameSize.x * Draw->viewport.bounds.width, FrameSize.y * Draw->viewport.bounds.height, Draw->viewport.bounds.minDepth, Draw->viewport.bounds.maxDepth);
        }
        
        else
        {
            GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Draw->destination.framebuffer, Draw->destination.index);
            if (Attachment)
            {
                size_t Width, Height;
                GFXTextureGetSize(Attachment->texture, &Width, &Height, NULL);
                Viewport = GFXViewport3D((float)Width * Draw->viewport.bounds.x, (float)Height * Draw->viewport.bounds.y, (float)Width * Draw->viewport.bounds.width, (float)Height * Draw->viewport.bounds.height, Draw->viewport.bounds.minDepth, Draw->viewport.bounds.maxDepth);
            }
        }
    }
    
    return Viewport;
}
