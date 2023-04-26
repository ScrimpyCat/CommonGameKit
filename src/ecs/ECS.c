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
    void *changes;
    CCArray components[ECS_SYSTEM_EXECUTOR_COMPONENT_MAX];
    struct {
        size_t offset;
        size_t count;
    } archetype;
    ECSExecutionGroup *executionGroup;
    ECSContext *world;
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

#define ECS_COMPONENT_ACCESS_RELEASE_MAX 16

typedef struct {
    size_t ids[ECS_COMPONENT_ACCESS_RELEASE_MAX];
    struct {
        const ECSSystemAccess *access;
        ECSExecutionGroup *executionGroup;
    } release[ECS_COMPONENT_ACCESS_RELEASE_MAX];
    size_t count;
} ECSComponentAccessRelease;

#ifndef ECS_WORKER_THREAD_MAX
#define ECS_WORKER_THREAD_MAX 128
#endif

int ECSWorker(ECSWorkerID WorkerID);
static inline size_t ECSComponentBaseIndex(ECSComponentID ID);

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
static _Atomic(size_t) AccessReleaseIndex[ECS_WORKER_THREAD_MAX] = {ATOMIC_VAR_INIT(0)};

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
                
                LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID], LocalAccessReleaseIndex, memory_order_consume);
            }
            
            for (size_t Loop = 0, ArchCount = Executor->archetype.count; Loop < ArchCount; Loop++)
            {
                const ECSArchetypePointer *ArchPointer = &Access->archetype.pointer[Executor->archetype.offset + Loop];
                ECSArchetype *Archetype = (void*)Executor->world + ArchPointer->archetype;
                
                if ((Archetype->entities) && (CCArrayGetCount(Archetype->entities)))
                {
                    for (size_t Loop2 = 0, Count = ArchPointer->count, Index = ArchPointer->componentIndexes; Loop2 < Count; Loop2++)
                    {
                        // TODO: non-arch components in the exec component list should be added after (initial indexes reserved for the arch components)
                        Executor->components[Loop2] = Archetype->components[*(ArchetypeComponentIndexes + Index + Loop2)];
                    }
                    
                    Executor->update(Executor->world, Executor->components, Archetype->entities, Executor->changes, Executor->time);
                }
            }
            
            const size_t Index = AccessReleases[WorkerID][LocalAccessReleaseIndex].count++;
            AccessReleases[WorkerID][LocalAccessReleaseIndex].release[Index].access = Access;
            AccessReleases[WorkerID][LocalAccessReleaseIndex].release[Index].executionGroup = Executor->executionGroup;
            
            atomic_thread_fence(memory_order_release);
            LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID], LocalAccessReleaseIndex, memory_order_consume);
        }
        
        else
        {
            ECSWaiting(WorkerID);
            
            if (AccessReleases[WorkerID][LocalAccessReleaseIndex].count)
            {
                LocalAccessReleaseIndex = atomic_exchange_explicit(&AccessReleaseIndex[WorkerID], LocalAccessReleaseIndex, memory_order_consume);
            }
        }
    }
    
    return 0;
}

static size_t LocalAccessReleaseIndexes[ECS_WORKER_THREAD_MAX];

void ECSInit(void)
{
    for (size_t Loop = 0; Loop < ECS_WORKER_THREAD_MAX; Loop++) LocalAccessReleaseIndexes[Loop] = 2;
}

#if CC_HARDWARE_PTR_64
typedef uint64_t ECSContextAccessFlag;
#elif CC_HARDWARE_PTR_32
typedef uint32_t ECSContextAccessFlag;
#endif

