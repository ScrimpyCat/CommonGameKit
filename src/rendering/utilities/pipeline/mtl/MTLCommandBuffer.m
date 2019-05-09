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

#import "MTLCommandBuffer.h"

static MTLGFXCommandBuffer MTLCommandBufferConstructor(CCAllocatorType Allocator);
static void MTLCommandBufferDestructor(MTLGFXCommandBuffer CommandBuffer);
static void MTLCommandBufferSetPresentable(MTLGFXCommandBuffer CommandBuffer, _Bool Presentable);
static void MTLCommandBufferCommit(MTLGFXCommandBuffer CommandBuffer);

const GFXCommandBufferInterface MTLCommandBufferInterface = {
    .create = (GFXCommandBufferConstructorCallback)MTLCommandBufferConstructor,
    .destroy = (GFXCommandBufferDestructorCallback)MTLCommandBufferDestructor,
    .setPresentable = (GFXCommandBufferSetPresentableCallback)MTLCommandBufferSetPresentable,
    .commit = (GFXCommandBufferCommitCallback)MTLCommandBufferCommit
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
        
        CommandBuffer->presentable = FALSE;
        CommandBuffer->commandBuffer = (__bridge id<MTLCommandBuffer>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->commandQueue commandBuffer]);
    }
    
    return CommandBuffer;
}

static void MTLCommandBufferDestructor(MTLGFXCommandBuffer CommandBuffer)
{
    CC_SAFE_Free(CommandBuffer);
}

static void MTLCommandBufferSetPresentable(MTLGFXCommandBuffer CommandBuffer, _Bool Presentable)
{
    CommandBuffer->presentable = Presentable;
}

static void MTLCommandBufferCommit(MTLGFXCommandBuffer CommandBuffer)
{
    if (CommandBuffer->presentable) [CommandBuffer->commandBuffer presentDrawable: MTLGFXGetDrawable()];
    
    [CommandBuffer->commandBuffer commit];
}

