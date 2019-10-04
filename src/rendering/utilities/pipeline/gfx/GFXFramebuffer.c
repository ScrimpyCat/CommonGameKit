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
#include "GFXFramebuffer.h"
#include "GFXMain.h"

GFXFramebuffer GFXFramebufferCreate(CCAllocatorType Allocator, GFXFramebufferAttachment *Attachments, size_t Count)
{
    return GFXMain->framebuffer->create(Allocator, Attachments, Count);
}

void GFXFramebufferDestroy(GFXFramebuffer Framebuffer)
{
    CCAssertLog(Framebuffer, "Framebuffer must not be null");
    
    GFXMain->framebuffer->destroy(Framebuffer);
}

GFXFramebuffer GFXFramebufferDefault(void)
{
    return GFXMain->framebuffer->getDefault();
}

GFXFramebufferAttachment *GFXFramebufferGetAttachment(GFXFramebuffer Framebuffer, size_t Index)
{
    CCAssertLog(Framebuffer, "Framebuffer must not be null");
    
    return GFXMain->framebuffer->attachment(Framebuffer, Index);
}
