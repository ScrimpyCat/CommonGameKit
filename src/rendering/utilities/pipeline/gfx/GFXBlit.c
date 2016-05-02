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

#include "GFXBlit.h"
#include "GFXMain.h"

static void GFXDrawDestructor(GFXBlit Blit)
{
    if (GFXMain->blit->optional.destroy) GFXMain->blit->optional.destroy(Blit);
    
    CC_SAFE_Free(Blit->source.framebuffer);
    CC_SAFE_Free(Blit->destination.framebuffer);
}

GFXBlit GFXBlitCreate(CCAllocatorType Allocator)
{
    GFXBlit Blit = CCMalloc(Allocator, sizeof(GFXBlitInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Blit)
    {
        CCMemorySetDestructor(Blit, (CCMemoryDestructorCallback)GFXDrawDestructor);
        
        *Blit = (GFXBlitInfo){
            .internal = NULL,
            .source = {
                .framebuffer = NULL,
                .index = 0,
                .region = { CCVector2DFill(0.0f), CCVector2DFill(0.0f) }
            },
            .destination = {
                .framebuffer = NULL,
                .index = 0,
                .region = { CCVector2DFill(0.0f), CCVector2DFill(0.0f) }
            },
            .filter = GFXTextureHintFilterModeNearest
        };
        
        if (GFXMain->blit->optional.create) GFXMain->blit->optional.create(Allocator, Blit);
    }
    
    else CC_LOG_ERROR("Failed to create blit command due to allocation failure. Allocation size (%zu)", sizeof(GFXBlitInfo));
    
    return Blit;
}

void GFXBlitDestroy(GFXBlit Blit)
{
    CCAssertLog(Blit, "Blit must not be null");
    
    CC_SAFE_Free(Blit);
}

void GFXBlitSubmit(GFXBlit Blit)
{
    CCAssertLog(Blit, "Blit must not be null");
    
    GFXMain->blit->submit(Blit);
}

void GFXBlitSetSource(GFXBlit Blit, GFXFramebuffer Framebuffer, size_t Index, CCRect Region)
{
    CCAssertLog(Blit, "Blit must not be null");
    
    CC_SAFE_Free(Blit->source.framebuffer);
    Blit->source = (GFXBlitFramebuffer){
        .framebuffer = CCRetain(Framebuffer),
        .index = Index,
        .region = Region
    };
    
    if (GFXMain->blit->optional.setSource) GFXMain->blit->optional.setSource(Blit, &Blit->source);
}

void GFXBlitSetDestination(GFXBlit Blit, GFXFramebuffer Framebuffer, size_t Index, CCRect Region)
{
    CCAssertLog(Blit, "Blit must not be null");
    
    CC_SAFE_Free(Blit->source.framebuffer);
    Blit->destination = (GFXBlitFramebuffer){
        .framebuffer = CCRetain(Framebuffer),
        .index = Index,
        .region = Region
    };
    
    if (GFXMain->blit->optional.setDestination) GFXMain->blit->optional.setDestination(Blit, &Blit->destination);
}

void GFXBlitSetFilterMode(GFXBlit Blit, GFXTextureHint FilterMode)
{
    CCAssertLog(Blit, "Blit must not be null");
    CCAssertLog((FilterMode == GFXTextureHintFilterModeNearest) || (FilterMode == GFXTextureHintFilterModeLinear), "Must be a filter mode");
    
    Blit->filter = FilterMode;
    if (GFXMain->blit->optional.setFilterMode) GFXMain->blit->optional.setFilterMode(Blit, FilterMode);
}
