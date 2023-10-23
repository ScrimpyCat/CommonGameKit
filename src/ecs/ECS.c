/*
 *  Copyright (c) 2022, Stefan Johnson
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

#include "ECS.h"
#include <threads.h>

#ifndef ECS_SYSTEM_EXECUTOR_COMPONENT_MAX
#define ECS_SYSTEM_EXECUTOR_COMPONENT_MAX 16
#endif

#ifndef ECS_SYSTEM_EXECUTION_POOL_MAX
#define ECS_SYSTEM_EXECUTION_POOL_MAX 1024
#endif

CC_ARRAY_DECLARE(ECSEntity);

typedef struct {
    const ECSSystemAccess *access;
    ECSTime time;
    ECSSystemUpdateCallback update;
    struct {
        size_t offset;
        size_t count;
    } archetype;
    ECSRange range;
    ECSExecutionGroup *executionGroup;
    ECSContext *context;
} ECSSystemExecutor;

#define CC_TYPE_ECSSystemExecutor(...) ECSSystemExecutor
#define CC_TYPE_0_ECSSystemExecutor CC_TYPE_ECSSystemExecutor,

#define CC_PRESERVE_CC_TYPE_ECSSystemExecutor CC_TYPE_ECSSystemExecutor

#define CC_TYPE_DECL_ECSSystemExecutor(...) ECSSystemExecutor, __VA_ARGS__
#define CC_TYPE_DECL_0_ECSSystemExecutor CC_TYPE_DECL_ECSSystemExecutor,

#define CC_MANGLE_TYPE_0_ECSSystemExecutor ECSSystemExecutor

#define T ECSSystemExecutor
#include <CommonC/ConcurrentPool.h>

static CCConcurrentPool(ECSSystemExecutor, ECS_SYSTEM_EXECUTION_POOL_MAX) ExecutorPool = CC_CONCURRENT_POOL_INIT;

#define SystemExecutorPool ((CCConcurrentPool*)&ExecutorPool)

#ifndef ECS_COMPONENT_ACCESS_RELEASE_MAX
#define ECS_COMPONENT_ACCESS_RELEASE_MAX 16
#endif

typedef struct {
    size_t count;
    struct {
        const ECSSystemAccess *access;
        ECSExecutionGroup *executionGroup;
    } release[ECS_COMPONENT_ACCESS_RELEASE_MAX];
} ECSComponentAccessRelease;

#ifndef ECS_WORKER_THREAD_MAX
#define ECS_WORKER_THREAD_MAX 128
#endif

int ECSWorker(ECSWorkerID WorkerID);

static ECSWorkerID WorkerThreadCount = 0;

_Bool ECSWorkerCreate(void)
{
    CCAssertLog(WorkerThreadCount < ECS_WORKER_THREAD_MAX, "Increase size of ECS_WORKER_THREAD_MAX");
    
    thrd_t Thread;
    if (thrd_create(&Thread, (thrd_start_t)ECSWorker, (void*)WorkerThreadCount) != thrd_success)
    {
        CC_LOG_DEBUG("Failed to create ECS worker (%" PRIuPTR ") thread", WorkerThreadCount);
        
        return FALSE;
    }
    
    WorkerThreadCount++;
    
    return TRUE;
}

static void ECSSpinWait(ECSWorkerID ID)
{
    CC_SPIN_WAIT();
}

ECSWaitingCallback ECSWaiting = ECSSpinWait;

const size_t *ECSArchetypeComponentIndexes;

static ECSComponentAccessRelease AccessReleases[ECS_WORKER_THREAD_MAX][3];
static _Alignas(CC_HARDWARE_CACHE_LINE) struct {
    _Atomic(uint8_t) index;
#if ECS_ACCESS_RELEASE_INDEX_PAD_TO_CACHE_LINE
    uint8_t pad[CC_ALIGN(sizeof(_Atomic(uint8_t)), CC_HARDWARE_CACHE_LINE) - sizeof(_Atomic(uint8_t))];
#endif
} AccessReleaseIndex[ECS_WORKER_THREAD_MAX] = {ATOMIC_VAR_INIT(0)};

int ECSWorker(ECSWorkerID WorkerID)
{
    const size_t * const ArchetypeComponentIndexes = ECSArchetypeComponentIndexes;
    size_t LocalAccessReleaseIndex = 1;
    
    for (ECSSystemExecutor *Executor; ; )
    {
        if (CCConcurrentPoolPop_weak(SystemExecutorPool, &Executor, ECS_SYSTEM_EXECUTION_POOL_MAX))
        {
            const ECSSystemAccess *Access = Executor->access;
            
            for (size_t Count = Access->read.count + Access->write.count; Count > (ECS_COMPONENT_ACCESS_RELEASE_MAX - AccessReleases[WorkerID][LocalAccessReleaseIndex].count); )
            {
                ECSWaiting(WorkerID);
                
                LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID].index, LocalAccessReleaseIndex, memory_order_consume);
            }
            
            const size_t ArchCount = Executor->archetype.count;
            const ECSSystemUpdateCallback Callback = Executor->update;
            ECSContext * const Context = Executor->context;
            const ECSTime Time = Executor->time;
            const size_t * const ComponentOffsets = Access->component.offsets;
            const ECSRange Range = Executor->range;
            
            if (ArchCount)
            {
                for (size_t Loop = 0; Loop < ArchCount; Loop++)
                {
                    const ECSArchetypePointer *ArchPointer = &Access->archetype.pointer[Executor->archetype.offset + Loop];
                    ECSArchetype *Archetype = (void*)Executor->context + ArchPointer->archetype;
                    
                    if ((Archetype->entities) && (CCArrayGetCount(Archetype->entities)))
                    {
                        Callback(Context, Archetype, ArchetypeComponentIndexes + ArchPointer->componentIndexes, ComponentOffsets, Range, Time);
                    }
                }
            }
            
            else
            {
                Callback(Context, NULL, NULL, ComponentOffsets, Range, Time);
            }
            
            const size_t Index = AccessReleases[WorkerID][LocalAccessReleaseIndex].count++;
            AccessReleases[WorkerID][LocalAccessReleaseIndex].release[Index].access = Access;
            AccessReleases[WorkerID][LocalAccessReleaseIndex].release[Index].executionGroup = Executor->executionGroup;
            
            atomic_thread_fence(memory_order_release);
            LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID].index, LocalAccessReleaseIndex, memory_order_consume);
        }
        
        else
        {
            ECSWaiting(WorkerID);
            
            if (AccessReleases[WorkerID][LocalAccessReleaseIndex].count)
            {
                LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID].index, LocalAccessReleaseIndex, memory_order_consume);
            }
        }
    }
    
    return 0;
}

static size_t LocalAccessReleaseIndexes[ECS_WORKER_THREAD_MAX];

size_t ECSSharedMemorySize = 1048576;

CCMemoryZone ECSSharedZone;

void ECSInit(void)
{
    for (size_t Loop = 0; Loop < ECS_WORKER_THREAD_MAX; Loop++) LocalAccessReleaseIndexes[Loop] = 2;
    
    ECSSharedZone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, ECSSharedMemorySize);
}

#if CC_HARDWARE_PTR_64
typedef uint64_t ECSContextAccessFlag;
#elif CC_HARDWARE_PTR_32
typedef uint32_t ECSContextAccessFlag;
#endif

static _Bool ECSSubmitSystem(ECSContext *Context, ECSTime Time, size_t SystemIndex, const ECSSystemRange *Range, const ECSSystemAccess *Access, const ECSSystemUpdate *Update, ECSContextAccessFlag *AccessFlags, uint16_t *Refs, uint8_t *Block, uint8_t BlockBit, ECSExecutionGroup *State, CCConcurrentPoolStage *Stage)
{
    for (size_t Loop = 0, IdCount = Access[SystemIndex].read.count; Loop < IdCount; Loop++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].read.ids[Loop]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        if ((AccessFlags[AccessFlagByteIndex] >> ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8))) & 1) return FALSE;
    }
    
    for (size_t Loop = 0, IdCount = Access[SystemIndex].write.count; Loop < IdCount; Loop++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].write.ids[Loop]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        if ((AccessFlags[AccessFlagByteIndex] >> ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8))) & 3) return FALSE;
    }
    
    *Block |= 1 << BlockBit;
    
    ECSSystemExecutor Executor = {
        .access = &Access[SystemIndex],
        .time = Time,
        .update = ECS_SYSTEM_UPDATE_GET_UPDATE(Update[SystemIndex]),
        .range = { 0, SIZE_MAX },
        .executionGroup = State,
        .context = Context
    };
    
    const _Bool Parallel = ECS_SYSTEM_UPDATE_GET_PARALLEL(Update[SystemIndex]);
    const size_t ArchCount = Access[SystemIndex].archetype.count;
    size_t RefCount = 0;
    
    if (Parallel)
    {
        const size_t ChunkSize = ECS_SYSTEM_UPDATE_GET_PARALLEL_CHUNK_SIZE(Update[SystemIndex]);
        
        if (ECS_SYSTEM_UPDATE_GET_PARALLEL_ARCHETYPE(Update[SystemIndex]))
        {
            Executor.archetype.count = 1;
            
            if (ChunkSize < SIZE_MAX)
            {
                for (size_t Loop = 0; Loop < ArchCount; Loop++)
                {
                    const ECSArchetypePointer *ArchPointer = &Access->archetype.pointer[Loop];
                    ECSArchetype *Archetype = (void*)Context + ArchPointer->archetype;
                    
                    size_t Count;
                    if ((Archetype->entities) && (Count = CCArrayGetCount(Archetype->entities)))
                    {
                        const size_t ChunkCount = ((Count - 1) / ChunkSize) + 1;
                        RefCount += ChunkCount;
                        
                        Executor.archetype.offset = Loop;
                        
                        for (size_t Loop2 = 0; Loop2 < ChunkCount; Loop2++)
                        {
                            const size_t Offset = Loop2 * ChunkSize;
                            Executor.range = (ECSRange){
                                .index = Offset,
                                .count = CCMin(Count - Offset, ChunkSize)
                            };
                            
                            CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
                        }
                        
                        State->running += ChunkCount;
                    }
                }
            }
            
            else
            {
                RefCount = ArchCount;
                
                for (size_t Loop = 0; Loop < ArchCount; Loop++)
                {
                    Executor.archetype.offset = Loop;
                    CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
                }
                
                State->running += ArchCount;
            }
        }
        
        else
        {
            CCArray Array = *(CCArray*)((void*)Context + ECS_SYSTEM_UPDATE_GET_PARALLEL_OFFSET(Update[SystemIndex]));
            size_t Count;
            
            if ((Array) && (Count = CCArrayGetCount(Array)))
            {
                const size_t ChunkCount = ((Count - 1) / ChunkSize) + 1;
                RefCount = ChunkCount;
                
                Executor.archetype.offset = 0;
                Executor.archetype.count = ArchCount;
                
                for (size_t Loop2 = 0; Loop2 < ChunkCount; Loop2++)
                {
                    const size_t Offset = Loop2 * ChunkSize;
                    Executor.range = (ECSRange){
                        .index = Offset,
                        .count = CCMin(Count - Offset, ChunkSize)
                    };
                    
                    CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
                }
                
                State->running += ChunkCount;
            }
        }
        
        if (!RefCount) return FALSE;
    }
    
    else
    {
        RefCount = 1;
        
        Executor.archetype.offset = 0;
        Executor.archetype.count = ArchCount;
        CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
        
        State->running++;
    }
    
    for (size_t Loop = 0, IdCount = Access[SystemIndex].read.count; Loop < IdCount; Loop++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].read.ids[Loop]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        AccessFlags[AccessFlagByteIndex] |= 2 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8));
        Refs[ComponentIndex] += RefCount;
    }
    
    for (size_t Loop = 0, IdCount = Access[SystemIndex].write.count; Loop < IdCount; Loop++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].write.ids[Loop]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        AccessFlags[AccessFlagByteIndex] |= 1 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8));
        Refs[ComponentIndex] += RefCount;
    }
    
    return TRUE;
}

static inline void ECSReleaseAccessRefs(size_t Worker, uint16_t *Refs, ECSContextAccessFlag *AccessFlags)
{
    for (size_t Loop = 0, Count = AccessReleases[Worker][LocalAccessReleaseIndexes[Worker]].count; Loop < Count; Loop++)
    {
        const ECSSystemAccess *Access = AccessReleases[Worker][LocalAccessReleaseIndexes[Worker]].release[Loop].access;
        
        for (size_t Loop2 = 0, IdCount = Access->read.count; Loop2 < IdCount; Loop2++)
        {
            const size_t ComponentIndex = ECSComponentBaseIndex(Access->read.ids[Loop2]);
            const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
            
            if (!--Refs[ComponentIndex]) AccessFlags[AccessFlagByteIndex] &= ~(3 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8)));
        }
        
        for (size_t Loop2 = 0, IdCount = Access->write.count; Loop2 < IdCount; Loop2++)
        {
            const size_t ComponentIndex = ECSComponentBaseIndex(Access->write.ids[Loop2]);
            const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
            
            if (!--Refs[ComponentIndex]) AccessFlags[AccessFlagByteIndex] &= ~(3 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8)));
        }
        
        AccessReleases[Worker][LocalAccessReleaseIndexes[Worker]].release[Loop].executionGroup->running--;
    }
    
    AccessReleases[Worker][LocalAccessReleaseIndexes[Worker]].count = 0;
}

void ECSTick(ECSContext *Context, const ECSGroup *Groups, size_t GroupCount, ECSExecutionGroup *State, ECSTime DeltaTime)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Groups, "Groups must not be null");
    CCAssertLog(State, "State must not be null");
    
    size_t RunGroupIndexes[GroupCount], RunCount = 0;
    ECSTime GroupTimes[GroupCount];
    
    for (size_t Loop = 0; Loop < GroupCount; Loop++)
    {
        State[Loop].running = 0;
        
        State[Loop].time += DeltaTime;
        ECSTime Ticks = State[Loop].time / Groups[Loop].freq;
        State[Loop].time -= Ticks * Groups[Loop].freq;
        
        if (Ticks)
        {
            State[Loop].executing = 0;
            RunGroupIndexes[RunCount++] = Loop;
            
            if (!Groups[Loop].dynamic)
            {
                State[Loop].time += (Ticks - 1) * Groups[Loop].freq;
                Ticks = 1;
            }
            
            GroupTimes[Loop] = Ticks * Groups[Loop].freq;
        }
        
        else State[Loop].executing = SIZE_MAX;
    }
    
    CCConcurrentPoolStage Stage = CCConcurrentPoolStageBegin(SystemExecutorPool);
    
    size_t TargetIndex = 0;
    
    ECSContextAccessFlag AccessFlags[((ECS_COMPONENT_MAX * 2) / (sizeof(ECSContextAccessFlag) * 8)) + 1] = {0};
    
    static uint16_t Refs[ECS_COMPONENT_MAX];
    
    while (RunCount)
    {
        for (size_t Loop = 0; Loop < RunCount; Loop++)
        {
            const size_t Index = RunGroupIndexes[Loop];
            const size_t Priority = State[Index].executing;
            
            if ((Groups[Index].priorities.deps[Priority].group == SIZE_MAX) || (Groups[Index].priorities.deps[Priority].priority < State[Groups[Index].priorities.deps[Priority].group].executing))
            {
                const ECSSystemRange *Range = &Groups[Index].priorities.systems.range[Priority];
                const uint8_t *Graph = &Groups[Index].priorities.systems.graphs[Range->index];
                const ECSSystemAccess *Access = &Groups[Index].priorities.systems.access[Range->index];
                const ECSSystemUpdate *Update = &Groups[Index].priorities.systems.update[Range->index];
                
                const size_t SystemCount = Range->count;
                const size_t BlockCount = (SystemCount + 7) / 8;
                
                _Bool Completed = TRUE;
                for (size_t Loop2 = 0; Loop2 < BlockCount; Loop2++)
                {
                    const uint8_t Block = State[Index].state[Loop2];
                    const size_t BitCount = (Loop2 + 1 != BlockCount ? 8 : (8 - ((BlockCount * 8) - SystemCount)));
                    
                    if (Block != (0xff >> (8 - BitCount)))
                    {
                        Completed = FALSE;
                        
                        for (size_t Loop3 = 0; Loop3 < BitCount; Loop3++)
                        {
                            if (!((Block >> Loop3) & 1))
                            {
                                const size_t SystemIndex = Loop3 + (Loop2 * 8);
                                
                                if (ECSSubmitSystem(Context, GroupTimes[Index], SystemIndex, Range, Access, Update, AccessFlags, Refs, &State[Index].state[Loop2], Loop3, &State[Index], &Stage))
                                {
                                    for (size_t Loop4 = 0; Loop4 < BlockCount; Loop4++)
                                    {
                                        const size_t ColIndex = (SystemIndex * BlockCount) + Loop4;
                                        const uint8_t GraphBlock = Graph[ColIndex];
                                        uint8_t *Block = &State[Index].state[Loop4];
                                        const size_t BitCount = (Loop4 + 1 != BlockCount ? 8 : (8 - ((BlockCount * 8) - SystemCount)));
                                        const uint8_t RunBlock = ~*Block & GraphBlock;
                                        
                                        if (RunBlock)
                                        {
                                            for (size_t Loop5 = 0; Loop5 < BitCount; Loop5++)
                                            {
                                                if ((RunBlock >> Loop5) & 1)
                                                {
                                                    const size_t SystemIndex = Loop5 + (Loop4 * 8);
                                                    
                                                    ECSSubmitSystem(Context, GroupTimes[Index], SystemIndex, Range, Access, Update, AccessFlags, Refs, Block, Loop5, &State[Index], &Stage);
                                                }
                                            }
                                        }
                                    }
                                    
                                    goto FinishedRow;
                                }
                            }
                        }
                    }
                }
                
            FinishedRow:;
                
                if (Completed)
                {
                    if (!State[Index].running)
                    {
                        if (++State[Index].executing >= Groups[Index].priorities.count)
                        {
                            RunGroupIndexes[Loop] = RunGroupIndexes[--RunCount];
                        }
                        
                        memset(State[Index].state, 0, BlockCount);
                        
                        Loop = -1;
                        continue;
                    }
                }
            }
        }
        
        CCConcurrentPoolStageCommit(SystemExecutorPool, &Stage);
        
        atomic_signal_fence(memory_order_release); // MARK: implicitly use CCConcurrentPoolStageCommit's release barrier, if it changes then replace this with atomic_thread_fence(memory_order_release)
        
        if (!RunCount)
        {
        WaitForWorkers:;
            size_t RunningCount = 0;
            for (size_t Loop = 0; Loop < GroupCount; Loop++)
            {
                RunningCount += State[Loop].running;
            }
            
            if (!RunningCount) goto Finished;
        }
        
        while (!AccessReleases[TargetIndex][(LocalAccessReleaseIndexes[TargetIndex] = atomic_exchange_explicit(&AccessReleaseIndex[TargetIndex].index, LocalAccessReleaseIndexes[TargetIndex], memory_order_consume))].count)
        {
            ECSWaiting(-1);
            
            TargetIndex = (TargetIndex + 1) % WorkerThreadCount;
        }
        
        ECSReleaseAccessRefs(TargetIndex, Refs, AccessFlags);
        
        for (size_t Loop = 1; Loop < WorkerThreadCount; Loop++)
        {
            const size_t Index = (TargetIndex + Loop) % WorkerThreadCount;
            LocalAccessReleaseIndexes[Index] = atomic_exchange_explicit(&AccessReleaseIndex[Index].index, LocalAccessReleaseIndexes[Index], memory_order_consume);
            
            ECSReleaseAccessRefs(Index, Refs, AccessFlags);
        }
        
        if (!RunCount) goto WaitForWorkers;
    }
    
Finished:;
}

static inline void ECSEntityInit(ECSContext *Context, size_t BaseIndex, size_t Count, ECSEntity *Entities)
{
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        const size_t Index = BaseIndex + Loop;
        
        Entities[Loop] = Index;
        
        ECSComponentRefs *Ref = CCArrayGetElementAtIndex(Context->manager.map, Index);
        Ref->archetype.ptr = NULL;
        Ref->archetype.index = 0;
        Ref->archetype.component.count = 0;
        
#if ECS_ARCHETYPE_COMPONENT_ID_SIMD_LOOKUP && CC_HARDWARE_VECTOR_SUPPORT_SSSE3
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
        memset(Ref->archetype.component.ids, INT8_MAX, sizeof(Ref->archetype.component.ids));
#else
        memset(Ref->archetype.component.ids, UINT8_MAX, sizeof(Ref->archetype.component.ids));
#endif
#endif
        
        CC_BITS_INIT_CLEAR(Ref->has);
    }
}

void ECSEntityCreate(ECSContext *Context, ECSEntity *Entities, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Entities, "Entities must not be null");
    
    size_t FreeCount = CCArrayGetCount(Context->manager.available);
    
    if (FreeCount < Count)
    {
        const size_t AppendCount = Count - FreeCount;
        const size_t Index = CCArrayAppendElements(Context->manager.map, NULL, AppendCount);
        
        Count = Count - AppendCount;
        
        ECSEntityInit(Context, Index, AppendCount, Entities + Count);
    }
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        const size_t FreeIndex = --FreeCount;
        const size_t *Index = CCArrayGetElementAtIndex(Context->manager.available, FreeIndex);
        CCArrayRemoveElementAtIndex(Context->manager.available, FreeIndex);
        
        ECSEntityInit(Context, *Index, 1, &Entities[Loop]);
    }
}

const ECSComponentID *ECSComponentIDs;

void ECSEntityDestroy(ECSContext *Context, ECSEntity *Entities, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Entities, "Entities must not be null");
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entities[Loop]);
        ECSComponentID IDs[32];
        size_t ComponentCount = 0;
        const size_t BlockSize = CC_BITS_BLOCK_SIZE(Refs->has);
        
        for (size_t Loop2 = 0; Loop2 < ECS_COMPONENT_MAX; Loop2 += BlockSize)
        {
            if (CCBitsAny(Refs->has, Loop2, BlockSize))
            {
                for (size_t Loop3 = 0; Loop3 < BlockSize; Loop3++)
                {
                    const size_t Index = Loop2 + Loop3;
                    
                    if (CCBitsGet(Refs->has, Index))
                    {
                        if (ComponentCount == (sizeof(IDs) / sizeof(*IDs)))
                        {
                            ECSEntityRemoveComponents(Context, Entities[Loop], IDs, ComponentCount);
                            ComponentCount = 0;
                        }
                        
                        IDs[ComponentCount++] = ECSComponentIDs[Index] & ~ECSComponentStorageModifierDuplicate;
                    }
                }
            }
        }
        
        if (ComponentCount) ECSEntityRemoveComponents(Context, Entities[Loop], IDs, ComponentCount);
    }
    
    CCArrayAppendElements(Context->manager.available, Entities, Count);
}

void ECSDuplicateDestructor(void *Data, ECSComponentID ID)
{
    CCAssertLog(Data, "Data must not be null");
    
    const size_t Index = (ID & ~ECSComponentStorageMask);
    ECSComponentDestructor Destructor = NULL;
    
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
            Destructor = ECSDuplicateArchetypeComponentDestructors[Index];
            break;
            
        case ECSComponentStorageTypePacked:
            Destructor = ECSDuplicatePackedComponentDestructors[Index];
            break;
            
        case ECSComponentStorageTypeIndexed:
            Destructor = ECSDuplicateIndexedComponentDestructors[Index];
            break;
            
        case ECSComponentStorageTypeLocal:
            Destructor = ECSDuplicateLocalComponentDestructors[ECSLocalComponentIndex(ID)];
            break;
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
    
    CCArray Duplicates = *(CCArray*)Data;
    
    if (Destructor)
    {
        for (size_t Loop = 0, Count = CCArrayGetCount(Duplicates); Loop < Count; Loop++)
        {
            Destructor(CCArrayGetElementAtIndex(Duplicates, Loop), ID);
        }
    }
    
    CCArrayDestroy(Duplicates);
}

void ECSEntityGetComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t *Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Count, "Count must not be null");
    
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    size_t ComponentCount = 0;
    const size_t BlockSize = CC_BITS_BLOCK_SIZE(Refs->has);
    
    for (size_t Loop2 = 0; Loop2 < ECS_COMPONENT_MAX; Loop2 += BlockSize)
    {
        if (CCBitsAny(Refs->has, Loop2, BlockSize))
        {
            for (size_t Loop3 = 0; Loop3 < BlockSize; Loop3++)
            {
                const size_t Index = Loop2 + Loop3;
                
                if (CCBitsGet(Refs->has, Index))
                {
                    if ((Components) && (ComponentCount < *Count))
                    {
                        const ECSComponentID ID = ECSComponentIDs[Index];
                        
                        Components[ComponentCount] = (ECSTypedComponent){
                            .id = ID,
                            .data = ECSEntityGetComponent(Context, Entity, ID) // TODO: Optimise this by handling archetype separate to the loop (loop only has to start for components after archetype max)
                        };
                    }
                    
                    if (ComponentCount++ >= *Count) return;
                }
            }
        }
    }
    
    *Count = ComponentCount;
}

const size_t *ECSArchetypeComponentSizes;
const size_t *ECSPackedComponentSizes;
const size_t *ECSIndexedComponentSizes;
const size_t *ECSLocalComponentSizes;
const size_t *ECSDuplicateArchetypeComponentSizes;
const size_t *ECSDuplicatePackedComponentSizes;
const size_t *ECSDuplicateIndexedComponentSizes;
const size_t *ECSDuplicateLocalComponentSizes;

const ECSComponentDestructor *ECSArchetypeComponentDestructors;
const ECSComponentDestructor *ECSPackedComponentDestructors;
const ECSComponentDestructor *ECSIndexedComponentDestructors;
const ECSComponentDestructor *ECSLocalComponentDestructors;
const ECSComponentDestructor *ECSDuplicateArchetypeComponentDestructors;
const ECSComponentDestructor *ECSDuplicatePackedComponentDestructors;
const ECSComponentDestructor *ECSDuplicateIndexedComponentDestructors;
const ECSComponentDestructor *ECSDuplicateLocalComponentDestructors;

static size_t SortedAdd(ECSArchetypeComponentID *Elements, size_t Count, ECSArchetypeComponentID Value)
{
    for (size_t Loop = Count; Loop--; )
    {
        if (Elements[Loop] > Value) Elements[Loop + 1] = Elements[Loop];
        else
        {
            Elements[Loop + 1] = Value;
            
            return Loop + 1;
        }
    }
    
    Elements[0] = Value;
    
    return 0;
}

static inline size_t FindIndex(const ECSArchetypeComponentID *Elements, size_t Count, ECSArchetypeComponentID Max, ECSArchetypeComponentID Value)
{
    size_t Offset = 0;
    
    if (Value >= (Max / 2))
    {
        const size_t Reverse = (Max - Value) + 1;
        
        if (Reverse < Count)
        {
            Offset = Count - Reverse;
            Count = Reverse;
        }
    }
    
    else if (Value < Count) Count = Value + 1;
    
    Elements += Offset;
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        if (Elements[Loop] == Value) return Loop + Offset;
        else if (Elements[Loop] > Value) break;
    }
    
    return SIZE_MAX;
}

static size_t SortedSub(ECSArchetypeComponentID *Elements, size_t Count, ECSArchetypeComponentID Value)
{
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
    ECSArchetypeComponentID CurrentID = INT8_MAX;
#else
    ECSArchetypeComponentID CurrentID = UINT8_MAX;
#endif
    
    for (size_t Loop = Count; Loop--; )
    {
        ECSArchetypeComponentID Temp = Elements[Loop];
        Elements[Loop] = CurrentID;
        CurrentID = Temp;
        
        if (CurrentID == Value) return Loop;
    }
    
    Elements[0] = CurrentID;
    
    return 0;
}

static void ArchetypeRemove(ECSContext *Context, ECSArchetype *Arch, size_t Count, size_t Index)
{
    const size_t LastIndex = CCArrayGetCount(Arch->components[0]) - 1;
    if (Index != LastIndex)
    {
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            const void *Data = CCArrayGetElementAtIndex(Arch->components[Loop], LastIndex);
            CCArrayReplaceElementAtIndex(Arch->components[Loop], Index, Data);
            CCArrayRemoveElementAtIndex(Arch->components[Loop], LastIndex);
        }
        
        const ECSEntity *Entity = CCArrayGetElementAtIndex(Arch->entities, LastIndex);
        CCArrayReplaceElementAtIndex(Arch->entities, Index, Entity);
        CCArrayRemoveElementAtIndex(Arch->entities, LastIndex);
        
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, *Entity);
        Refs->archetype.index = Index;
    }
    
    else
    {
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            CCArrayRemoveElementAtIndex(Arch->components[Loop], LastIndex);
        }
        
        CCArrayRemoveElementAtIndex(Arch->entities, LastIndex);
    }
}

static size_t ArchtypeIndex(ECSArchetypeComponentID *set, size_t n)
{
    size_t result = 0;
    size_t factorial = 1;
    
    for (size_t i = 0; i < n; i++)
    {
        factorial *= (i + 1) * 1;

        size_t p = 1;
        for (size_t j = 0; j <= i; j++)
        {
            p *= (set[i] - j);
        }

        result += p / factorial;
    }

    return result;
}

#define ECS_ARCHETYPE_OFFSET(x, index) { offsetof(ECSContext, archetypes##index), sizeof(ECSArchetype(index)) }
#define ECS_ARCHETYPE_INIT_OFFSETS(count) CC_REPEAT(1, count, ECS_ARCHETYPE_OFFSET)

static const struct {
    ptrdiff_t base;
    ptrdiff_t size;
} ArchetypeOffset[ECS_ARCHETYPE_MAX + 1] = {
    { 0, 0 },
    ECS_ARCHETYPE_INIT_OFFSETS(ECS_ARCHETYPE_MAX)
};

void ECSArchetypeAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t CompIndex = ID & ~ECSComponentStorageMask;
        const size_t AddedIndex = SortedAdd(Refs->archetype.component.ids, Refs->archetype.component.count++, CompIndex);
        const size_t Count = Refs->archetype.component.count;
        const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
        ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
        
        if (CC_UNLIKELY(!Archetype->entities))
        {
            const size_t ChunkSize = ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ArchID, Count);
            
            Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);

            for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], ChunkSize);
        }
        
        const size_t Index = CCArrayAppendElement(Archetype->components[AddedIndex], Data);
        CCArrayAppendElement(Archetype->entities, &Entity);
        
        if (Refs->archetype.ptr)
        {
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0; Loop < Count; Loop++)
            {
                if (Loop < AddedIndex)
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop], Copy);
                }
                
                else if (Loop > AddedIndex)
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop - 1], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop], Copy);
                }
            }
            
            ArchetypeRemove(Context, PrevArchetype, Count - 1, Refs->archetype.index);
        }
        
        Refs->archetype.ptr = Archetype;
        Refs->archetype.index = Index;
        
        CCBitsSet(Refs->has, CompIndex);
    }
}

void ECSArchetypeRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t CompIndex = ID & ~ECSComponentStorageMask;
        const size_t RemovedIndex = SortedSub(Refs->archetype.component.ids, Refs->archetype.component.count--, CompIndex);
        
        _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
        CCBitsClear(Refs->has, CompIndex);
        
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor) ECSArchetypeComponentDestructors[CompIndex](CCArrayGetElementAtIndex(Refs->archetype.ptr->components[RemovedIndex], Refs->archetype.index), ID);
#else
        void *CopiedComponent;
        if (ID & ECSComponentStorageModifierDestructor)
        {
            CCMemoryZoneSave(ECSSharedZone);
            
            CopiedComponent = ECSSharedZoneStore(CCArrayGetElementAtIndex(Refs->archetype.ptr->components[RemovedIndex], Refs->archetype.index), ECSArchetypeComponentSizes[CompIndex]);
        }
#endif
        
        const size_t Count = Refs->archetype.component.count;
        if (Count)
        {
            const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
            ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
            
            if (CC_UNLIKELY(!Archetype->entities))
            {
                const size_t ChunkSize = ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ArchID, Count);
                
                Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);
                
                for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], ChunkSize);
            }
            
            const size_t Index = CCArrayAppendElement(Archetype->entities, &Entity);
            
            const size_t PrevCount = Count + 1;
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0; Loop < PrevCount; Loop++)
            {
                if (Loop < RemovedIndex)
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop], Copy);
                }
                
                else if (Loop > RemovedIndex)
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop - 1], Copy);
                }
            }
            
            ArchetypeRemove(Context, PrevArchetype, PrevCount, Refs->archetype.index);
            
            Refs->archetype.ptr = Archetype;
            Refs->archetype.index = Index;
        }
        
        else
        {
            ArchetypeRemove(Context, Refs->archetype.ptr, 1, Refs->archetype.index);
            
            Refs->archetype.ptr = NULL;
            Refs->archetype.index = 0;
        }
        
        
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
            ECSArchetypeComponentDestructors[CompIndex](CopiedComponent, ID);
#pragma clang diagnostic pop
            
            CCMemoryZoneRestore(ECSSharedZone);
        }
#endif
    }
}

void ECSPackedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentStorageMask);
        ECSPackedComponent *Packed = &Context->packed[Index];
        
        CCArray(ECSEntity) Entities = Packed->entities;
        CCArray Components = *Packed->components;
        
        if (CC_UNLIKELY(!Components))
        {
            const size_t ChunkSize = ECS_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(Index);
            
            Packed->entities = (Entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize));
            
            *Packed->components = (Components = CCArrayCreate(CC_STD_ALLOCATOR, ECSPackedComponentSizes[Index], ChunkSize));
        }
        
        CCArrayAppendElement(Components, Data);
        Refs->packed.indexes[Index] = CCArrayAppendElement(Entities, &Entity);
        
        CCBitsSet(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypePacked));
    }
}

void ECSPackedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentStorageMask);
        ECSPackedComponent *Packed = &Context->packed[Index];
        
        CCArray(ECSEntity) Entities = Packed->entities;
        CCArray Components = *Packed->components;
        const size_t EntityIndex = Refs->packed.indexes[Index];
        const size_t LastIndex = CCArrayGetCount(Components) - 1;
        
        CCBitsClear(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypePacked));
        
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor) ECSPackedComponentDestructors[Index](CCArrayGetElementAtIndex(Components, EntityIndex), ID);
#else
        void *CopiedComponent;
        if (ID & ECSComponentStorageModifierDestructor)
        {
            CCMemoryZoneSave(ECSSharedZone);
            
            CopiedComponent = ECSSharedZoneStore(CCArrayGetElementAtIndex(Components, EntityIndex), ECSPackedComponentSizes[Index]);
        }
#endif
        
        if (EntityIndex != LastIndex)
        {
            const void *Data = CCArrayGetElementAtIndex(Components, LastIndex);
            CCArrayReplaceElementAtIndex(Components, EntityIndex, Data);
            CCArrayRemoveElementAtIndex(Components, LastIndex);
            
            const size_t *Entity = CCArrayGetElementAtIndex(Entities, LastIndex);
            CCArrayReplaceElementAtIndex(Entities, EntityIndex, Entity);
            CCArrayRemoveElementAtIndex(Entities, LastIndex);
            
            ECSComponentRefs *ReplacementRef = CCArrayGetElementAtIndex(Context->manager.map, *Entity);
            ReplacementRef->packed.indexes[Index] = EntityIndex;
        }
        
        else
        {
            CCArrayRemoveElementAtIndex(Components, LastIndex);
            CCArrayRemoveElementAtIndex(Entities, LastIndex);
        }
        
        Refs->packed.indexes[Index] = SIZE_MAX;
        
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor)
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
            ECSPackedComponentDestructors[Index](CopiedComponent, ID);
#pragma clang diagnostic pop
            
            CCMemoryZoneRestore(ECSSharedZone);
        }
#endif
    }
}

void ECSIndexedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentStorageMask);
        ECSIndexedComponent *Indexed = &Context->indexed[Index];
        
        CCArray Components = *Indexed;
        
        if (CC_UNLIKELY(!Components))
        {
            *Indexed = (Components = CCArrayCreate(CC_STD_ALLOCATOR, ECSIndexedComponentSizes[Index], ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(Index)));
        }
        
        const size_t Count = CCArrayGetCount(Components);
        if (Entity >= Count)
        {
            CCArrayAppendElements(Components, NULL, CC_ALIGN((Entity - Count) + 1, ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(Index)));
        }
        
        CCArrayReplaceElementAtIndex(Components, Entity, Data);
        
        CCBitsSet(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypeIndexed));
    }
}

void ECSIndexedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentStorageMask);
        CCBitsClear(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypeIndexed));
        
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor) ECSIndexedComponentDestructors[Index](CCArrayGetElementAtIndex(Context->indexed[Index], Entity), ID);
#else
        if (ID & ECSComponentStorageModifierDestructor)
        {
            CCMemoryZoneSave(ECSSharedZone);
            
            ECSIndexedComponentDestructors[Index](ECSSharedZoneStore(CCArrayGetElementAtIndex(Context->indexed[Index], Entity), ECSIndexedComponentSizes[Index]), ID);
            
            CCMemoryZoneRestore(ECSSharedZone);
        }
#endif
    }
}

void ECSLocalAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = ECSLocalComponentIndex(ID);
        
        if (Data)
        {
            const ptrdiff_t Offset = ECSLocalComponentOffset(ID);
            memcpy(&Refs->local[Offset], Data, ECSLocalComponentSizes[Index]);
        }
        
        CCBitsSet(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypeLocal));
    }
}

void ECSLocalRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = ECSLocalComponentIndex(ID);
        CCBitsClear(Refs->has, Index + ECSComponentBaseIndex(ECSComponentStorageTypeLocal));
        
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor) ECSLocalComponentDestructors[Index](&Refs->local[ECSLocalComponentOffset(ID)], ID);
#else
        if (ID & ECSComponentStorageModifierDestructor)
        {
            CCMemoryZoneSave(ECSSharedZone);
            
            ECSLocalComponentDestructors[Index](ECSSharedZoneStore(&Refs->local[ECSLocalComponentOffset(ID)], ECSLocalComponentSizes[Index]), ID);
            
            CCMemoryZoneRestore(ECSSharedZone);
        }
#endif
    }
}

void ECSEntityAddComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Components, "Components must not be null");
    
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    void *RefData[ECS_ARCHETYPE_MAX];
    void *ComponentData[ECS_ARCHETYPE_COMPONENT_MAX];
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) AddedComponent;
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) CachedComponentLookup;
    
    CC_BITS_INIT_CLEAR(AddedComponent);
    CC_BITS_INIT_CLEAR(CachedComponentLookup);
    
    size_t IndexCount = 0, LastIndex = 0, RefDataCount = 0;
    ECSArchetypeComponentID LastID = 0;
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        switch (Components[Loop].id & (ECSComponentStorageTypeMask | ECSComponentStorageModifierDuplicate))
        {
            case ECSComponentStorageTypeArchetype:
            {
                const ECSArchetypeComponentID ID = Components[Loop].id & ~ECSComponentStorageMask;
                
                if (!ECSEntityHasComponent(Context, Entity, ID))
                {
                    const size_t Offset = LastID < ID ? LastIndex : 0;
                    
                    LastIndex = SortedAdd(Refs->archetype.component.ids + Offset, Refs->archetype.component.count++ - Offset, ID) + Offset;
                    ComponentData[ID] = Components[Loop].data;
                    IndexCount++;
                    CCBitsSet(AddedComponent, ID);
                    CCBitsSet(Refs->has, ID);
                    
                    LastID = ID;
                }
                break;
            }
                
            case ECSComponentStorageTypePacked:
                ECSPackedAddComponent(Context, Entity, Components[Loop].data, Components[Loop].id);
                break;
                
            case ECSComponentStorageTypeIndexed:
                ECSIndexedAddComponent(Context, Entity, Components[Loop].data, Components[Loop].id);
                break;
                
            case ECSComponentStorageTypeLocal:
                ECSLocalAddComponent(Context, Entity, Components[Loop].data, Components[Loop].id);
                break;
                
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeArchetype:
            {
                const ECSArchetypeComponentID ID = Components[Loop].id & ~ECSComponentStorageMask;
                
                if (!ECSEntityHasComponent(Context, Entity, ID))
                {
                    const size_t Offset = LastID < ID ? LastIndex : 0;
                    
                    LastIndex = SortedAdd(Refs->archetype.component.ids + Offset, Refs->archetype.component.count++ - Offset, ID) + Offset;
                    RefData[RefDataCount] = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateArchetypeComponentSizes[ID], ECS_DUPLICATE_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ID));
                    ComponentData[ID] = &RefData[RefDataCount++];
                    IndexCount++;
                    CCBitsSet(AddedComponent, ID);
                    CCBitsSet(Refs->has, ID);
                    
                    LastID = ID;
                }
                
                if ((!CCBitsGet(AddedComponent, ID)) && (!CCBitsGet(CachedComponentLookup, ID)))
                {
                    CCBitsSet(CachedComponentLookup, ID);
                    
                    const size_t Offset = ID ? CCBitsCount(AddedComponent, 0, ID - 1) : 0;
                    size_t Index = ECSArchetypeComponentIndex(Refs, ID) - Offset;
                    ComponentData[ID] = CCArrayGetElementAtIndex(Refs->archetype.ptr->components[Index], Refs->archetype.index);
                }
                
                CCArray *Duplicates = ComponentData[ID];
                CCArrayAppendElement(*Duplicates, Components[Loop].data);
                break;
            }
                
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypePacked:
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeIndexed:
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeLocal:
                ECSEntityAddDuplicateComponent(Context, Entity, Components[Loop].data, Components[Loop].id, 1);
                break;
                
            default:
                CCAssertLog(0, "Unsupported component type");
                break;
        }
    }
    
    if (IndexCount)
    {
        Count = Refs->archetype.component.count;
        
        const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
        ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
        
        if (CC_UNLIKELY(!Archetype->entities))
        {
            const size_t ChunkSize = ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ArchID, Count);
            
            Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);

            for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], ChunkSize);
        }
        
        const size_t Index = CCArrayAppendElement(Archetype->entities, &Entity);
        
        if (Refs->archetype.ptr)
        {
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0, ComponentIndex = 0; Loop < Count; Loop++)
            {
                _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
                
                const ECSArchetypeComponentID ID = Refs->archetype.component.ids[Loop];
                
                if (CCBitsGet(AddedComponent, ID))
                {
                    CCArrayAppendElement(Archetype->components[Loop], ComponentData[ID]);
                    ComponentIndex++;
                }
                
                else
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop - ComponentIndex], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop], Copy);
                }
            }
            
            ArchetypeRemove(Context, PrevArchetype, Count - IndexCount, Refs->archetype.index);
        }
        
        else
        {
            for (size_t Loop = 0; Loop < Count; Loop++)
            {
                CCArrayAppendElement(Archetype->components[Loop], ComponentData[Refs->archetype.component.ids[Loop]]);
            }
        }
        
        Refs->archetype.ptr = Archetype;
        Refs->archetype.index = Index;
    }
}

void ECSEntityRemoveComponents(ECSContext *Context, ECSEntity Entity, ECSComponentID *IDs, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(IDs, "IDs must not be null");
    
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    void *ComponentData[ECS_ARCHETYPE_COMPONENT_MAX];
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) RemovedComponent;
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) CachedComponentLookup;
    
    CC_BITS_INIT_CLEAR(RemovedComponent);
    CC_BITS_INIT_CLEAR(CachedComponentLookup);
    
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
    CCMemoryZoneSave(ECSSharedZone);
    
    size_t CopiedComponentCount = 0;
    struct {
        ECSComponentDestructor destructor;
        ECSTypedComponent component;
    } *CopiedComponents = CCMemoryZoneAllocate(ECSSharedZone, sizeof(*CopiedComponents) * Count);
#endif
    
    size_t IndexCount = 0, LastIndex = 0;
    ECSArchetypeComponentID LastID = 0;
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        const ECSComponentID ID = IDs[Loop];
        
        switch (ID & (ECSComponentStorageTypeMask | ECSComponentStorageModifierDuplicate))
        {
            case ECSComponentStorageTypeArchetype:
            {
                if (ECSEntityHasComponent(Context, Entity, ID))
                {
                    _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
                    
                    const ECSArchetypeComponentID CompID = ID & ~ECSComponentStorageMask;
                    
                    const size_t Offset = LastID < CompID ? LastIndex : 0;
                    
                    LastIndex = SortedSub(Refs->archetype.component.ids + Offset, Refs->archetype.component.count-- - Offset, CompID) + Offset;
                    IndexCount++;
                    CCBitsSet(RemovedComponent, CompID);
                    CCBitsClear(Refs->has, CompID);
                    
                    LastID = CompID;
                    
                    if (ID & ECSComponentStorageModifierDestructor)
                    {
                        const size_t CompOffset = CompID ? CCBitsCount(RemovedComponent, 0, CompID - 1) : 0;
                        const size_t RemovedIndex = ECSArchetypeComponentIndex(Refs, CompID) + CompOffset;
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
                        ECSArchetypeComponentDestructors[CompID](CCArrayGetElementAtIndex(Refs->archetype.ptr->components[RemovedIndex], Refs->archetype.index), ID);
#else
                        CopiedComponents[CopiedComponentCount++] = (typeof(*CopiedComponents)){
                            .destructor = ECSArchetypeComponentDestructors[CompID],
                            .component = {
                                .id = ID,
                                .data = ECSSharedZoneStore(CCArrayGetElementAtIndex(Refs->archetype.ptr->components[RemovedIndex], Refs->archetype.index), ECSArchetypeComponentSizes[CompID])
                            }
                        };
#endif
                    }
                }
                break;
            }
                
            case ECSComponentStorageTypePacked:
                ECSPackedRemoveComponent(Context, Entity, ID);
                break;
                
            case ECSComponentStorageTypeIndexed:
                ECSIndexedRemoveComponent(Context, Entity, ID);
                break;
                
            case ECSComponentStorageTypeLocal:
                ECSLocalRemoveComponent(Context, Entity, ID);
                break;
                
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeArchetype:
            {
                if (ECSEntityHasComponent(Context, Entity, ID))
                {
                    const ECSArchetypeComponentID CompID = ID & ~ECSComponentStorageMask;
                    
                    if (!CCBitsGet(CachedComponentLookup, CompID))
                    {
                        CCBitsSet(CachedComponentLookup, CompID);
                        
                        const size_t Offset = CompID ? CCBitsCount(RemovedComponent, 0, CompID - 1) : 0;
                        const size_t Index = ECSArchetypeComponentIndex(Refs, CompID) + Offset;
                        ComponentData[CompID] = CCArrayGetElementAtIndex(Refs->archetype.ptr->components[Index], Refs->archetype.index);
                    }
                    
                    CCArray *Duplicates = ComponentData[CompID];
                    const size_t Count = CCArrayGetCount(*Duplicates);
                    if (Count)
                    {
                        if (ID & ECSComponentStorageModifierDestructor)
                        {
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
                            ECSDuplicateArchetypeComponentDestructors[CompID](CCArrayGetElementAtIndex(*Duplicates, Count - 1), ID);
#else
                            CopiedComponents[CopiedComponentCount++] = (typeof(*CopiedComponents)){
                                .destructor = ECSDuplicateArchetypeComponentDestructors[CompID],
                                .component = {
                                    .id = ID,
                                    .data = ECSSharedZoneStore(CCArrayGetElementAtIndex(*Duplicates, Count - 1), ECSDuplicateArchetypeComponentSizes[CompID])
                                }
                            };
#endif
                        }
                        
                        CCArrayRemoveElementAtIndex(*Duplicates, Count - 1);
                    }
                    
                    else
                    {
                        if (ID & ECSComponentStorageModifierDestructor) CCArrayDestroy(*Duplicates);
                        
                        const size_t Offset = LastID < CompID ? LastIndex : 0;
                        
                        LastIndex = SortedSub(Refs->archetype.component.ids + Offset, Refs->archetype.component.count-- - Offset, CompID) + Offset;
                        IndexCount++;
                        CCBitsSet(RemovedComponent, CompID);
                        CCBitsClear(Refs->has, CompID);
                        
                        LastID = CompID;
                    }
                }
                break;
            }
                
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypePacked:
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeIndexed:
            case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeLocal:
                ECSEntityRemoveDuplicateComponent(Context, Entity, ID, -1, 1);
                break;
                
            default:
                CCAssertLog(0, "Unsupported component type");
                break;
        }
    }
    
    if (IndexCount)
    {
        if ((Count = Refs->archetype.component.count))
        {
            const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
            ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
            
            if (CC_UNLIKELY(!Archetype->entities))
            {
                const size_t ChunkSize = ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ArchID, Count);
                
                Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);
                
                for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], ChunkSize);
            }
            
            const size_t Index = CCArrayAppendElement(Archetype->entities, &Entity);
            
            const size_t PrevCount = Count + IndexCount;
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0, ComponentIndex = 0; Loop < PrevCount; Loop++)
            {
                _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
                
                const ECSArchetypeComponentID ID = Refs->archetype.component.ids[Loop];
                
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
                if (ID == INT8_MAX) ComponentIndex++;
#else
                if (ID == UINT8_MAX) ComponentIndex++;
#endif
                else
                {
                    const void *Copy = CCArrayGetElementAtIndex(PrevArchetype->components[Loop], Refs->archetype.index);
                    CCArrayAppendElement(Archetype->components[Loop - ComponentIndex], Copy);
                }
            }
            
            ArchetypeRemove(Context, PrevArchetype, PrevCount, Refs->archetype.index);
            
            Refs->archetype.ptr = Archetype;
            Refs->archetype.index = Index;
        }
        
        else
        {
            ArchetypeRemove(Context, Refs->archetype.ptr, IndexCount, Refs->archetype.index);
            
            Refs->archetype.ptr = NULL;
            Refs->archetype.index = 0;
        }
    }
    
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
    for (size_t Loop = 0; Loop < CopiedComponentCount; Loop++)
    {
        CopiedComponents[Loop].destructor(CopiedComponents[Loop].component.data, CopiedComponents[Loop].component.id);
    }
    
    CCMemoryZoneRestore(ECSSharedZone);
#endif
}

size_t ECSArchetypeComponentIndex(ECSComponentRefs *Refs, ECSComponentID ID)
{
    CCAssertLog((ID & ECSComponentStorageMask) == 0, "ID must not have any modifiers");
    
#if ECS_ARCHETYPE_COMPONENT_ID_SIMD_LOOKUP && CC_HARDWARE_VECTOR_SUPPORT_SSSE3
    const __m128i Zero = _mm_setzero_si128();
    const __m128i One = _mm_set1_epi8(1);
    const __m128i Value = _mm_set1_epi8(ID);
    
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
    __m128i Mask = _mm_and_si128(_mm_cmplt_epi8(_mm_load_si128((__m128i*)Refs->archetype.component.ids), Value), One);
    for (size_t Loop = 16; Loop < ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX); Loop += 16)
    {
        Mask = _mm_add_epi8(Mask, _mm_and_si128(_mm_cmplt_epi8(_mm_load_si128((__m128i*)(Refs->archetype.component.ids + Loop)), Value), One));
    }
    
    const uint32_t Index = _mm_cvtsi128_si32(_mm_hadd_epi16(_mm_hadd_epi16(_mm_hadd_epi16(Mask, Zero), Zero), Zero));
    
    return (Index & 0xff) + (Index >> 8);
#else
    __m128i Max = _mm_max_epu8(_mm_load_si128((__m128i*)Refs->archetype.component.ids), Value);
    __m128i Mask = _mm_and_si128(_mm_cmpeq_epi8(Max, Value), One);
    for (size_t Loop = 16; Loop < ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX); Loop += 16)
    {
        Max = _mm_max_epu8(_mm_load_si128((__m128i*)(Refs->archetype.component.ids + Loop)), Value);
        Mask = _mm_add_epi8(Mask, _mm_and_si128(_mm_cmpeq_epi8(Max, Value), One));
    }
    
    const uint32_t Index = _mm_cvtsi128_si32(_mm_hadd_epi16(_mm_hadd_epi16(_mm_hadd_epi16(Mask, Zero), Zero), Zero));
    
    return (Index & 0xff) + (Index >> 8) - 1;
#endif
#else
    const size_t Index = FindIndex(Refs->archetype.component.ids, Refs->archetype.component.count, ECS_ARCHETYPE_COMPONENT_MAX, ID);
    
    CCAssertLog(Index != SIZE_MAX, "Invalid ID for archetype");
    
    return Index;
#endif
}
