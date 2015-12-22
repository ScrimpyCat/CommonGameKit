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

#include "GLDraw.h"
#include "GLBuffer.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"
#include "GLShader.h"

static void GLDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);
//static void GLDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);
static void GLDrawConstructor(CCAllocatorType Allocator, GFXDraw Draw);
static void GLDrawDestructor(GFXDraw Draw);
//static void GLDrawSetShader(GFXDraw Draw, GFXShader Shader);
//static void GLDrawSetFramebuffer(GFXDraw, GFXDrawDestination *Destination);
//static void GLDrawSetIndexBuffer(GFXDraw Draw, GFXDrawIndexBuffer *IndexBuffer);
//static void GLDrawSetVertexBuffer(GFXDraw Draw, GFXDrawInputVertexBuffer *VertexBuffer);
//static void GLDrawSetBuffer(GFXDraw Draw, GFXDrawInputBuffer *Buffer);
//static void GLDrawSetTexture(GFXDraw Draw, GFXDrawInputTexture *Texture);


const GFXDrawInterface GLDrawInterface = {
    .submit = GLDrawSubmit,
//    .indexedSubmit = GLDrawSubmitIndexed,
    .optional = {
        .create = GLDrawConstructor,
        .destroy = GLDrawDestructor,
//        .setShader = GLDrawSetShader,
//        .setFramebuffer = GLDrawSetFramebuffer,
//        .setIndexBuffer = GLDrawSetIndexBuffer,
//        .setVertexBuffer = GLDrawSetVertexBuffer,
//        .setBuffer = GLDrawSetBuffer,
//        .setTexture = GLDrawSetTexture
    }
};


static void GLDrawConstructor(CCAllocatorType Allocator, GFXDraw Draw)
{
    Draw->internal = CCMalloc(Allocator, sizeof(GLDrawState), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Draw)
    {
        glGenVertexArrays(1, &((GLDrawState*)Draw->internal)->vao); CC_GL_CHECK();
    }
    
    else CC_LOG_ERROR("Failed to create internal GL draw state due to allocation failure. Allocation size (%zu)", sizeof(GLDrawState));
}

static void GLDrawDestructor(GFXDraw Draw)
{
    CCAssertLog(Draw->internal, "GL implementation must not be null"); //TODO: handle if it is?
    
    glDeleteVertexArrays(1, &((GLDrawState*)Draw->internal)->vao); CC_GL_CHECK();
    CC_SAFE_Free(Draw->internal);
}

static GLenum GLDrawPrimitive(GFXPrimitiveType Primitive)
{
    switch (Primitive)
    {
        case GFXPrimitiveTypePoint:
            return GL_POINTS;
            
        case GFXPrimitiveTypeLine:
            return GL_LINES;
            
        case GFXPrimitiveTypeLineStrip:
            return GL_LINE_STRIP;
            
        case GFXPrimitiveTypeTriangle:
            return GL_TRIANGLES;
            
        case GFXPrimitiveTypeTriangleStrip:
            return GL_TRIANGLE_STRIP;
    }
    
    CCAssertLog(0, "Unsupported primitive");
}

static void GLDrawSetUniformState(GLShader Shader, CCCollection Buffers)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Buffers, &Enumerator);
    
    for (GFXDrawInputBuffer *Uniform = CCCollectionEnumeratorGetCurrent(&Enumerator); Uniform; Uniform = CCCollectionEnumeratorNext(&Enumerator))
    {
        if ((((GLBuffer)Uniform->buffer)->hint & GFXBufferHintDataMask) == GFXBufferHintData)
        {
            GLShaderSetUniform(Shader, (GLShaderUniformInfo*)Uniform->input.shaderInput, GFXBufferGetSize(Uniform->buffer) / GFXBufferFormatGetSize(((GLShaderUniformInfo*)Uniform->input.shaderInput)->type), CCDataGetBuffer(((GLBuffer)Uniform->buffer)->data));
        }
        
        else
        {
            CCAssertLog(0, "No support for UBOs yet");
        }
    }
}

static void GLDrawSetUniformTextureState(GLShader Shader, CCCollection Textures)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Textures, &Enumerator);
    
    GLint TextureUnit = 0; //TODO: later loop through state to see if we can reuse without causing rebinding
    for (GFXDrawInputTexture *Uniform = CCCollectionEnumeratorGetCurrent(&Enumerator); Uniform; Uniform = CCCollectionEnumeratorNext(&Enumerator))
    {
        CC_GL_ACTIVE_TEXTURE(TextureUnit);
        CC_GL_BIND_TEXTURE_TARGET(GLTextureTarget(GFXTextureGetHints(Uniform->texture)), GLTextureGetID((GLTexture)Uniform->texture));
        
        GLShaderSetUniform(Shader, (GLShaderUniformInfo*)Uniform->input.shaderInput, 1, &TextureUnit);
        TextureUnit++;
    }
}

