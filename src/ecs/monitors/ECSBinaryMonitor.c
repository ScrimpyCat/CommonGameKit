/*
 *  Copyright (c) 2023, Stefan Johnson
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

#include "ECSBinaryMonitor.h"

void ECSBinaryMonitorContextDestructor(ECSBinaryMonitorContext *Context);
void ECSBinaryMonitorInitialize(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, ECSComponentID ID);
void *ECSBinaryMonitorDiff(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, CCMemoryZone Zone, ECSComponentID ID, const void *Data);
void *ECSBinaryMonitorTransform(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, CCMemoryZone Zone, const void *Diff, ECSComponentID ID, void *Data);

const ECSMonitorInterface ECSBinaryMonitorInterface = {
    .contextSize = sizeof(ECSBinaryMonitorContext),
    .initialize = (ECSMonitorInitializer)ECSBinaryMonitorInitialize,
    .diff = (ECSMonitorDiffer)ECSBinaryMonitorDiff,
    .transform = (ECSMonitorTransformer)ECSBinaryMonitorTransform,
    .optional = {
        .diffDestructor = NULL,
        .contextDestructor = (ECSMonitorContextDestructor)ECSBinaryMonitorContextDestructor,
        .sharedContextDestructor = NULL
    }
};

const ECSMonitorInterface * const ECSBinaryMonitor = &ECSBinaryMonitorInterface;

void ECSBinaryMonitorContextDestructor(ECSBinaryMonitorContext *Context)
{
    CCFree(Context->copy.data);
}

void ECSBinaryMonitorInitialize(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, ECSComponentID ID)
{
    Context->copy.data = CCMalloc(SharedContext->allocator, SharedContext->copy.size, NULL, CC_DEFAULT_ERROR_CALLBACK);
    Context->copy.created = FALSE;
}

static inline void ECSBinaryMonitorCopyValue(void *Ptr, size_t Offset, size_t Size)
{
#if CC_HARDWARE_ENDIAN_LITTLE
    memcpy(Ptr, &Offset, Size);
#elif CC_HARDWARE_ENDIAN_LITTLE
    memcpy(Ptr, (uint8_t&)&Offset + (sizeof(size_t) - Size), Size);
#else
#error Unknown endianness
#endif
}

static inline size_t ECSBinaryMonitorReadValue(const void *Ptr, size_t Size)
{
    size_t Offset = 0;
    
#if CC_HARDWARE_ENDIAN_LITTLE
    memcpy(&Offset, Ptr, Size);
#elif CC_HARDWARE_ENDIAN_LITTLE
    memcpy((uint8_t&)&Offset + (sizeof(size_t) - Size), Ptr, Size);
#else
#error Unknown endianness
#endif
    
    return Offset;
}

void *ECSBinaryMonitorDiff(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, CCMemoryZone Zone, ECSComponentID ID, const void *Data)
{
    if ((Context->copy.created) && (Data))
    {
        const size_t DataSize = SharedContext->copy.size;
        const size_t OffsetSize = SharedContext->diff.offset + 1;
        void *DiffCount = CCMemoryZoneAllocate(Zone, OffsetSize);
        
        size_t ChangeCount = 0;
        size_t SeqSize = SharedContext->diff.size;
        size_t DiffSize = OffsetSize + SeqSize;
        size_t Count = DataSize / SeqSize;
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            void *CopyChunk = Context->copy.data + (Loop * SeqSize);
            const void *Chunk = Data + (Loop * SeqSize);
            if (memcmp(CopyChunk, Chunk, SeqSize))
            {
                void *Ptr = CCMemoryZoneAllocate(Zone, DiffSize);
                ECSBinaryMonitorCopyValue(Ptr, Loop, OffsetSize);
                memcpy(Ptr + OffsetSize, CopyChunk, SeqSize);
                
                memcpy(CopyChunk, Chunk, SeqSize);
                
                ChangeCount++;
            }
        }
        
        size_t Remainder = DataSize % SeqSize;
        if (Remainder)
        {
            void *CopyChunk = Context->copy.data + (Count * SeqSize);
            const void *Chunk = Data + (Count * SeqSize);
            if (memcmp(CopyChunk, Chunk, Remainder))
            {
                void *Ptr = CCMemoryZoneAllocate(Zone, DiffSize);
                ECSBinaryMonitorCopyValue(Ptr, Count, OffsetSize);
                memcpy(Ptr + OffsetSize, CopyChunk, Remainder);
                
                memcpy(CopyChunk, Chunk, Remainder);
                
                ChangeCount++;
            }
        }
        
        if (ChangeCount)
        {
            ECSBinaryMonitorCopyValue(DiffCount, ChangeCount - 1, OffsetSize);
            
            return DiffCount;
        }
        
        else CCMemoryZoneDeallocate(Zone, OffsetSize);
    }
    
    else if ((Context->copy.created) && (!Data)) //removed
    {
        Context->copy.created = FALSE;
        
        const size_t DataSize = SharedContext->copy.size;
        
        const size_t OffsetSize = SharedContext->diff.offset + 1;
        void *DiffCount = CCMemoryZoneAllocate(Zone, OffsetSize);
        
        const size_t SeqSize = SharedContext->diff.size;
        const size_t DiffSize = OffsetSize + SeqSize;
        size_t Count = DataSize / SeqSize;
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            void *Ptr = CCMemoryZoneAllocate(Zone, DiffSize);
            ECSBinaryMonitorCopyValue(Ptr, Loop, OffsetSize);
            memcpy(Ptr + OffsetSize, Context->copy.data + (Loop * SeqSize), SeqSize);
        }
        
        const size_t Remainder = DataSize % SeqSize;
        if (Remainder)
        {
            void *Ptr = CCMemoryZoneAllocate(Zone, DiffSize);
            ECSBinaryMonitorCopyValue(Ptr, Count, OffsetSize);
            memcpy(Ptr + OffsetSize, Context->copy.data + (Count * SeqSize), Remainder);
            
            Count++;
        }
        
        ECSBinaryMonitorCopyValue(DiffCount, Count - 1, OffsetSize);
        
        return DiffCount;
    }
    
    else if ((!Context->copy.created) && (Data)) //created
    {
        const size_t DataSize = SharedContext->copy.size;
        
        Context->copy.created = TRUE;
        memcpy(Context->copy.data, Data, DataSize);
        
        return &ECSMonitorRemovedDiff;
    }
    
    return NULL;
}

void *ECSBinaryMonitorTransform(ECSBinaryMonitorSharedContext *SharedContext, ECSBinaryMonitorContext *Context, CCMemoryZone Zone, const void *Diff, ECSComponentID ID, void *Data)
{
    if (Diff == &ECSMonitorRemovedDiff) return NULL;
    
    const size_t DataSize = SharedContext->copy.size;
    const size_t OffsetSize = SharedContext->diff.offset + 1;
    const size_t DiffCount = ECSBinaryMonitorReadValue(Diff, OffsetSize);
    
    size_t SeqSize = SharedContext->diff.size;
    size_t DiffSize = OffsetSize + SeqSize;
    
    ptrdiff_t RelativeOffset;
    CCMemoryZoneBlock *Block = CCMemoryZoneGetBlockForPointer(Zone, Diff, &RelativeOffset);
    
    RelativeOffset += OffsetSize;
    
    for (size_t Loop = 0; Loop < DiffCount; Loop++)
    {
        Diff = CCMemoryZoneBlockGetPointer(&Block, &RelativeOffset, NULL);
        
        const size_t Offset = ECSBinaryMonitorReadValue(Diff, OffsetSize);
        
        memcpy(Data + (Offset * SeqSize), Diff + OffsetSize, SeqSize);
        
        RelativeOffset += DiffSize;
    }
    
    Diff = CCMemoryZoneBlockGetPointer(&Block, &RelativeOffset, NULL);
    
    const size_t Offset = ECSBinaryMonitorReadValue(Diff, OffsetSize);
    
    size_t Remainder = DataSize % SeqSize;
    
    if (!Remainder) Remainder = SeqSize;
    
    memcpy(Data + (Offset * SeqSize), Diff + OffsetSize, SeqSize);
    
    return Data;
}
