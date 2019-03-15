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

#import "MTLBlit.h"
#import "MTLTexture.h"

static void BlitSubmit(GFXBlit Blit);


const GFXBlitInterface MTLBlitInterface = {
    .submit = BlitSubmit,
    .optional = {
        .create = NULL,
        .destroy = NULL,
        .setSource = NULL,
        .setDestination = NULL,
        .setFilterMode = NULL
    }
};


static id <MTLRenderCommandEncoder>ClearEncoder(GFXFramebufferAttachment *Attachment)
{
    MTLRenderPassDescriptor *ClearDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    ClearDescriptor.colorAttachments[0].texture = MTLGFXTextureGetTexture((MTLGFXTexture)Attachment->texture);
    ClearDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
    ClearDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    
    return [((MTLInternal*)MTLGFX->internal)->commandBuffer renderCommandEncoderWithDescriptor: ClearDescriptor];
}

static _Bool ClearBuffer(GFXFramebufferAttachment *Attachment, GFXFramebufferAttachmentAction *Action, _Bool Cleared)
{
    if ((*Action == GFXFramebufferAttachmentActionClear) || (*Action & GFXFramebufferAttachmentActionFlagClearOnce))
    {
        if (!Cleared) [ClearEncoder(Attachment) endEncoding];
        
        *Action &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        return TRUE;
    }
    
    return FALSE;
}

static void BlitSubmit(GFXBlit Blit)
{
    CCAssertLog(((NSUInteger)Blit->source.region.size.x == (NSUInteger)Blit->destination.region.size.x) && ((NSUInteger)Blit->source.region.size.y == (NSUInteger)Blit->destination.region.size.y), "Source and destination sizes must match");
    
    GFXFramebufferAttachment *SrcAttachment = GFXFramebufferGetAttachment(Blit->source.framebuffer, Blit->source.index);
    ClearBuffer(SrcAttachment, &SrcAttachment->load, FALSE);
    
    GFXFramebufferAttachment *DstAttachment = GFXFramebufferGetAttachment(Blit->destination.framebuffer, Blit->destination.index);
    const _Bool DstCleared = ClearBuffer(DstAttachment, &DstAttachment->load, FALSE);
    
    if (!ClearBuffer(DstAttachment, &DstAttachment->store, DstCleared))
    {
        id <MTLBlitCommandEncoder>BlitEncoder = [((MTLInternal*)MTLGFX->internal)->commandBuffer blitCommandEncoder];
        [BlitEncoder copyFromTexture: MTLGFXTextureGetTexture((MTLGFXTexture)SrcAttachment->texture)
                         sourceSlice: 0
                         sourceLevel: 0
                        sourceOrigin: MTLOriginMake(Blit->source.region.position.x, Blit->source.region.position.y, 0)
                          sourceSize: MTLSizeMake(Blit->source.region.size.x, Blit->source.region.size.y, 1)
                           toTexture: MTLGFXTextureGetTexture((MTLGFXTexture)DstAttachment->texture)
                    destinationSlice: 0
                    destinationLevel: 0
                   destinationOrigin: MTLOriginMake(Blit->destination.region.position.x, Blit->destination.region.position.y, 0)];
        
        [BlitEncoder endEncoding];
    }
}
