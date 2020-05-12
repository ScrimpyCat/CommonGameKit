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
#import "MTLCommandBuffer.h"

static MTLGFXCommandBuffer MTLCommandBufferConstructor(CCAllocatorType Allocator);
static void MTLCommandBufferDestructor(MTLGFXCommandBuffer CommandBuffer);
static void MTLCommandBufferCommit(MTLGFXCommandBuffer CommandBuffer, _Bool Present);
static _Bool MTLCommandBufferCompleted(MTLGFXCommandBuffer CommandBuffer);

const GFXCommandBufferInterface MTLCommandBufferInterface = {
    .create = (GFXCommandBufferConstructorCallback)MTLCommandBufferConstructor,
    .destroy = (GFXCommandBufferDestructorCallback)MTLCommandBufferDestructor,
    .commit = (GFXCommandBufferCommitCallback)MTLCommandBufferCommit,
    .completed = (GFXCommandBufferCompletedCallback)MTLCommandBufferCompleted
};


static void CommandBufferDestroy(MTLGFXCommandBuffer CommandBuffer)
{
    if (CommandBuffer->commandBuffer)
    {
        CFRelease((__bridge CFTypeRef)CommandBuffer->commandBuffer);
    }
}

static MTLGFXCommandBuffer MTLCommandBufferConstructor(CCAllocatorType Allocator)
{
    MTLGFXCommandBuffer CommandBuffer = CCMalloc(Allocator, sizeof(MTLGFXCommandBufferInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (CommandBuffer)
    {
        CCMemorySetDestructor(CommandBuffer, (CCMemoryDestructorCallback)CommandBufferDestroy);
        
        @autoreleasepool {
            CommandBuffer->commandBuffer = (__bridge id<MTLCommandBuffer>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->commandQueue commandBuffer]);
        }
    }
    
    return CommandBuffer;
}

static void MTLCommandBufferDestructor(MTLGFXCommandBuffer CommandBuffer)
{
    @autoreleasepool {
        CC_SAFE_Free(CommandBuffer);
    }
}

static _Bool MTLCommandBufferCompleted(MTLGFXCommandBuffer CommandBuffer)
{
    @autoreleasepool {
        switch (CommandBuffer->commandBuffer.status)
        {
            case MTLCommandBufferStatusCompleted:
            case MTLCommandBufferStatusError:
                return TRUE;
                
            default:
                return FALSE;
        }
    }
}

static void MTLCommandBufferCommit(MTLGFXCommandBuffer CommandBuffer, _Bool Present)
{
    @autoreleasepool {
        if (Present) [CommandBuffer->commandBuffer presentDrawable: MTLGFXGetDrawable()];
        
        [CommandBuffer->commandBuffer commit];
    }
}
