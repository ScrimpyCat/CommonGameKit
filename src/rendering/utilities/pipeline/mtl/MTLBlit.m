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

#define CC_QUICK_COMPILE
#import "MTLBlit.h"
#import "MTLTexture.h"
#import "MTLFramebuffer.h"
#import "MTLCommandBuffer.h"
#import "AssetManager.h"

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


id <MTLRenderCommandEncoder>MTLGFXClearEncoder(GFXFramebufferAttachment *Attachment)
{
    MTLRenderPassDescriptor *ClearDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    ClearDescriptor.colorAttachments[0].texture = MTLGFXFramebufferAttachmentGetTexture(Attachment);
    ClearDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(Attachment->colour.clear.r, Attachment->colour.clear.g, Attachment->colour.clear.b, Attachment->colour.clear.a);
    ClearDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    
    return [((MTLGFXCommandBuffer)GFXCommandBufferRecording())->commandBuffer renderCommandEncoderWithDescriptor: ClearDescriptor];
}

static _Bool ClearBuffer(GFXFramebufferAttachment *Attachment, GFXFramebufferAttachmentAction *Action, _Bool Cleared)
{
    if ((*Action == GFXFramebufferAttachmentActionClear) || (*Action & GFXFramebufferAttachmentActionFlagClearOnce))
    {
        if (!Cleared) [MTLGFXClearEncoder(Attachment) endEncoding];
        
        *Action &= ~GFXFramebufferAttachmentActionFlagClearOnce;
        return TRUE;
    }
    
    return FALSE;
}

static void BlitSubmit(GFXBlit Blit)
{
    @autoreleasepool {
        CCAssertLog(((NSUInteger)Blit->source.region.size.x == (NSUInteger)Blit->destination.region.size.x) && ((NSUInteger)Blit->source.region.size.y == (NSUInteger)Blit->destination.region.size.y), "Source and destination sizes must match");
        
        GFXFramebufferAttachment *SrcAttachment = GFXFramebufferGetAttachment(Blit->source.framebuffer, Blit->source.index);
        ClearBuffer(SrcAttachment, &SrcAttachment->load, FALSE);
        
        GFXFramebufferAttachment *DstAttachment = GFXFramebufferGetAttachment(Blit->destination.framebuffer, Blit->destination.index);
        const _Bool DstCleared = ClearBuffer(DstAttachment, &DstAttachment->load, FALSE);
        
        if (!ClearBuffer(DstAttachment, &DstAttachment->store, DstCleared))
        {
            id <MTLTexture>Src = MTLGFXFramebufferAttachmentGetTexture(SrcAttachment), Dst = MTLGFXFramebufferAttachmentGetTexture(DstAttachment);
            if ((Src.pixelFormat == Dst.pixelFormat) && (CCVector2Equal(Blit->source.region.size, Blit->destination.region.size)))
            {
                id <MTLBlitCommandEncoder>BlitEncoder = [((MTLGFXCommandBuffer)GFXCommandBufferRecording())->commandBuffer blitCommandEncoder];
                [BlitEncoder copyFromTexture: Src
                                 sourceSlice: 0
                                 sourceLevel: 0
                                sourceOrigin: MTLOriginMake(Blit->source.region.position.x, Blit->source.region.position.y, 0)
                                  sourceSize: MTLSizeMake(Blit->source.region.size.x, Blit->source.region.size.y, 1)
                                   toTexture: Dst
                            destinationSlice: 0
                            destinationLevel: 0
                           destinationOrigin: MTLOriginMake(Blit->destination.region.position.x, Blit->destination.region.position.y, 0)];
                
                [BlitEncoder endEncoding];
            }
            
            else
            {
                CCAssertLog(CCVector2Equal(Blit->source.region.size, Blit->destination.region.size) || ((Blit->filter & GFXTextureHintFilterModeMask) == (GFXTextureGetHints(SrcAttachment->texture) & GFXTextureHintFilterModeMask)), "Blit filter mode must be the same as the source texture");
                
                GFXDraw Drawer = GFXDrawCreate(CC_STD_ALLOCATOR);
                
                GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("texture2d"));
                GFXDrawSetShader(Drawer, Shader);
                GFXShaderDestroy(Shader);
                
                const CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, Dst.width, 0.0f, Dst.height, -1.0f, 1.0f);
                GFXBuffer Projection = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
                GFXDrawSetBuffer(Drawer, CC_STRING("modelViewProjectionMatrix"), Projection);
                GFXBufferDestroy(Projection);
                
                const CCVector2D SrcSize = CCVector2DMake(Src.width, Src.height);
                const CCRect DstRect = Blit->destination.region, SrcRect = { .position = CCVector2Div(Blit->source.region.position, SrcSize), .size = CCVector2Div(Blit->source.region.size, SrcSize) };
                struct {
                    CCVector2D position;
                    CCVector2D coord;
                } VertData[4] = {
                    { .position = DstRect.position, .coord = SrcRect.position },
                    { .position = CCVector2DMake(DstRect.position.x + DstRect.size.x, DstRect.position.y), .coord = CCVector2DMake(SrcRect.position.x + SrcRect.size.x, SrcRect.position.y) },
                    { .position = CCVector2DMake(DstRect.position.x, DstRect.position.y + DstRect.size.y), .coord = CCVector2DMake(SrcRect.position.x, SrcRect.position.y + SrcRect.size.y) },
                    { .position = CCVector2DMake(DstRect.position.x + DstRect.size.x, DstRect.position.y + DstRect.size.y), .coord = CCVector2DMake(SrcRect.position.x + SrcRect.size.x, SrcRect.position.y + SrcRect.size.y) }
                };
                
                GFXBuffer VertBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(VertData), &VertData);
                GFXDrawSetVertexBuffer(Drawer, CC_STRING("vPosition"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(typeof(*VertData)), offsetof(typeof(*VertData), position));
                GFXDrawSetVertexBuffer(Drawer, CC_STRING("vTexCoord"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(typeof(*VertData)), offsetof(typeof(*VertData), coord));
                GFXBufferDestroy(VertBuffer);
                
                GFXDrawSetTexture(Drawer, CC_STRING("tex"), SrcAttachment->texture);
                GFXDrawSetFramebuffer(Drawer, Blit->destination.framebuffer, Blit->destination.index);
                GFXDrawSubmit(Drawer, GFXPrimitiveTypeTriangleStrip, 0, 4);
                
                GFXDrawDestroy(Drawer);
            }
        }
    }
}
