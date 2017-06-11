/*
 *  Copyright (c) 2016, Stefan Johnson
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

#include "GLBlit.h"
#include "GLTexture.h"
#include "GLFramebuffer.h"

static void GLBlitSubmit(GFXBlit Blit);


const GFXBlitInterface GLBlitInterface = {
    .submit = GLBlitSubmit,
    .optional = {
        .create = NULL,
        .destroy = NULL,
        .setSource = NULL,
        .setDestination = NULL,
        .setFilterMode = NULL
    }
};


static void GLBlitSetSourceFramebufferState(GLFramebuffer Framebuffer, size_t Index)
{
    CC_GL_BIND_FRAMEBUFFER(GL_READ_FRAMEBUFFER, GLFramebufferGetID(Framebuffer, Index));
#ifdef GL_COLOR_ATTACHMENT1
    if ((GFXFramebuffer)Framebuffer != GFXFramebufferDefault())
    {
        glReadBuffer(GLFramebufferGetAttachmentIndex(Framebuffer, Index)); CC_GL_CHECK();
    }
#endif
    
    GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment((GFXFramebuffer)Framebuffer, Index);
    if ((Attachment->load == GFXFramebufferAttachmentActionClear) || (Attachment->load & GFXFramebufferAttachmentActionFlagClearOnce))
    {
        CC_GL_BIND_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER, GLFramebufferGetID(Framebuffer, Index));
#ifdef GL_COLOR_ATTACHMENT1
        if ((GFXFramebuffer)Framebuffer != GFXFramebufferDefault())
        {
            glDrawBuffers(1, &(GLenum){ GLFramebufferGetAttachmentIndex(Framebuffer, Index) }); CC_GL_CHECK();
        }
#endif
        
        CC_GL_CLEAR_COLOR(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
        
        Attachment->load &= ~GFXFramebufferAttachmentActionFlagClearOnce;
    }
}

static void GLBlitSetDestinationFramebufferState(GLFramebuffer Framebuffer, size_t Index)
{
    CC_GL_BIND_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER, GLFramebufferGetID(Framebuffer, Index));
#ifdef GL_COLOR_ATTACHMENT1
    if ((GFXFramebuffer)Framebuffer != GFXFramebufferDefault())
    {
        glDrawBuffers(1, &(GLenum){ GLFramebufferGetAttachmentIndex(Framebuffer, Index) }); CC_GL_CHECK();
    }
#endif
    
    GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment((GFXFramebuffer)Framebuffer, Index);
    if ((Attachment->load == GFXFramebufferAttachmentActionClear) || (Attachment->load & GFXFramebufferAttachmentActionFlagClearOnce))
    {
        CC_GL_CLEAR_COLOR(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
        
        Attachment->load &= ~GFXFramebufferAttachmentActionFlagClearOnce;
    }
}

static inline CC_CONSTANT_FUNCTION GLenum GLBlitFilter(GFXTextureHint FilterMode)
{
    return (GLenum[2]){
        GL_NEAREST,
        GL_LINEAR
    }[FilterMode & GFXTextureHintFilterModeMask];
}

static void GLBlitSubmit(GFXBlit Blit)
{
    GLBlitSetSourceFramebufferState((GLFramebuffer)Blit->source.framebuffer, Blit->source.index);
    GLBlitSetDestinationFramebufferState((GLFramebuffer)Blit->destination.framebuffer, Blit->destination.index);
    
    glBlitFramebuffer(Blit->source.region.position.x,
                      Blit->source.region.position.y,
                      Blit->source.region.position.x + Blit->source.region.size.x,
                      Blit->source.region.position.y + Blit->source.region.size.y,
                      Blit->destination.region.position.x,
                      Blit->destination.region.position.y,
                      Blit->destination.region.position.x + Blit->destination.region.size.x,
                      Blit->destination.region.position.y + Blit->destination.region.size.y, GL_COLOR_BUFFER_BIT, GLBlitFilter(Blit->filter)); CC_GL_CHECK();
    
    GFXFramebufferAttachment *Attachment = GFXFramebufferGetAttachment(Blit->destination.framebuffer, Blit->destination.index);
    if ((Attachment->store == GFXFramebufferAttachmentActionClear) || (Attachment->store & GFXFramebufferAttachmentActionFlagClearOnce))
    {
        CC_GL_CLEAR_COLOR(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
        
        Attachment->store &= ~GFXFramebufferAttachmentActionFlagClearOnce;
    }
}
