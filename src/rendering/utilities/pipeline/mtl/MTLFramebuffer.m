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

#import "MTLFramebuffer.h"
#import "MTLTexture.h"
@import ObjectiveC;

static MTLGFXFramebuffer FramebufferConstructor(CCAllocatorType Allocator, GFXFramebufferAttachment *Attachments, size_t Count);
static void FramebufferDestructor(MTLGFXFramebuffer Framebuffer);
static MTLGFXFramebuffer FramebufferDefault(void);
static GFXFramebufferAttachment *FramebufferGetAttachment(MTLGFXFramebuffer Framebuffer, size_t Index);

const GFXFramebufferInterface MTLFramebufferInterface = {
    .create = (GFXFramebufferConstructorCallback)FramebufferConstructor,
    .destroy = (GFXFramebufferDestructorCallback)FramebufferDestructor,
    .getDefault = (GFXFramebufferDefaultCallback)FramebufferDefault,
    .attachment = (GFXFramebufferGetAttachmentCallback)FramebufferGetAttachment,
};

static void FramebufferDestroy(MTLGFXFramebuffer Framebuffer)
{
    for (size_t Loop = 0; Loop < Framebuffer->attachmentCount; Loop++)
    {
        GFXTextureDestroy(Framebuffer->attachments[Loop].texture);
    }
}

static MTLGFXFramebuffer FramebufferConstructor(CCAllocatorType Allocator, GFXFramebufferAttachment *Attachments, size_t Count)
{
    CCAssertLog(Count, "Must have at least 1 attachment");
    
    MTLGFXFramebuffer Framebuffer = CCMalloc(Allocator, sizeof(MTLGFXFramebufferInfo) + (sizeof(GFXFramebufferAttachment) * Count), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Framebuffer)
    {
        CCMemorySetDestructor(Framebuffer, (CCMemoryDestructorCallback)FramebufferDestroy);
        
        Framebuffer->attachmentCount = Count;
        
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            Framebuffer->attachments[Loop] = Attachments[Loop];
        }
    }
    
    return Framebuffer;
}

static void FramebufferDestructor(MTLGFXFramebuffer Framebuffer)
{
    CCAssertLog(Framebuffer != FramebufferDefault(), "Cannot destroy default framebuffer");
    
    CC_SAFE_Free(Framebuffer);
}

static MTLGFXFramebuffer FramebufferDefault(void)
{
    static struct {
        int allocator;
        union {
            MTLGFXFramebufferInfo info;
            struct {
                size_t attachmentCount;
                GFXFramebufferAttachment attachments[1];
            } init;
        };
    } DefaultFramebuffer = {
        .allocator = -1,
        .init = {
            .attachmentCount = 1,
            .attachments = {
                (GFXFramebufferAttachment){
                    .type = GFXFramebufferAttachmentTypeColour,
                    .texture = NULL,
                    .load = GFXFramebufferAttachmentActionLoad,
                    .store = GFXFramebufferAttachmentActionStore,
                    .colour.clear = { 0.0f, 0.0f, 0.0f, 0.0f }
                }
            }
        }
    };
    
    if (!DefaultFramebuffer.init.attachmentCount) // TODO: make threadsafe
    {
        DefaultFramebuffer.init.attachments[0] = GFXFramebufferAttachmentCreateColour(((MTLInternal*)MTLGFX->internal)->drawable, GFXFramebufferAttachmentActionLoad, GFXFramebufferAttachmentActionStore, CCVector4DFill(0.0f));
        DefaultFramebuffer.init.attachmentCount = 1;
    }
    
    return &DefaultFramebuffer.info;
}

static GFXFramebufferAttachment *FramebufferGetAttachment(MTLGFXFramebuffer Framebuffer, size_t Index)
{
    CCAssertLog(Index < Framebuffer->attachmentCount, "Index must not exceed bounds");
    
    return &Framebuffer->attachments[Index];
}
