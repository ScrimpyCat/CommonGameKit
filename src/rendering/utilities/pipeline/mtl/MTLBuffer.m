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
static _Bool BufferResize(MTLGFXBuffer Buffer, size_t Size);
static size_t BufferReadBuffer(MTLGFXBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data);
static size_t BufferWriteBuffer(MTLGFXBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data);
static size_t BufferCopyBuffer(MTLGFXBuffer SrcInternal, ptrdiff_t SrcOffset, size_t Size, MTLGFXBuffer DstInternal, ptrdiff_t DstOffset);


const GFXBufferInterface MTLBufferInterface = {
    .create = (GFXBufferConstructorCallback)BufferConstructor,
    .destroy = (GFXBufferDestructorCallback)BufferDestructor,
    .hints = (GFXBufferGetHintCallback)BufferGetHint,
    .size = (GFXBufferGetSizeCallback)BufferGetSize,
    .resize = (GFXBufferResizeCallback)BufferResize,
    .read = (GFXBufferReadBufferCallback)BufferReadBuffer,
    .write = (GFXBufferWriteBufferCallback)BufferWriteBuffer,
    .optional = {
        .copy = (GFXBufferCopyBufferCallback)BufferCopyBuffer
    }
};

static CC_CONSTANT_FUNCTION MTLResourceOptions BufferResourceOptions(GFXBufferHint Hint)
{
#if CC_PLATFORM_IOS
#define MTL_GFX_STORAGE_DEFAULT MTLResourceStorageModeShared
#define MTL_GFX_STORAGE_WRITE_ONLY MTLResourceStorageModeMemoryless
#elif CC_PLATFORM_OS_X
#define MTL_GFX_STORAGE_DEFAULT MTLResourceStorageModeManaged
#define MTL_GFX_STORAGE_WRITE_ONLY MTLResourceStorageModePrivate
#endif
    
    static const MTLResourceOptions Storage[2][2][2][2] = {
        // GFXBufferHintCPUReadNone
        {
            // GFXBufferHintCPUWriteNone
            {
                // GFXBufferHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_WRITE_ONLY  // GFXBufferHintGPUWrite*
                },
                // GFXBufferHintGPURead*
                {
                    MTLResourceStorageModePrivate,      // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_WRITE_ONLY  // GFXBufferHintGPUWrite*
                }
            },
            // GFXBufferHintCPUWrite*
            {
                // GFXBufferHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                },
                // GFXBufferHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                }
            }
        },
        // GFXBufferHintCPURead*
        {
            // GFXBufferHintCPUWriteNone
            {
                // GFXBufferHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                },
                // GFXBufferHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                }
            },
            // GFXBufferHintCPUWrite*
            {
                // GFXBufferHintGPUReadNone
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                },
                // GFXBufferHintGPURead*
                {
                    MTL_GFX_STORAGE_DEFAULT,    // GFXBufferHintGPUWriteNone
                    MTL_GFX_STORAGE_DEFAULT     // GFXBufferHintGPUWrite*
                }
            }
        }
    };
    
    const _Bool CPUWrite = (Hint >> GFXBufferHintCPUWrite) & GFXBufferHintAccessMask;
    const _Bool GPUWrite = (Hint >> GFXBufferHintGPUWrite) & GFXBufferHintAccessMask;
    const _Bool CPURead = (Hint >> GFXBufferHintCPURead) & GFXBufferHintAccessMask;
    const _Bool GPURead = (Hint >> GFXBufferHintGPURead) & GFXBufferHintAccessMask;
    
    return Storage[CPURead][CPUWrite][GPURead][GPUWrite];
}

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
        Buffer->size = Size;
        Buffer->buffer = (__bridge id<MTLBuffer>)((__bridge_retained CFTypeRef)[((MTLInternal*)MTLGFX->internal)->device newBufferWithBytes: Data length: Size options: BufferResourceOptions(Hint)]);
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

static size_t BufferGetSize(MTLGFXBuffer Buffer)
{
    return Buffer->size;
}

static _Bool BufferResize(MTLGFXBuffer Buffer, size_t Size)
{
    if (Buffer->buffer.length < Size)
    {
        id <MTLBuffer>NewBuffer = [((MTLInternal*)MTLGFX->internal)->device newBufferWithLength: Size options: BufferResourceOptions(Buffer->hint)];
        
        id <MTLBlitCommandEncoder>BlitEncoder = [((MTLInternal*)MTLGFX->internal)->commandBuffer blitCommandEncoder];
        [BlitEncoder copyFromBuffer: Buffer->buffer
                       sourceOffset: 0
                           toBuffer: NewBuffer
                  destinationOffset: 0
                               size: Buffer->size];
        
        [BlitEncoder endEncoding];
        
        CFRelease((__bridge CFTypeRef)Buffer->buffer);
        Buffer->buffer = (__bridge id<MTLBuffer>)((__bridge_retained CFTypeRef)NewBuffer);
    }
    
    Buffer->size = Size;
    
    return TRUE;
}

static size_t BufferReadBuffer(MTLGFXBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data)
{
    const size_t ReadSize = Buffer->size < Offset ? (Buffer->size - Offset > Size ? Size : Buffer->size - Offset) : 0;
    memcpy(Data, Buffer->buffer.contents + Offset, ReadSize);
    
    return ReadSize;
}

static size_t BufferWriteBuffer(MTLGFXBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data)
{
    const size_t WriteSize = Buffer->size < Offset ? (Buffer->size - Offset > Size ? Size : Buffer->size - Offset) : 0;
    memcpy(Buffer->buffer.contents + Offset, Data, WriteSize);
    
    [Buffer->buffer didModifyRange: NSMakeRange(Offset, WriteSize)];
    
    return WriteSize;
}

static size_t BufferCopyBuffer(MTLGFXBuffer SrcInternal, ptrdiff_t SrcOffset, size_t Size, MTLGFXBuffer DstInternal, ptrdiff_t DstOffset)
{
    size_t CopySize = SrcInternal->size < SrcOffset ? (SrcInternal->size - SrcOffset > Size ? Size : SrcInternal->size - SrcOffset) : 0;
    CopySize = DstInternal->size < DstOffset ? (DstInternal->size - DstOffset > CopySize ? CopySize : DstInternal->size - DstOffset) : 0;
    
    id <MTLBlitCommandEncoder>BlitEncoder = [((MTLInternal*)MTLGFX->internal)->commandBuffer blitCommandEncoder];
    [BlitEncoder copyFromBuffer: SrcInternal->buffer
                   sourceOffset: SrcOffset
                       toBuffer: DstInternal->buffer
              destinationOffset: DstOffset
                           size: CopySize];
    
    [BlitEncoder endEncoding];
    
    return CopySize;
}
