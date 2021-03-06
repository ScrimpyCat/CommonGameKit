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
#include "GLDraw.h"
#include "GLBuffer.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"
#include "GLShader.h"

static void GLDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances);
static void GLDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances);
static void GLDrawConstructor(CCAllocatorType Allocator, GFXDraw Draw);
static void GLDrawDestructor(GFXDraw Draw);
static void GLDrawSetIndexBuffer(GFXDraw Draw, GFXDrawIndexBuffer *IndexBuffer);
static void GLDrawSetVertexBuffer(GFXDraw Draw, GFXDrawInputVertexBuffer *VertexBuffer);
static void GLDrawSetShader(GFXDraw Draw, GFXShader Shader);


const GFXDrawInterface GLDrawInterface = {
    .submit = GLDrawSubmit,
    .indexedSubmit = GLDrawSubmitIndexed,
    .optional = {
        .create = GLDrawConstructor,
        .destroy = GLDrawDestructor,
        .setIndexBuffer = GLDrawSetIndexBuffer,
        .setVertexBuffer = GLDrawSetVertexBuffer,
        .setShader = GLDrawSetShader
    }
};


static void GLDrawConstructor(CCAllocatorType Allocator, GFXDraw Draw)
{
    Draw->internal = CCMalloc(Allocator, sizeof(GLDrawState), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Draw)
    {
        glGenVertexArrays(1, &((GLDrawState*)Draw->internal)->vao); CC_GL_CHECK();
        ((GLDrawState*)Draw->internal)->rebindIBO = FALSE;
        ((GLDrawState*)Draw->internal)->rebindVBO = FALSE;
    }
    
    else CC_LOG_ERROR("Failed to create internal GL draw state due to allocation failure. Allocation size (%zu)", sizeof(GLDrawState));
}

static void GLDrawDestructor(GFXDraw Draw)
{
    CCAssertLog(Draw->internal, "GL implementation must not be null"); //TODO: handle if it is?
    
    glDeleteVertexArrays(1, &((GLDrawState*)Draw->internal)->vao); CC_GL_CHECK();
    
#if CC_GL_STATE_VERTEX_ARRAY_OBJECT
    if (CC_GL_CURRENT_STATE->bindVertexArray.array == ((GLDrawState*)Draw->internal)->vao) CC_GL_CURRENT_STATE->bindVertexArray.array = 0;
#endif
    
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

static void GLDrawSetUniformState(GLShader Shader, CCCollection(GFXDrawInputBuffer) Buffers)
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

static void GLDrawSetUniformTextureState(GLShader Shader, CCCollection(GFXDrawInputTexture) Textures)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Textures, &Enumerator);
    
    GLint TextureUnit = 0; //TODO: later loop through state to see if we can reuse without causing rebinding
    for (GFXDrawInputTexture *Uniform = CCCollectionEnumeratorGetCurrent(&Enumerator); Uniform; Uniform = CCCollectionEnumeratorNext(&Enumerator))
    {
        CC_GL_ACTIVE_TEXTURE(GL_TEXTURE0 + TextureUnit);
        CC_GL_BIND_TEXTURE_TARGET(GLTextureTarget(GFXTextureGetHints(Uniform->texture)), GLTextureGetID((GLTexture)Uniform->texture));
        
        GLShaderSetUniform(Shader, (GLShaderUniformInfo*)Uniform->input.shaderInput, 1, &TextureUnit);
        TextureUnit++;
    }
}

static void GLDrawSetFramebufferState(GLFramebuffer Framebuffer, GFXDraw Draw)
{
    const GLuint FBO = GLFramebufferGetID(Framebuffer, Draw->destination[0].index);
    CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, FBO);
    
    if ((GFXFramebuffer)Framebuffer != GFXFramebufferDefault())
    {
        GLenum DrawBuffers[sizeof(Draw->destination) / sizeof(typeof(*Draw->destination))];
        size_t Loop = 0;
        for ( ; (Draw->destination[Loop].index != SIZE_MAX) && (Loop < sizeof(Draw->destination) / sizeof(typeof(*Draw->destination))); Loop++)
        {
            CCAssertLog(FBO == GLFramebufferGetID(Framebuffer, Draw->destination[Loop].index), "Attachment belongs to another internal FBO");
            
            DrawBuffers[Loop] = GLFramebufferGetAttachmentIndex(Framebuffer, Draw->destination[Loop].index);
        }
        
        glDrawBuffers((GLsizei)Loop, DrawBuffers); CC_GL_CHECK();
    }
}

static GLenum GLDrawTypeFromBufferFormat(GFXBufferFormat Format)
{
    if (GFXBufferFormatIsInteger(Format))
    {
        switch (GFXBufferFormatGetBitSize(Format))
        {
            case 8:
                return GFXBufferFormatIsSigned(Format) ? GL_BYTE : GL_UNSIGNED_BYTE;
                
            case 16:
                return GFXBufferFormatIsSigned(Format) ? GL_SHORT : GL_UNSIGNED_SHORT;
                
            case 32:
                return GFXBufferFormatIsSigned(Format) ? GL_INT : GL_UNSIGNED_INT;
        }
    }
    
    else //float
    {
        switch (GFXBufferFormatGetBitSize(Format))
        {
            case 16:
                return GL_HALF_FLOAT;
                
            case 32:
                return GL_FLOAT;
                
            case 64:
                return GL_DOUBLE;
        }
    }
    
    CCAssertLog(0, "Unsupported format %d", Format);
}

