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

#include "ECSMonitor.h"

uint8_t ECSMonitorRemovedDiff;

void ECSMonitorDestroy(ECSMonitor *Monitor)
{
    const ECSMonitorDiffDestructor DiffDestructor = Monitor->interface->optional.diffDestructor;
    const ECSMonitorContextDestructor ContextDestructor = Monitor->interface->optional.contextDestructor;
    const ECSMonitorSharedContextDestructor SharedContextDestructor = Monitor->interface->optional.sharedContextDestructor;
    
    if (Monitor->id & ECSComponentStorageModifierDuplicate)
    {
        if (DiffDestructor)
        {
            ECSMonitorDuplicateContext *DuplicateContext = Monitor->context;
            
            const size_t DiffCount = Monitor->page.size * Monitor->page.count;
            const size_t CurrentIndex = (Monitor->page.index - 1) + DiffCount;
            
            for (size_t Loop = 0, Count = Monitor->page.size * Monitor->page.count, DuplicateCount = DuplicateContext->prevCount; Loop < Count; Loop++)
            {
                const size_t Index = (CurrentIndex - Loop) % DiffCount;
                
                const ECSMonitorDuplicateDiff *DuplicateDiff = Monitor->page.diffs[Index];
                
                if (!DuplicateDiff) break;
                
                if (DuplicateCount)
                {
                    for (size_t Loop2 = 0; Loop2 < DuplicateCount; Loop2++)
                    {
                        void *Diff = DuplicateDiff->diffs[Loop2];
                        
                        if (Diff) DiffDestructor(Diff);
                    }
                }
                
                DuplicateCount = DuplicateDiff->count;
            }
        }
        
        ECSMonitorDuplicateContext *DuplicateContext = Monitor->context;
        
        if (ContextDestructor)
        {
            const size_t ContextSize = Monitor->interface->contextSize;
            CCMemoryZoneBlock *Block = CCMemoryZoneGetBlock(DuplicateContext->zone);
            
            ptrdiff_t Offset = 0;
            for (size_t Loop = 0, Count = DuplicateContext->prevCount; Loop < Count; Loop++)
            {
                ContextDestructor(CCMemoryZoneBlockGetPointer(&Block, &Offset, NULL));
                Offset += ContextSize;
            }
        }
        
        CCMemoryZoneDestroy(DuplicateContext->zone);
    }
    
    else
    {
        if (DiffDestructor)
        {
            for (size_t Loop = 0, Count = Monitor->page.size * Monitor->page.count; Loop < Count; Loop++)
            {
                if (Monitor->page.diffs[Loop]) DiffDestructor(Monitor->page.diffs[Loop]);
            }
        }
        
        if (ContextDestructor) ContextDestructor(Monitor->context);
    }
    
    CCFree(Monitor->page.diffs);
    CCFree(Monitor->context);
    
    if (SharedContextDestructor) SharedContextDestructor(Monitor->sharedContext);
    
    for (size_t Loop = 0, Count = Monitor->page.count; Loop < Count; Loop++)
    {
        CCMemoryZoneDestroy(Monitor->page.zones[Loop]);
    }
}