static void GLDrawSetFramebufferState(GLFramebuffer Framebuffer, size_t Index)
{
    CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, GLFramebufferGetID(Framebuffer, Index));
#ifdef GL_COLOR_ATTACHMENT1
    if (Framebuffer)
    {
        glDrawBuffers(1, &(GLenum){ GLFramebufferGetAttachmentIndex(Framebuffer, Index) }); CC_GL_CHECK();
    }
#endif
}

static GLenum GLDrawTypeFromBufferFormat(GFXBufferFormat Format)
{
    switch (Format)
    {
        case GFXBufferFormatInt8:
            return GL_BYTE;
            
        case GFXBufferFormatUInt8:
            return GL_UNSIGNED_BYTE;
            
        case GFXBufferFormatInt16:
            return GL_SHORT;
            
        case GFXBufferFormatUInt16:
            return GL_UNSIGNED_SHORT;
            
        case GFXBufferFormatInt32:
            return GL_INT;
            
        case GFXBufferFormatUInt32:
            return GL_UNSIGNED_INT;
            
        case GFXBufferFormatFloat16:
            return GL_HALF_FLOAT;
            
        case GFXBufferFormatFloat32:
            return GL_FLOAT;
            
        case GFXBufferFormatFloat64:
            return GL_DOUBLE;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported format %d", Format);
}

static void GLDrawSetVertexBufferState(CCCollection VertexBuffers)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(VertexBuffers, &Enumerator);
    
    for (GFXDrawInputVertexBuffer *Vertex = CCCollectionEnumeratorGetCurrent(&Enumerator); Vertex; Vertex = CCCollectionEnumeratorNext(&Enumerator))
    {
        const GLuint Index = ((GLShaderAttributeInfo*)Vertex->input.shaderInput)->location;
        glEnableVertexAttribArray(Index); CC_GL_CHECK();
        
        const GLvoid *Data = NULL;
        if ((((GLBuffer)Vertex->buffer)->hint & GFXBufferHintDataMask) == GFXBufferHintData)
        {
            Data = CCDataGetBuffer(((GLBuffer)Vertex->buffer)->data);
        }
        
        else
        {
            CC_GL_BIND_BUFFER(GL_ARRAY_BUFFER, ((GLBuffer)Vertex->buffer)->gl.buffer);
        }
        
        glVertexAttribPointer(Index, (GLint)GFXBufferFormatGetElementCount(Vertex->format), GLDrawTypeFromBufferFormat(Vertex->format), Vertex->format & GFXBufferFormatNormalized ? GL_TRUE : GL_FALSE, (GLsizei)Vertex->stride, Data + Vertex->offset); CC_GL_CHECK();
    }
}

static void GLDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count)
{
    CCAssertLog(Draw->internal, "GL implementation must not be null"); //TODO: handle if it is?
    CCAssertLog((GLint)Offset < Offset, "GL implementation cannot support offsets of that size");
    CCAssertLog((GLsizei)Count < Count, "GL implementation cannot support counts of that size");
    
    CC_GL_USE_PROGRAM(((GLShader)Draw->shader)->program);
    GLDrawSetUniformState((GLShader)Draw->shader, Draw->buffers);
    GLDrawSetUniformTextureState((GLShader)Draw->shader, Draw->textures);
    
    GLDrawSetFramebufferState((GLFramebuffer)Draw->destination.framebuffer, Draw->destination.index);
    GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Draw->destination.framebuffer, Draw->destination.index);
    if (Attachment->load == GFXFramebufferAttachmentActionClear)
    {
        CC_GL_CLEAR_COLOR(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
    }
    
    
    CC_GL_BIND_VERTEX_ARRAY(((GLDrawState*)Draw->internal)->vao);
    
    //if rebind vao state
    //bind IBO
    GLDrawSetVertexBufferState(Draw->vertexBuffers);
    
    //else
    
    //TODO: blending
    glDrawArrays(GLDrawPrimitive(Primitive), (GLint)Offset, (GLsizei)Count); CC_GL_CHECK();
    
    
    if (Attachment->store == GFXFramebufferAttachmentActionClear)
    {
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
    }
}

//static void GLDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count);
