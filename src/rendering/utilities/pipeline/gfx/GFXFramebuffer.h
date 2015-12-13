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

#ifndef Blob_Game_GFXFramebuffer_h
#define Blob_Game_GFXFramebuffer_h

#include "GFXTexture.h"
#include <CommonC/Common.h>

typedef enum {
    GFXFramebufferAttachmentActionDontCare,
    GFXFramebufferAttachmentActionClear,
    GFXFramebufferAttachmentActionLoad,
    GFXFramebufferAttachmentActionStore
} GFXFramebufferAttachmentAction;

typedef enum {
    GFXFramebufferAttachmentTypeColour,
//    GFXFramebufferAttachmentTypeDepth,
//    GFXFramebufferAttachmentTypeStencil
} GFXFramebufferAttachmentType;

typedef struct GFXFramebufferAttachment {
    const GFXTexture texture;
    const GFXFramebufferAttachmentType type;
    GFXFramebufferAttachmentAction load;
    GFXFramebufferAttachmentAction store;
    union {
        struct {
            CCColourRGBA clear;
        } colour;
    };
} GFXFramebufferAttachment;


typedef struct GFXFramebuffer *GFXFramebuffer;

static inline GFXFramebufferAttachment GFXFramebufferAttachmentCreateColour(GFXTexture Texture, GFXFramebufferAttachmentAction Load, GFXFramebufferAttachmentAction Store, CCColourRGBA ClearColour);

GFXFramebuffer GFXFramebufferCreate(CCAllocatorType Allocator, GFXFramebufferAttachment Attachments, size_t Count); //takes ownership of textures in attachments
void GFXFramebufferDestroy(GFXFramebuffer Framebuffer);
GFXFramebufferAttachment *GFXFramebufferGetAttachment(GFXFramebuffer Framebuffer, size_t Index);


static inline GFXFramebufferAttachment GFXFramebufferAttachmentCreateColour(GFXTexture Texture, GFXFramebufferAttachmentAction Load, GFXFramebufferAttachmentAction Store, CCColourRGBA ClearColour)
{
    return (GFXFramebufferAttachment){
        .type = GFXFramebufferAttachmentTypeColour,
        .texture = Texture,
        .load = Load,
        .store = Store,
        .colour.clear = ClearColour
    };
}

#endif