void ECSMonitorRecord(ECSMonitor *Monitor, const void *Data)
{
    if (Monitor->id & ECSComponentStorageModifierDuplicate)
    {
        ECSMonitorDuplicateContext *DuplicateContext = Monitor->context;
        const size_t Count = Data ? CCArrayGetCount(*(CCArray*)Data) : 0;
        
        ECSMonitorDuplicateDiff *HighestDuplicateDiff = Monitor->page.diffs[DuplicateContext->highest % (Monitor->page.size * Monitor->page.count)];
        const size_t HighestCount = HighestDuplicateDiff ? HighestDuplicateDiff->count : 0;
        
        if (Count >= HighestCount)
        {
            const size_t ContextCount = CCMax(HighestCount, DuplicateContext->prevCount);
            
            if (Count > ContextCount)
            {
                const size_t NewContextCount = Count - ContextCount;
                const size_t ContextSize = Monitor->interface->contextSize;
                const ECSMonitorInitializer Init = Monitor->interface->initialize;
                
                for (size_t Loop = 0; Loop < NewContextCount; Loop++)
                {
                    Init(Monitor->sharedContext, CCMemoryZoneAllocate(DuplicateContext->zone, ContextSize), Monitor->id);
                }
            }
            
            DuplicateContext->highest = Monitor->page.index;
        }
        
        const size_t Index = Monitor->page.index;
        CCMemoryZone Zone = Monitor->page.zones[(Index / Monitor->page.size) % Monitor->page.count];
        
        const size_t ElementCount = CCMax(Count, DuplicateContext->prevCount);
        const size_t DuplicateDiffSize = sizeof(ECSMonitorDuplicateDiff) + (sizeof(void*) * ElementCount);
        ECSMonitorDuplicateDiff *DuplicateDiff = CCMemoryZoneAllocate(Zone, DuplicateDiffSize);
        
        DuplicateDiff->count = DuplicateContext->prevCount;
        
        CCMemoryZoneBlock *Block = CCMemoryZoneGetBlock(DuplicateContext->zone);
        ptrdiff_t Offset = 0;
        const size_t ContextSize = Monitor->interface->contextSize;
        _Bool Changed = Count != DuplicateContext->prevCount;
        
        for (size_t Loop = 0; Loop < ElementCount; Loop++)
        {
            void *Diff = Monitor->interface->diff(Monitor->sharedContext, CCMemoryZoneBlockGetPointer(&Block, &Offset, NULL), Zone, Monitor->id, Loop < Count ? CCArrayGetElementAtIndex(*(CCArray*)Data, Loop) : NULL);
            
            DuplicateDiff->diffs[Loop] = Diff;
            
            Changed |= (_Bool)Diff;
            
            Offset += ContextSize;
        }
        
        if (Changed)
        {
            Monitor->page.diffs[Index] = DuplicateDiff;
            
            const size_t NextIndex = ++Monitor->page.index;
            
            if (NextIndex % (Monitor->page.size * Monitor->page.count) == 0) Monitor->page.index = 0;
            
            if (NextIndex % Monitor->page.size == 0)
            {
                const size_t PurgedPageIndex = (NextIndex / Monitor->page.size) % Monitor->page.count;
                
                if (PurgedPageIndex == ((DuplicateContext->highest / Monitor->page.size) + 1) % Monitor->page.count)
                {
                    size_t NewHighestCount = 0, NewHighestCountIndex = 0;
                    for (size_t Loop = 0, PageCount = Monitor->page.count, ActivePageCount = PageCount - 1; Loop < ActivePageCount; Loop++)
                    {
                        const size_t CurrentPageIndex = ((PurgedPageIndex - Loop) + ActivePageCount) % PageCount;
                        
                        for (size_t PageSize = Monitor->page.size, Loop2 = PageSize; Loop2--; )
                        {
                            const size_t DiffIndex = (CurrentPageIndex * PageSize) + Loop2;
                            
                            ECSMonitorDuplicateDiff *Diff = Monitor->page.diffs[DiffIndex];
                            
                            if (!Diff) break;
                            
                            const size_t PrevCount = Diff->count;
                            
                            if (PrevCount > NewHighestCount)
                            {
                                NewHighestCount = PrevCount;
                                NewHighestCountIndex = DiffIndex;
                            }
                        }
                    }
                    
                    if (Count > NewHighestCount)
                    {
                        NewHighestCount = Count;
                        NewHighestCountIndex = Index;
                    }
                    
                    if (NewHighestCount < HighestCount)
                    {
                        const size_t ExpiredContextCount = HighestCount - NewHighestCount;
                        const size_t ContextSize = Monitor->interface->contextSize;
                        const ECSMonitorContextDestructor Destructor = Monitor->interface->optional.contextDestructor;
                        
                        if (Destructor)
                        {
                            CCMemoryZoneBlock *Block = CCMemoryZoneGetBlock(DuplicateContext->zone);
                            
                            ptrdiff_t ContextOffset = (NewHighestCount + 1) * ContextSize;
                            for (size_t Loop = 0; Loop < ExpiredContextCount; Loop++)
                            {
                                Destructor(CCMemoryZoneBlockGetPointer(&Block, &ContextOffset, NULL));
                                ContextOffset += ContextSize;
                            }
                        }
                        
                        CCMemoryZoneDeallocate(DuplicateContext->zone, ContextSize * ExpiredContextCount);
                    }
                }
                
                const ECSMonitorDiffDestructor DiffDestructor = Monitor->interface->optional.diffDestructor;
                const size_t PurgedDiffIndex = PurgedPageIndex * Monitor->page.size;
                
                if (DiffDestructor)
                {
                    const size_t DiffCount = Monitor->page.size * Monitor->page.count;
                    const size_t CurrentIndex = ((PurgedDiffIndex + Monitor->page.size) - 1) + DiffCount;
                    
                    const ECSMonitorDuplicateDiff *NextDiff = Monitor->page.diffs[(CurrentIndex + 1) % DiffCount];
                    
                    for (size_t Loop = 0, Count = Monitor->page.size; Loop < Count; Loop++)
                    {
                        const size_t Index = (CurrentIndex - Loop) % DiffCount;
                        
                        const ECSMonitorDuplicateDiff *DuplicateDiff = Monitor->page.diffs[Index];
                        
                        if (!DuplicateDiff) break;
                        
                        const size_t DuplicateCount = NextDiff->count;
                        if (DuplicateCount)
                        {
                            for (size_t Loop2 = 0; Loop2 < DuplicateCount; Loop2++)
                            {
                                void *Diff = DuplicateDiff->diffs[Loop2];
                                
                                if (Diff) DiffDestructor(Diff);
                            }
                        }
                        
                        Monitor->page.diffs[Index] = NULL;
                        
                        NextDiff = DuplicateDiff;
                    }
                }
                
                else memset(Monitor->page.diffs + PurgedDiffIndex, 0, sizeof(void*) * Monitor->page.size);
                
                CCMemoryZoneDeallocate(Monitor->page.zones[PurgedPageIndex], SIZE_MAX);
            }
            
            DuplicateContext->prevCount = Count;
        }
        
        else CCMemoryZoneDeallocate(Zone, DuplicateDiffSize);
    }
    
    else
    {
        const size_t Index = Monitor->page.index;
        CCMemoryZone Zone = Monitor->page.zones[(Index / Monitor->page.size) % Monitor->page.count];
        
        void *Diff = Monitor->interface->diff(Monitor->sharedContext, Monitor->context, Zone, Monitor->id, Data);
        if (Diff)
        {
            Monitor->page.diffs[Index] = Diff;
            
            const size_t NextIndex = ++Monitor->page.index;
            
            if (NextIndex % (Monitor->page.size * Monitor->page.count) == 0) Monitor->page.index = 0;
            
            if (NextIndex % Monitor->page.size == 0)
            {
                const ECSMonitorDiffDestructor DiffDestructor = Monitor->interface->optional.diffDestructor;
                
                const size_t PurgedPageIndex = (NextIndex / Monitor->page.size) % Monitor->page.count;
                const size_t PurgedDiffIndex = PurgedPageIndex * Monitor->page.size;
                
                if (DiffDestructor)
                {
                    for (size_t Loop = 0, Count = Monitor->page.size; Loop < Count; Loop++)
                    {
                        if (Monitor->page.diffs[Loop + PurgedDiffIndex]) DiffDestructor(Monitor->page.diffs[Loop + PurgedDiffIndex]);
                        
                        Monitor->page.diffs[Loop + PurgedDiffIndex] = NULL;
                    }
                }
                
                else memset(Monitor->page.diffs + PurgedDiffIndex, 0, sizeof(void*) * Monitor->page.size);
                
                CCMemoryZoneDeallocate(Monitor->page.zones[PurgedPageIndex], SIZE_MAX);
            }
        }
    }
}

