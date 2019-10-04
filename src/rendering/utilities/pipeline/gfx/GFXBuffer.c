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
#include "GFXBuffer.h"
#include "GFXMain.h"

GFXBuffer GFXBufferCreate(CCAllocatorType Allocator, GFXBufferHint Hint, size_t Size, const void *Data)
{
    return GFXMain->buffer->create(Allocator, Hint, Size, Data);
}

void GFXBufferDestroy(GFXBuffer Buffer)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    GFXMain->buffer->destroy(Buffer);
}

GFXBufferHint GFXBufferGetHints(GFXBuffer Buffer)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    return GFXMain->buffer->hints(Buffer);
}

size_t GFXBufferGetSize(GFXBuffer Buffer)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    return GFXMain->buffer->size(Buffer);
}

_Bool GFXBufferSetSize(GFXBuffer Buffer, size_t Size)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    return GFXMain->buffer->resize(Buffer, Size);
}

void GFXBufferInvalidate(GFXBuffer Buffer)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    if (GFXMain->buffer->optional.invalidate) GFXMain->buffer->optional.invalidate(Buffer);
}

size_t GFXBufferReadBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, void *Data)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    return GFXMain->buffer->read(Buffer, Offset, Size, Data);
}

size_t GFXBufferWriteBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, const void *Data)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    return GFXMain->buffer->write(Buffer, Offset, Size, Data);
}

size_t GFXBufferCopyBuffer(GFXBuffer SrcBuffer, ptrdiff_t SrcOffset, size_t Size, GFXBuffer DstBuffer, ptrdiff_t DstOffset)
{
    CCAssertLog(SrcBuffer && DstBuffer, "Source and destination buffers must not be null");
    
    if (GFXMain->buffer->optional.copy) return GFXMain->buffer->optional.copy(SrcBuffer, SrcOffset, Size, DstBuffer, DstOffset);
    
    uint8_t *CopiedData = NULL;
    CC_TEMP_Malloc(CopiedData, Size,
                   return 0;
                   );
    
    size_t CopiedSize = GFXBufferReadBuffer(SrcBuffer, SrcOffset, Size, CopiedData);
    CopiedSize = GFXBufferWriteBuffer(DstBuffer, DstOffset, CopiedSize, CopiedData);
    
    CC_TEMP_Free(CopiedData);
    
    return CopiedSize;
}

size_t GFXBufferFillBuffer(GFXBuffer Buffer, ptrdiff_t Offset, size_t Size, uint8_t Fill)
{
    CCAssertLog(Buffer, "Buffer must not be null");
    
    if (GFXMain->buffer->optional.fill) return GFXMain->buffer->optional.fill(Buffer, Offset, Size, Fill);
    
    uint8_t *FillData = NULL;
    CC_TEMP_Malloc(FillData, Size,
                   return 0;
                   );
    
    memset(FillData, Fill, Size);
    const size_t Written = GFXBufferWriteBuffer(Buffer, Offset, Size, FillData);
    
    CC_TEMP_Free(FillData);
    
    return Written;
}