static void GLDrawSetVertexBufferState(CCCollection(GFXDrawInputVertexBuffer) VertexBuffers)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(VertexBuffers, &Enumerator);
    
    for (GFXDrawInputVertexBuffer *Vertex = CCCollectionEnumeratorGetCurrent(&Enumerator); Vertex; Vertex = CCCollectionEnumeratorNext(&Enumerator))
    {
        const GLuint Index = ((GLShaderAttributeInfo*)Vertex->input.shaderInput)->location;
        
        if (Vertex->buffer)
        {
            glEnableVertexAttribArray(Index); CC_GL_CHECK();
            
            CC_GL_BIND_BUFFER(GL_ARRAY_BUFFER, ((GLBuffer)Vertex->buffer)->gl.buffer);
            glVertexAttribPointer(Index, (GLint)GFXBufferFormatGetElementCount(Vertex->format), GLDrawTypeFromBufferFormat(Vertex->format), Vertex->format & GFXBufferFormatNormalized ? GL_TRUE : GL_FALSE, (GLsizei)Vertex->stride, NULL + Vertex->offset); CC_GL_CHECK();
        }
        
        else
        {
            glDisableVertexAttribArray(Index); CC_GL_CHECK();
        }
    }
}

static GLenum GLDrawBlendFunc(GFXBlend Factor)
{
    switch (Factor)
    {
        case GFXBlendFactorZero:
            return GL_ZERO;
            
        case GFXBlendFactorOne:
            return GL_ONE;
            
        case GFXBlendFactorSourceColour:
            return GL_SRC_COLOR;
            
        case GFXBlendFactorOneMinusSourceColor:
            return GL_ONE_MINUS_SRC_COLOR;
            
        case GFXBlendFactorSourceAlpha:
            return GL_SRC_ALPHA;
            
        case GFXBlendFactorOneMinusSourceAlpha:
            return GL_ONE_MINUS_SRC_ALPHA;
            
        case GFXBlendFactorDestinationColor:
            return GL_DST_COLOR;
            
        case GFXBlendFactorOneMinusDestinationColor:
            return GL_ONE_MINUS_DST_COLOR;
            
        case GFXBlendFactorDestinationAlpha:
            return GL_DST_ALPHA;
            
        case GFXBlendFactorOneMinusDestinationAlpha:
            return GL_ONE_MINUS_DST_ALPHA;
            
        case GFXBlendFactorSourceAlphaSaturated:
            return GL_SRC_ALPHA_SATURATE;
            
        case GFXBlendFactorBlendColor:
            return GL_CONSTANT_COLOR;
            
        case GFXBlendFactorOneMinusBlendColor:
            return GL_ONE_MINUS_CONSTANT_COLOR;
            
        case GFXBlendFactorBlendAlpha:
            return GL_CONSTANT_ALPHA;
            
        case GFXBlendFactorOneMinusBlendAlpha:
            return GL_ONE_MINUS_CONSTANT_ALPHA;
            
        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported factor %d", Factor);
}

static GLenum GLDrawBlendOp(GFXBlend Op)
{
    switch (Op)
    {
        case GFXBlendOperationAdd:
            return GL_FUNC_ADD;
            
        case GFXBlendOperationSubtract:
            return GL_FUNC_SUBTRACT;
            
        case GFXBlendOperationReverseSubtract:
            return GL_FUNC_REVERSE_SUBTRACT;
            
        case GFXBlendOperationMin:
            return GL_MIN;
            
        case GFXBlendOperationMax:
            return GL_MAX;

        default:
            break;
    }
    
    CCAssertLog(0, "Unsupported operation %d", Op);
}

static void GLDrawSetBlendingState(GFXBlend BlendMask, GLuint Index)
{
    if (BlendMask != GFXBlendOpaque)
    {
        CC_GL_ENABLEi(GL_BLEND, Index);
        
        CC_GL_BLEND_FUNC_SEPARATEi(Index,
                                   GLDrawBlendFunc(GFXBlendGetFactor(BlendMask, GFXBlendComponentRGB, GFXBlendSource)),
                                   GLDrawBlendFunc(GFXBlendGetFactor(BlendMask, GFXBlendComponentRGB, GFXBlendDestination)),
                                   GLDrawBlendFunc(GFXBlendGetFactor(BlendMask, GFXBlendComponentAlpha, GFXBlendSource)),
                                   GLDrawBlendFunc(GFXBlendGetFactor(BlendMask, GFXBlendComponentAlpha, GFXBlendDestination)));
        
        CC_GL_BLEND_EQUATION_SEPARATEi(Index,
                                       GLDrawBlendOp(GFXBlendGetOperation(BlendMask, GFXBlendComponentRGB)),
                                       GLDrawBlendOp(GFXBlendGetOperation(BlendMask, GFXBlendComponentAlpha)));
    }
    
    else
    {
        CC_GL_DISABLEi(GL_BLEND, Index);
    }
}

static void GLDraw(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances, _Bool Indexed)
{
    CCAssertLog(Draw->internal, "GL implementation must not be null"); //TODO: handle if it is?
    CCAssertLog((GLint)Offset == Offset, "GL implementation cannot support offsets of that size");
    CCAssertLog((GLsizei)Count == Count, "GL implementation cannot support counts of that size");
    CCAssertLog((GLsizei)Instances == Instances, "GL implementation cannot support instances of that size");
    
    const GFXViewport Viewport = GFXDrawGetViewport(Draw);
    CC_GL_VIEWPORT(Viewport.x, Viewport.y, Viewport.width, Viewport.height);
    CC_GL_DEPTH_RANGE(Viewport.minDepth, Viewport.maxDepth);
    
    CC_GL_USE_PROGRAM(((GLShader)Draw->shader)->program);
    GLDrawSetUniformState((GLShader)Draw->shader, Draw->buffers);
    GLDrawSetUniformTextureState((GLShader)Draw->shader, Draw->textures);
    
    
    GLDrawSetFramebufferState((GLFramebuffer)Draw->framebuffer, Draw);
    for (size_t Loop = 0; (Draw->destination[Loop].index != SIZE_MAX) && (Loop < sizeof(Draw->destination) / sizeof(typeof(*Draw->destination))); Loop++)
    {
        GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Draw->framebuffer, Draw->destination[Loop].index);
        if ((Attachment->load == GFXFramebufferAttachmentActionClear) || (Attachment->load & GFXFramebufferAttachmentActionFlagClearOnce))
        {
            switch (Attachment->type)
            {
                case GFXFramebufferAttachmentTypeColour:
                    glClearBufferfv(GL_COLOR, (GLint)Loop, (GLfloat*)&Attachment->colour.clear); CC_GL_CHECK();
                    break;
            }
            
            Attachment->load &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        }
        
        GLDrawSetBlendingState(Draw->destination[Loop].blending, (GLint)Loop);
    }
    
    
    CC_GL_BIND_VERTEX_ARRAY(((GLDrawState*)Draw->internal)->vao);
    
    if (((GLDrawState*)Draw->internal)->rebindIBO)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Draw->index.buffer ? ((GLBuffer)Draw->index.buffer)->gl.buffer : 0); CC_GL_CHECK();
        ((GLDrawState*)Draw->internal)->rebindIBO = FALSE;
    }
    
    if (((GLDrawState*)Draw->internal)->rebindVBO)
    {
        GLDrawSetVertexBufferState(Draw->vertexBuffers); //TODO: optimize later to only do partial rebinding
        ((GLDrawState*)Draw->internal)->rebindVBO = FALSE;
    }
    
    
    if (Indexed)
    {
        glDrawElementsInstancedBaseVertex(GLDrawPrimitive(Primitive), (GLsizei)Count, GLDrawTypeFromBufferFormat(Draw->index.format), NULL, (GLsizei)Instances, (GLint)Offset); CC_GL_CHECK();
    }
    
    else
    {
        glDrawArraysInstanced(GLDrawPrimitive(Primitive), (GLint)Offset, (GLsizei)Count, (GLsizei)Instances); CC_GL_CHECK();
    }
    
    
    for (size_t Loop = 0; (Draw->destination[Loop].index != SIZE_MAX) && (Loop < sizeof(Draw->destination) / sizeof(typeof(*Draw->destination))); Loop++)
    {
        GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Draw->framebuffer, Draw->destination[Loop].index);
        if ((Attachment->store == GFXFramebufferAttachmentActionClear) || (Attachment->store & GFXFramebufferAttachmentActionFlagClearOnce))
        {
            switch (Attachment->type)
            {
                case GFXFramebufferAttachmentTypeColour:
                    glClearBufferfv(GL_COLOR, (GLint)Loop, (GLfloat*)&Attachment->colour.clear); CC_GL_CHECK();
                    break;
            }
            
            Attachment->store &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        }
    }
    
    CC_GL_BIND_VERTEX_ARRAY(0);
}

static void GLDrawSubmit(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances)
{
    GLDraw(Draw, Primitive, Offset, Count, Instances, FALSE);
}

static void GLDrawSubmitIndexed(GFXDraw Draw, GFXPrimitiveType Primitive, size_t Offset, size_t Count, size_t Instances)
{
    GLDraw(Draw, Primitive, Offset, Count, Instances, TRUE);
}

static void GLDrawSetIndexBuffer(GFXDraw Draw, GFXDrawIndexBuffer *IndexBuffer)
{
    ((GLDrawState*)Draw->internal)->rebindIBO = TRUE;
}

static void GLDrawSetVertexBuffer(GFXDraw Draw, GFXDrawInputVertexBuffer *VertexBuffer)
{
    ((GLDrawState*)Draw->internal)->rebindVBO = TRUE;
}

static void GLDrawSetShader(GFXDraw Draw, GFXShader Shader)
{
    ((GLDrawState*)Draw->internal)->rebindVBO = TRUE;
}