_Bool ECSMonitorTransform(ECSMonitor *Monitor, void *Data, size_t Revisions)
{
    if (Monitor->id & ECSComponentStorageModifierDuplicate)
    {
        const ECSMonitorTransformer Transform = Monitor->interface->transform;
        
        const size_t DiffCount = Monitor->page.size * Monitor->page.count;
        const size_t CurrentIndex = (Monitor->page.index - 1) + DiffCount;
        
        Revisions = CCMin(Revisions, DiffCount);
        
        CCArray Duplicates = *(CCArray*)Data;
        if (!Duplicates) Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSComponentSize(Monitor->id), 16);
        
        ECSMonitorDuplicateContext *DuplicateContext = Monitor->context;
        
        for (size_t Loop = 0; Loop < Revisions; Loop++)
        {
            const size_t Index = (CurrentIndex - Loop) % DiffCount;
            
            CCMemoryZone Zone = Monitor->page.zones[(Index / Monitor->page.size) % Monitor->page.count];
            
            const ECSMonitorDuplicateDiff *DuplicateDiff = Monitor->page.diffs[Index];
            
            if (!DuplicateDiff) break;
            
            const size_t DuplicateCount = DuplicateDiff->count;
            
            if (DuplicateCount)
            {
                const size_t CurrentCount = CCArrayGetCount(Duplicates);
                
                if (DuplicateCount < CurrentCount)
                {
                    CCArrayRemoveElementsAtIndex(Duplicates, DuplicateCount, CurrentCount - DuplicateCount);
                }
                
                else if (DuplicateCount > CurrentCount)
                {
                    CCArrayAppendElements(Duplicates, NULL, DuplicateCount - CurrentCount);
                }
                
                CCMemoryZoneBlock *Block = CCMemoryZoneGetBlock(DuplicateContext->zone);
                ptrdiff_t Offset = 0;
                const size_t ContextSize = Monitor->interface->contextSize;
                
                for (size_t Loop2 = 0; Loop2 < DuplicateCount; Loop2++)
                {
                    void *Diff = DuplicateDiff->diffs[Loop2];
                    
                    if (Diff)
                    {
                        void *Element = CCArrayGetElementAtIndex(Duplicates, Loop2);
                        void *TransformedData = Transform(Monitor->sharedContext, CCMemoryZoneBlockGetPointer(&Block, &Offset, NULL), Zone, DuplicateDiff->diffs[Loop2], Monitor->id, Element);
                        
                        if (TransformedData != Element) CCArrayReplaceElementAtIndex(Duplicates, Loop2, TransformedData);
                    }
                    
                    Offset += ContextSize;
                }
                
                *(CCArray*)Data = Duplicates;
            }
            
            else
            {
                CCArrayRemoveAllElements(Duplicates);
                *(CCArray*)Data = NULL;
            }
        }
        
        if (!*(CCArray*)Data)
        {
            CCArrayDestroy(Duplicates);
            
            return FALSE;
        }
        
        return TRUE;
    }
    
    else
    {
        const ECSMonitorTransformer Transform = Monitor->interface->transform;
        
        void *TransformedData = Data;
        
        const size_t DiffCount = Monitor->page.size * Monitor->page.count;
        const size_t CurrentIndex = (Monitor->page.index - 1) + DiffCount;
        
        Revisions = CCMin(Revisions, DiffCount);
        
        for (size_t Loop = 0; Loop < Revisions; Loop++)
        {
            const size_t Index = (CurrentIndex - Loop) % DiffCount;
            
            CCMemoryZone Zone = Monitor->page.zones[(Index / Monitor->page.size) % Monitor->page.count];
            
            const void *Diff = Monitor->page.diffs[Index];
            
            if (!Diff) break;
            
            if (!TransformedData) TransformedData = Data;
            
            TransformedData = Transform(Monitor->sharedContext, Monitor->context, Zone, Diff, Monitor->id, TransformedData);
        }
        
        if ((TransformedData) && (Data != TransformedData)) memcpy(Data, TransformedData, ECSComponentSize(Monitor->id));
        
        return TransformedData;
    }
}