static _Bool ECSSubmitSystem(ECSContext *Context, ECSTime Time, size_t SystemIndex, const ECSSystemRange *Range, const ECSSystemAccess *Access, const ECSSystemUpdate *Update, ECSContextAccessFlag *AccessFlags, uint16_t *Refs, uint8_t *Block, uint8_t BlockBit, ECSExecutionGroup *State, CCConcurrentPoolStage *Stage)
{
    _Bool Parallel = ECS_SYSTEM_UPDATE_GET_PARALLEL(Update[SystemIndex]);
    const size_t ArchCount = Access[SystemIndex].archetype.count;
    const size_t RefCount = (Parallel ? ArchCount : 1); // TODO: handle non-arch +1
    
    for (size_t Loop4 = 0, IdCount = Access[SystemIndex].read.count; Loop4 < IdCount; Loop4++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].read.ids[Loop4]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        if ((AccessFlags[AccessFlagByteIndex] >> ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8))) & 1) return FALSE;
    }
    
    for (size_t Loop4 = 0, IdCount = Access[SystemIndex].write.count; Loop4 < IdCount; Loop4++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].write.ids[Loop4]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        if ((AccessFlags[AccessFlagByteIndex] >> ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8))) & 3) return FALSE;
    }
    
    for (size_t Loop4 = 0, IdCount = Access[SystemIndex].read.count; Loop4 < IdCount; Loop4++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].read.ids[Loop4]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        AccessFlags[AccessFlagByteIndex] |= 2 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8));
        Refs[ComponentIndex] += RefCount;
    }
    
    for (size_t Loop4 = 0, IdCount = Access[SystemIndex].write.count; Loop4 < IdCount; Loop4++)
    {
        const size_t ComponentIndex = ECSComponentBaseIndex(Access[SystemIndex].write.ids[Loop4]);
        const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
        
        AccessFlags[AccessFlagByteIndex] |= 1 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8));
        Refs[ComponentIndex] += RefCount;
    }
    
    ECSSystemExecutor Executor = {
        .access = &Access[SystemIndex],
        .time = Time,
        .update = ECS_SYSTEM_UPDATE_GET_UPDATE(Update[SystemIndex]),
        .executionGroup = State,
        .changes = NULL,
        .world = Context
    };
    
    if (ECS_SYSTEM_UPDATE_GET_PARALLEL(Update[SystemIndex]))
    {
        Executor.archetype.count = 1;
        
        for (size_t Loop4 = 0; Loop4 < ArchCount; Loop4++)
        {
            Executor.archetype.offset = Loop4;
            CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
        }
        
        State->running += ArchCount;
    }
    
    else
    {
        Executor.archetype.offset = 0;
        Executor.archetype.count = ArchCount;
        CCConcurrentPoolStagePush(SystemExecutorPool, Executor, ECS_SYSTEM_EXECUTION_POOL_MAX, Stage);
        
        State->running++;
    }
    
    *Block |= 1 << BlockBit;
    
    return TRUE;
}

void ECSTick(ECSContext *Context, const ECSGroup *Groups, size_t GroupCount, ECSExecutionGroup *State, ECSTime DeltaTime)
{
    size_t RunGroupIndexes[GroupCount], RunCount = 0;
    ECSTime GroupTimes[GroupCount];
    
    for (size_t Loop = 0; Loop < GroupCount; Loop++)
    {
        State[Loop].running = ATOMIC_VAR_INIT(0);
        
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
                    const int BitCount = (Loop2 + 1 != BlockCount ? 8 : SystemCount % 8);
                    
                    if (Block != (0xff >> (8 - BitCount)))
                    {
                        Completed = FALSE;
                        
                        for (int Loop3 = 0; Loop3 < BitCount; Loop3++)
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
                                        const int BitCount = (Loop4 + 1 != BlockCount ? 8 : SystemCount % 8);
                                        const uint8_t RunBlock = ~*Block & GraphBlock;
                                        
                                        if (RunBlock)
                                        {
                                            for (int Loop5 = 0; Loop5 < BitCount; Loop5++)
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
        
        while (!AccessReleases[TargetIndex][(LocalAccessReleaseIndexes[TargetIndex] = atomic_exchange_explicit(&AccessReleaseIndex[TargetIndex], LocalAccessReleaseIndexes[TargetIndex], memory_order_consume))].count)
        {
            ECSWaiting(-1);
            
            TargetIndex = (TargetIndex + 1) % WorkerThreadCount;
        }
        
        // FIXME: Only if spreading out the workload (make this a compile time conditional option mode)
        for (size_t Loop = 1; Loop < WorkerThreadCount; Loop++)
        {
            const size_t Index = (TargetIndex + Loop) % WorkerThreadCount;
            LocalAccessReleaseIndexes[Index] = atomic_exchange_explicit(&AccessReleaseIndex[Index], LocalAccessReleaseIndexes[Index], memory_order_consume);
        }
        
        // TODO: combine with loop above (as will add additional time between accessing each release index so higher chance we'll get a non-empty one)
        for (size_t Loop = 0; Loop < WorkerThreadCount; Loop++)
        {
            const size_t Index = Loop;
            for (size_t Loop2 = 0, Count = AccessReleases[Loop][LocalAccessReleaseIndexes[Index]].count; Loop2 < Count; Loop2++)
            {
                const ECSSystemAccess *Access = AccessReleases[Loop][LocalAccessReleaseIndexes[Index]].release[Loop2].access;
                
                for (size_t Loop3 = 0, IdCount = Access->read.count; Loop3 < IdCount; Loop3++)
                {
                    const size_t ComponentIndex = ECSComponentBaseIndex(Access->read.ids[Loop3]);
                    const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
                    
                    if (!--Refs[ComponentIndex]) AccessFlags[AccessFlagByteIndex] &= ~(3 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8)));
                }
                
                for (size_t Loop3 = 0, IdCount = Access->write.count; Loop3 < IdCount; Loop3++)
                {
                    const size_t ComponentIndex = ECSComponentBaseIndex(Access->write.ids[Loop3]);
                    const size_t AccessFlagByteIndex = (ComponentIndex * 2) / (sizeof(ECSContextAccessFlag) * 8);
                    
                    if (!--Refs[ComponentIndex]) AccessFlags[AccessFlagByteIndex] &= ~(3 << ((ComponentIndex * 2) % (sizeof(ECSContextAccessFlag) * 8)));
                }
                
                AccessReleases[Loop][LocalAccessReleaseIndexes[Index]].release[Loop2].executionGroup->running--;
            }
            
            AccessReleases[Loop][LocalAccessReleaseIndexes[Index]].count = 0;
        }
        
        if (!RunCount) goto WaitForWorkers;
    }
    
Finished:;
}

