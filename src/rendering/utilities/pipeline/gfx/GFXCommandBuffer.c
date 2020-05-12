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
#include "GFXCommandBuffer.h"
#include "GFXMain.h"
#include <threads.h>

GFXCommandBuffer GFXCommandBufferCreate(CCAllocatorType Allocator)
{
    return GFXMain->commandBuffer->create(Allocator);
}

void GFXCommandBufferDestroy(GFXCommandBuffer CommandBuffer)
{
    CCAssertLog(CommandBuffer, "CommandBuffer must not be null");
    
    GFXMain->commandBuffer->destroy(CommandBuffer);
}

_Bool GFXCommandBufferIsComplete(GFXCommandBuffer CommandBuffer)
{
    CCAssertLog(CommandBuffer, "CommandBuffer must not be null");
    
    return GFXMain->commandBuffer->completed(CommandBuffer);
}

static _Thread_local GFXCommandBuffer Recording = NULL;

void GFXCommandBufferRecord(GFXCommandBuffer CommandBuffer)
{
    if (Recording) GFXCommandBufferDestroy(Recording);
    
    Recording = CCRetain(CommandBuffer);
}

void GFXCommandBufferCommit(GFXCommandBuffer CommandBuffer, _Bool Present)
{
    CCAssertLog(CommandBuffer, "CommandBuffer must not be null");
    
    if (CommandBuffer == Recording)
    {
        GFXCommandBufferDestroy(Recording);
        Recording = NULL;
    }
    
    GFXMain->commandBuffer->commit(CommandBuffer, Present);
    
    GFXCommandBufferDestroy(CommandBuffer);
}

GFXCommandBuffer GFXCommandBufferRecording(void)
{
    return Recording;
}

