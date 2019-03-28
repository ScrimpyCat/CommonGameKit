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

#import "MTLBuffer.h"

static MTLGFXBuffer BufferConstructor(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data);
static void BufferDestructor(MTLGFXBuffer Buffer);
static GFXBufferHint BufferGetHint(MTLGFXBuffer Buffer);
static size_t BufferGetSize(MTLGFXBuffer Buffer);


const GFXBufferInterface MTLBufferInterface = {
    .create = (GFXBufferConstructorCallback)BufferConstructor,
    .destroy = (GFXBufferDestructorCallback)BufferDestructor,
    .hints = (GFXBufferGetHintCallback)BufferGetHint,
    .size = (GFXBufferGetSizeCallback)BufferGetSize,
};

static void BufferDestroy(MTLGFXBuffer Buffer)
{
    CFRelease((__bridge CFTypeRef)Buffer->buffer);
}

static MTLGFXBuffer BufferConstructor(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data)
{
    MTLGFXBuffer Buffer = CCMalloc(Allocator, sizeof(MTLGFXBufferInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Buffer)
    {
        CCMemorySetDestructor(Buffer, (CCMemoryDestructorCallback)BufferDestroy);
        
        Buffer->hint = Hint;
        Buffer->buffer = (__bridge id<MTLBuffer>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->device newBufferWithBytes: Data length: Size options: 0]);
    }
    
    return Buffer;
}

static void BufferDestructor(MTLGFXBuffer Buffer)
{
    CC_SAFE_Free(Buffer);
}

static GFXBufferHint BufferGetHint(MTLGFXBuffer Buffer)
{
    return Buffer->hint;
}

static size_t GLBufferGetSize(MTLGFXBuffer Buffer)
{
    return Buffer->buffer.length;
}