static inline size_t ECSComponentBaseIndex(ECSComponentID ID)
{
    size_t Index = ID & ~ECSComponentTypeMask;
    
    switch (ID & ECSComponentTypeMask)
    {
        case ECSComponentTypeIndividual:
            Index += ECS_ARCHETYPE_COMPONENT_MAX;
        case ECSComponentTypeArchetype:
            return Index;
    }
    
    CCAssertLog(0, "Unsupported component type");
    
    return SIZE_MAX;
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
        
        CC_BITS_INIT_CLEAR(Ref->archetype.has);
        CC_BITS_INIT_CLEAR(Ref->individual.has);
    }
}

void ECSEntityCreate(ECSContext *Context, ECSEntity *Entities, size_t Count)
{
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

const size_t *ECSArchetypeComponentSizes;
const size_t *ECSIndividualComponentSizes;

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
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t AddedIndex = SortedAdd(Refs->archetype.component.ids, Refs->archetype.component.count++, ID);
        const size_t Count = Refs->archetype.component.count;
        const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
        ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
        
        if (CC_UNLIKELY(!Archetype->entities))
        {
            Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);

            for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], 16);
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
        CCBitsSet(Refs->archetype.has, ID);
    }
}

void ECSArchetypeRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t RemovedIndex = SortedSub(Refs->archetype.component.ids, Refs->archetype.component.count--, ID);
        
        const size_t Count = Refs->archetype.component.count;
        if (Count)
        {
            const size_t ArchID = ArchtypeIndex(Refs->archetype.component.ids, Count);
            ECSArchetype *Archetype = ((void*)Context + ArchetypeOffset[Count].base) + (ArchetypeOffset[Count].size * ArchID);
            
            if (CC_UNLIKELY(!Archetype->entities))
            {
                Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);
                
                for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], 16);
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
        
        CCBitsClear(Refs->archetype.has, ID);
    }
}

void ECSIndividualAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    if (!ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentTypeMask);
        ECSComponent *Individual = &Context->components[Index];
        
        CCArray(ECSEntity) Entities = Individual->entities;
        CCArray Components = *Individual->components;
        
        if (CC_UNLIKELY(!Components))
        {
            Individual->entities = (Entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16));
            
            *Individual->components = (Components = CCArrayCreate(CC_STD_ALLOCATOR, ECSIndividualComponentSizes[Index], 16));
        }
        
        CCArrayAppendElement(Components, Data);
        Refs->individual.indexes[Index] = CCArrayAppendElement(Entities, &Entity);
        
        CCBitsSet(Refs->individual.has, Index);
    }
}

void ECSIndividualRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    if (ECSEntityHasComponent(Context, Entity, ID))
    {
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        const size_t Index = (ID & ~ECSComponentTypeMask);
        ECSComponent *Individual = &Context->components[Index];
        
        CCArray(ECSEntity) Entities = Individual->entities;
        CCArray Components = *Individual->components;
        const size_t EntityIndex = Refs->individual.indexes[Index];
        const size_t LastIndex = CCArrayGetCount(Components) - 1;
        
        if (EntityIndex != LastIndex)
        {
            const void *Data = CCArrayGetElementAtIndex(Components, LastIndex);
            CCArrayReplaceElementAtIndex(Components, EntityIndex, Data);
            CCArrayRemoveElementAtIndex(Components, LastIndex);
            
            const size_t *Entity = CCArrayGetElementAtIndex(Entities, LastIndex);
            CCArrayReplaceElementAtIndex(Entities, EntityIndex, Entity);
            CCArrayRemoveElementAtIndex(Entities, LastIndex);
        }
        
        else
        {
            CCArrayRemoveElementAtIndex(Components, LastIndex);
            CCArrayRemoveElementAtIndex(Entities, LastIndex);
        }
        
        Refs->individual.indexes[Index] = SIZE_MAX;
        CCBitsClear(Refs->individual.has, Index);
    }
}

void ECSEntityAddComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t Count)
{
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    void *ComponentData[ECS_ARCHETYPE_MAX];
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) AddedComponent;
    
    CC_BITS_INIT_CLEAR(AddedComponent);
    
    size_t IndexCount = 0, LastIndex = 0;
    ECSArchetypeComponentID LastID = 0;
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        switch (Components[Loop].id & ECSComponentTypeMask)
        {
            case ECSComponentTypeArchetype:
            {
                const ECSArchetypeComponentID ID = Components[Loop].id;
                
                if (!ECSEntityHasComponent(Context, Entity, ID))
                {
                    const size_t Offset = LastID < ID ? LastIndex : 0;
                    
                    LastIndex = SortedAdd(Refs->archetype.component.ids + Offset, Refs->archetype.component.count++ - Offset, ID) + Offset;
                    ComponentData[IndexCount++] = Components[Loop].data;
                    CCBitsSet(AddedComponent, ID);
                    CCBitsSet(Refs->archetype.has, ID);
                    
                    LastID = ID;
                }
                break;
            }
                
            case ECSComponentTypeIndividual:
                ECSIndividualAddComponent(Context, Entity, Components[Loop].data, Components[Loop].id);
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
            Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);

            for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], 16);
        }
        
        const size_t Index = CCArrayAppendElement(Archetype->entities, &Entity);
        
        if (Refs->archetype.ptr)
        {
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0, ComponentIndex = 0; Loop < Count; Loop++)
            {
                const ECSArchetypeComponentID ID = Refs->archetype.component.ids[Loop];
                
                if (CCBitsGet(AddedComponent, ID))
                {
                    CCArrayAppendElement(Archetype->components[Loop], ComponentData[ComponentIndex++]);
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
                CCArrayAppendElement(Archetype->components[Loop], ComponentData[Loop]);
            }
        }
        
        Refs->archetype.ptr = Archetype;
        Refs->archetype.index = Index;
    }
}

void ECSEntityRemoveComponents(ECSContext *Context, ECSEntity Entity, ECSComponentID *IDs, size_t Count)
{
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    CCBits(uint64_t, ECS_ARCHETYPE_COMPONENT_MAX) RemovedComponent;
    
    CC_BITS_INIT_CLEAR(RemovedComponent);
    
    size_t IndexCount = 0, LastIndex = 0;
    ECSArchetypeComponentID LastID = 0;
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        const ECSArchetypeComponentID ID = IDs[Loop];
        
        if (ECSEntityHasComponent(Context, Entity, ID))
        {
            switch (ID & ECSComponentTypeMask)
            {
                case ECSComponentTypeArchetype:
                {
                    const size_t Offset = LastID < ID ? LastIndex : 0;
                    
                    LastIndex = SortedSub(Refs->archetype.component.ids + Offset, Refs->archetype.component.count-- - Offset, ID) + Offset;
                    IndexCount++;
                    CCBitsSet(RemovedComponent, ID);
                    CCBitsClear(Refs->archetype.has, ID);
                    
                    LastID = ID;
                    break;
                }
                    
                case ECSComponentTypeIndividual:
                    ECSIndividualRemoveComponent(Context, Entity, ID);
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported component type");
                    break;
            }
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
                Archetype->entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);
                
                for (size_t Loop = 0; Loop < Count; Loop++) Archetype->components[Loop] = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[Refs->archetype.component.ids[Loop]], 16);
            }
            
            const size_t Index = CCArrayAppendElement(Archetype->entities, &Entity);
            
            const size_t PrevCount = Count + IndexCount;
            ECSArchetype *PrevArchetype = Refs->archetype.ptr;
            
            for (size_t Loop = 0, ComponentIndex = 0; Loop < PrevCount; Loop++)
            {
                const ECSArchetypeComponentID ID = Refs->archetype.component.ids[Loop];
                
                if (CCBitsGet(RemovedComponent, ID)) ComponentIndex++;
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
}

size_t ECSArchetypeComponentIndex(ECSComponentRefs *Refs, ECSComponentID ID)
{
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
