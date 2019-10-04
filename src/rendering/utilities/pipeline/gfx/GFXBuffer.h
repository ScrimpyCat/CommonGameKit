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

#ifndef CommonGameKit_GFXBuffer_h
#define CommonGameKit_GFXBuffer_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/GFXBufferFormat.h>

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
    
    /// Equivalent to not passing any access options, an unknown access leaves the interpretation completely up to the implementation.
    GFXBufferHintAccessUnknown = GFXBufferHintCPUReadNone | GFXBufferHintCPUWriteNone | GFXBufferHintGPUReadNone | GFXBufferHintGPUWriteNone,
    
    
    GFXBufferHintDataMask = 0xff,
    GFXBufferHintData = 0,
    GFXBufferHintDataVertex,
    GFXBufferHintDataIndex,
    GFXBufferHintDataUniform
} GFXBufferHint;

/*!
 * @brief The graphics buffer.
 * @description Allows @b CCRetain.
 */
typedef struct GFXBuffer *GFXBuffer;


/*!
 * @brief Create a buffer of memory available to the GPU.
 * @param Allocator The allocator to be used for the allocations.
 * @param Hint The required usage for the buffer.
 * @param Size The size of the buffer.
 * @param Data The data to be copied into the buffer.
 * @return The created buffer.
 */
CC_NEW GFXBuffer GFXBufferCreate(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data);

/*!
 * @brief Destroy the buffer.
 * @param Buffer The buffer to be destroyed.
 */
void GFXBufferDestroy(GFXBuffer CC_DESTROY(Buffer));

/*!
 * @brief Get the usage hints for the buffer.
 * @param Buffer The buffer to get the hints for.
 * @return The hints.
 */
GFXBufferHint GFXBufferGetHints(GFXBuffer Buffer);

/*!
 * @brief Get the size of buffer.
 * @param Buffer The buffer to get the size of.
 * @return The size.
 */
size_t GFXBufferGetSize(GFXBuffer Buffer);

/*!
 * @brief Set the size of buffer.
 * @description Contents of the buffer are lost after resizing.
 * @param Buffer The buffer to set the size of.
 * @param Size The new size of the data.
 * @return Whether the resize was successful or not.
 */
_Bool GFXBufferSetSize(GFXBuffer Buffer, size_t Size);

/*!
 * @brief Invalidate the buffer, replacing it with a new buffer.
 * @param Buffer The buffer to be invalidated.
 */
void GFXBufferInvalidate(GFXBuffer Buffer);

/*!
 * @brief Read the contents of the buffer.
 * @param Buffer The buffer to be read.
 * @param Offset The offset to begin reading from.
 * @param Size The amount to read.
 * @param Data The buffer the contents should be written to.
 * @return The amount successfully read.
 */
size_t GFXBufferReadBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data);

/*!
 * @brief Write over the contents of the buffer.
 * @param Buffer The buffer to be written to.
 * @param Offset The offset to begin writing from.
 * @param Size The amount to write.
 * @param Data The buffer the contents should be read from.
 * @return The amount successfully written.
 */
size_t GFXBufferWriteBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data);

/*!
 * @brief Copy the contents of one buffer to another buffer.
 * @param SrcBuffer The buffer to copy from.
 * @param SrcOffset The offset to begin copying from.
 * @param Size The amount to copy.
 * @param DstBuffer The buffer to copy to.
 * @param DstOffset The offset to begin copying to.
 * @return The amount successfully copied.
 */
size_t GFXBufferCopyBuffer(GFXBuffer SrcBuffer, ptrdiff_t SrcOffset, size_t Size, GFXBuffer DstBuffer, ptrdiff_t DstOffset);

/*!
 * @brief Fill the contents of the buffer.
 * @param Buffer The buffer to be filled.
 * @param Offset The offset to begin filling from.
 * @param Size The amount to fill.
 * @param Fill The value to be used to fill.
 * @return The amount successfully filled.
 */
size_t GFXBufferFillBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, uint8_t Fill);

#endif
