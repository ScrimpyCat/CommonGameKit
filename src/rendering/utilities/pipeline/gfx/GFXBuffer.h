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

#ifndef Blob_Game_GFXBuffer_h
#define Blob_Game_GFXBuffer_h

#include <CommonC/Common.h>

typedef enum {
    GFXBufferHintAccessMask = 3,
    GFXBufferHintAccessNone = 0,
    GFXBufferHintAccessOnce = 1,
    GFXBufferHintAccessMany = 2,
    GFXBufferHintCPURead = 8,   //000000xx 00000000
    GFXBufferHintCPUWrite = 10, //0000xx00 00000000
    GFXBufferHintGPURead = 12,  //00xx0000 00000000
    GFXBufferHintGPUWrite = 14, //xx000000 00000000
    /*
     Access hints are shifted by the operation type. 
     e.g. (GFXBufferHintAccessOnce << GFXBufferHintCPUWrite) | (GFXBufferHintAccessMany << GFXBufferHintGPURead)
     Will specify a buffer usage where the CPU will write to the buffer once, and the GPU will read from
     the buffer many times. And the CPU won't read from the buffer, and the GPU won't write to the buffer.
     */
    GFXBufferHintCPUReadMask = (GFXBufferHintAccessMask << GFXBufferHintCPURead),
    GFXBufferHintCPUReadNone = 0,
    GFXBufferHintCPUReadOnce = (GFXBufferHintAccessOnce << GFXBufferHintCPURead),
    GFXBufferHintCPUReadMany = (GFXBufferHintAccessMany << GFXBufferHintCPURead),
    
    GFXBufferHintCPUWriteMask = (GFXBufferHintAccessMask << GFXBufferHintCPUWrite),
    GFXBufferHintCPUWriteNone = 0,
    GFXBufferHintCPUWriteOnce = (GFXBufferHintAccessOnce << GFXBufferHintCPUWrite),
    GFXBufferHintCPUWriteMany = (GFXBufferHintAccessMany << GFXBufferHintCPUWrite),
    
    GFXBufferHintGPUReadMask = (GFXBufferHintAccessMask << GFXBufferHintGPURead),
    GFXBufferHintGPUReadNone = 0,
    GFXBufferHintGPUReadOnce = (GFXBufferHintAccessOnce << GFXBufferHintGPURead),
    GFXBufferHintGPUReadMany = (GFXBufferHintAccessMany << GFXBufferHintGPURead),
    
    GFXBufferHintGPUWriteMask = (GFXBufferHintAccessMask << GFXBufferHintGPUWrite),
    GFXBufferHintGPUWriteNone = 0,
    GFXBufferHintGPUWriteOnce = (GFXBufferHintAccessOnce << GFXBufferHintGPUWrite),
    GFXBufferHintGPUWriteMany = (GFXBufferHintAccessMany << GFXBufferHintGPUWrite),

    
    GFXBufferHintDataMask = 0xff,
    GFXBufferHintData = 0,
    GFXBufferHintDataVertex,
    GFXBufferHintDataIndex,
    GFXBufferHintDataUniform,
    GFXBufferHintDataTexture
} GFXBufferHint;


typedef struct GFXBuffer *GFXBuffer;


GFXBuffer GFXBufferCreate(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data);
void GFXBufferDestroy(GFXBuffer Buffer);
GFXBufferHint GFXBufferGetHints(GFXBuffer Buffer);
size_t GFXBufferGetSize(GFXBuffer Buffer);
void GFXBufferInvalidate(GFXBuffer Buffer);
size_t GFXBufferReadBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data);
size_t GFXBufferWriteBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data);
size_t GFXBufferCopyBuffer(GFXBuffer SrcBuffer, ptrdiff_t SrcOffset, size_t Size, GFXBuffer DstBuffer, ptrdiff_t DstOffset);
size_t GFXBufferFillBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, uint8_t Fill);

#endif
