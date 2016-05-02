//
//  GLBlit.c
//  Blob Game
//
//  Created by Stefan Johnson on 2/05/2016.
//  Copyright © 2016 Stefan Johnson. All rights reserved.
//

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
}
