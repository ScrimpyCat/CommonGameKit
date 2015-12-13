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

#include "GLFramebuffer.h"

static GLFramebuffer GLFramebufferConstructor(CCAllocatorType Allocator, GFXFramebufferAttachment *Attachments, size_t Count);
static void GLFramebufferDestructor(GLFramebuffer Framebuffer);
static GFXFramebufferAttachment *GLFramebufferGetAttachment(GLFramebuffer Framebuffer, size_t Index);

const GFXFramebufferInterface GLFramebufferInterface = {
    .create = (GFXFramebufferConstructorCallback)GLFramebufferConstructor,
    .destroy = (GFXFramebufferDestructorCallback)GLFramebufferDestructor,
    .attachment = (GFXFramebufferGetAttachmentCallback)GLFramebufferGetAttachment,
};

static GLFBO *GLFBOCreate(CCAllocatorType Allocator, GLuint FBO, GFXFramebufferAttachment *Attachments, size_t Count)
{
    GLFBO *Framebuffer = CCMalloc(Allocator, sizeof(GLFBO) + (sizeof(GFXFramebufferAttachment) * Count), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Framebuffer)
    {
        Framebuffer->fbo = FBO;
        Framebuffer->attachmentCount = Count;
        
        CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, FBO);
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            Framebuffer->attachments[Loop] = Attachments[Loop];
            
            switch (GFXTextureGetHints(Attachments[Loop].texture) & GFXTextureHintDimensionMask)
            {
                case GFXTextureHintDimension1D:
                    glFramebufferTexture1D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)Loop, GL_TEXTURE_1D, GLTextureGetID((GLTexture)Attachments[Loop].texture), 0); CC_GL_CHECK();
                    break;
                    
                case GFXTextureHintDimension2D:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)Loop, GL_TEXTURE_2D, GLTextureGetID((GLTexture)Attachments[Loop].texture), 0); CC_GL_CHECK();
                    break;
                    
                case GFXTextureHintDimension3D:
                    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)Loop, GL_TEXTURE_3D, GLTextureGetID((GLTexture)Attachments[Loop].texture), 0, 0); CC_GL_CHECK(); //TODO: Will need to provide the zoffset
                    break;
            }
        }
        
        
        GLuint Status = glCheckFramebufferStatus(GL_FRAMEBUFFER); CC_GL_CHECK();
        if (Status != GL_FRAMEBUFFER_COMPLETE)
        {
            switch (Status)
            {
#ifdef GL_FRAMEBUFFER_UNDEFINED
                case GL_FRAMEBUFFER_UNDEFINED:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_UNDEFINED");
                    break;
#endif
                    
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
                    break;
                    
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
                    break;
                    
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
                    break;
#endif
                    
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
                    break;
#endif
                    
                case GL_FRAMEBUFFER_UNSUPPORTED:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_UNSUPPORTED");
                    break;
                    
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
                case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                    CC_LOG_ERROR("Failed to create FBO: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
                    break;
#endif
            }
            
            CCAssertLog(0, "Failed to create FBO.");
        }
    }
    
    return Framebuffer;
}

static void GLFBODestroy(GLFBO *Framebuffer)
{
    for (size_t Loop = 0; Loop < Framebuffer->attachmentCount; Loop++)
    {
        GFXTextureDestroy(Framebuffer->attachments[Loop].texture);
    }
    
    CC_SAFE_Free(Framebuffer);
}

static GFXFramebufferAttachment *GLFBOGetAttachment(GLFBO *Framebuffer, size_t Index)
{
    CCAssertLog(Index < Framebuffer->attachmentCount, "Index must not exceed bounds");
    
    return &Framebuffer->attachments[Index];
}

static GLFramebuffer GLFramebufferConstructor(CCAllocatorType Allocator, GFXFramebufferAttachment *Attachments, size_t Count)
{
    CCAssertLog(Count, "Must have at least 1 attachment");
    
    size_t Amount = Count / ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments;
    if (Count % ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments == 0) Amount--;
    const size_t FBOCount = Amount + 1;
    
    GLFramebuffer Framebuffer = CCMalloc(Allocator, sizeof(GLFramebuffer) + (sizeof(GLFBO*) * FBOCount), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Framebuffer)
    {
        GLuint FramebufferObjects[FBOCount];
        glGenFramebuffers((GLsizei)FBOCount, FramebufferObjects); CC_GL_CHECK();
        
        Framebuffer->fboCount = FBOCount;
        
        for (size_t Loop = 0; Loop < Amount; Loop++)
        {
            Framebuffer->framebuffers[Loop] = GLFBOCreate(Allocator, FramebufferObjects[Loop], Attachments, ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments);
            Attachments += ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments;
            Count -= ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments;
        }
        
        Framebuffer->framebuffers[Amount] = GLFBOCreate(Allocator, FramebufferObjects[Amount], Attachments, Count);
    }
    
    return Framebuffer;
}

static void GLFramebufferDestructor(GLFramebuffer Framebuffer)
{
    CCAssertLog(Framebuffer, "Framebuffer must not be null");
    
    for (size_t Loop = 0; Loop < Framebuffer->fboCount; Loop++)
    {
        GLFBODestroy(Framebuffer->framebuffers[Loop]);
    }
    
    CC_SAFE_Free(Framebuffer);
}

static GFXFramebufferAttachment *GLFramebufferGetAttachment(GLFramebuffer Framebuffer, size_t Index)
{
    CCAssertLog(Framebuffer, "Framebuffer must not be null");
    
    size_t FBOIndex = Index / ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments;
    
    CCAssertLog(FBOIndex < Framebuffer->fboCount, "Index must not exceed bounds");
    
    return GLFBOGetAttachment(Framebuffer->framebuffers[FBOIndex], Index - (FBOIndex * ((GLInternal*)GLGFX->internal)->limits.maxColourAttachments));
}
