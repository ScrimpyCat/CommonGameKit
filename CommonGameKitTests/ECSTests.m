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

//ecs_tool setup -i CommonGameKitTests/ECSTests.h src/ecs/systems/*.h src/ecs/components/*.h -a CommonGameKitTests/ECSTestAccessors.h --max-local 64 -c ecs_env -e 'ECS_COMPONENT_ID_BASE=1' CommonGameKitTests/ECSTestData.h

#import <XCTest/XCTest.h>
#import "ECS.h"
#define ECS_PACKED_COMPONENT_ID_BASE 1
#import "ECSComponentIDs.h"
#import "ECSMonitorComponent.h"
#import "ECSMonitorSystem.h"
#import "ECSTestAccessors.h"
#import "ECSTests.h"
#import "ECSTestData.h"
#import "ECSMonitor.h"
#import "ECSBinaryMonitor.h"


static _Atomic(size_t) RunCount = ATOMIC_VAR_INIT(0);

static struct {
    _Atomic(size_t) first;
    _Atomic(size_t) last;
} RunStats[15] = {
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) },
    { ATOMIC_VAR_INIT(SIZE_MAX), ATOMIC_VAR_INIT(0) }
};

#define RUN_Sys1ReadA_WriteB 0
#define RUN_Sys2ReadAC_WriteB 1
#define RUN_Sys3ReadAC_WriteD 2
#define RUN_Sys4ReadA 3
#define RUN_Sys5ReadC 4
#define RUN_Sys6ReadAC 5
#define RUN_Sys7WriteB 6
#define RUN_Sys8ReadD_WriteC 7
#define RUN_Sys9ReadFGH_WriteAI 8
#define RUN_Sys10WriteJ 9
#define RUN_Sys11ReadAWithArchTag 10
#define RUN_Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA 11
#define RUN_Sys13ReadDestroyMeTag 12
#define RUN_Sys14ReadAJ 13
#define RUN_Sys15ReadH 14


#define RECORD_RUN RecordRunStat(CC_CAT(RUN_, ECS_SYSTEM_NAME))

static void RecordRunStat(size_t Index)
{
    size_t CurrentRunIndex = atomic_fetch_add(&RunCount, 1);
    size_t First = atomic_load(&RunStats[Index].first);
    while (CurrentRunIndex < First)
    {
        if (atomic_compare_exchange_weak(&RunStats[Index].first, &First, CurrentRunIndex)) break;
        
        First = atomic_load(&RunStats[Index].first);
    }
    
    size_t Last = atomic_load(&RunStats[Index].last);
    while (CurrentRunIndex > Last)
    {
        if (atomic_compare_exchange_weak(&RunStats[Index].last, &Last, CurrentRunIndex)) break;
        
        Last = atomic_load(&RunStats[Index].last);
    }
}

static _Atomic(_Bool) CorrectAccessors = ATOMIC_VAR_INIT(TRUE);

#define CHECK_SIZE(component) if (CCArrayGetElementSize(ECS_GET(component)) != sizeof(component)) atomic_store(&CorrectAccessors, FALSE);

#define ECS_SYSTEM_NAME Sys1ReadA_WriteB
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
    CHECK_SIZE(CompB);
    
    const CompA *A = CCArrayGetData(ECS_GET(CompA));
    CompB *B = CCArrayGetData(ECS_GET(CompB));
    
    for (size_t Loop = 0, Count = CCArrayGetCount(Archetype->entities); Loop < Count; Loop++)
    {
        B[Loop].v[1] += A[Loop].v[0];
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys2ReadAC_WriteB
static ECS_SYSTEM(Sys2ReadAC_WriteB)
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
    CHECK_SIZE(CompC);
    CHECK_SIZE(CompB);
    
    const CompA *A = CCArrayGetData(ECS_GET(CompA));
    const CompC *C = CCArrayGetData(ECS_GET(CompC));
    CompB *B = CCArrayGetData(ECS_GET(CompB));
    
    for (size_t Loop = 0, Count = CCArrayGetCount(Archetype->entities); Loop < Count; Loop++)
    {
        B[Loop].v[0] += A[Loop].v[0] * C[Loop].v[2];
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys3ReadAC_WriteD
static ECS_SYSTEM(Sys3ReadAC_WriteD, (CompA, CompC), (CompD))
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
    CHECK_SIZE(CompC);
    CHECK_SIZE(CompD);
    
    ECS_ITER(const CompA *A, const CompC *C, CompD *D)
    {
        D->v[0] += A->v[0] * C->v[1];
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys4ReadA
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys5ReadC
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompC);
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys6ReadAC
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
    CHECK_SIZE(CompC);
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys7WriteB
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompB);
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys8ReadD_WriteC
static ECS_PARALLEL_SYSTEM(Sys8ReadD_WriteC)
{
    RECORD_RUN;
    
    CHECK_SIZE(CompC);
    CHECK_SIZE(CompD);
    
    ECS_ITER(CompC *C, const CompD *D)
    {
        C->v[1] = D->v[0] * 10;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys9ReadFGH_WriteAI
static void Sys9ReadFGH_WriteAI(ECSContext *Context, ECSArchetype *Archetype, const size_t *ArchetypeComponentIndexes, const size_t *ComponentOffsets, ECSRange Range, ECSTime Time)
{
    RECORD_RUN;
    
    CHECK_SIZE(CompF);
    CHECK_SIZE(CompG);
    CHECK_SIZE(CompH);
    CHECK_SIZE(CompI);
    CHECK_SIZE(CompA);
    
    ECS_ITER(CompA *A, const CompF *F, const CompG *G, const CompH *H, CompI *I)
    {
        A->v[0] += F->v[0] + G->v[0] + H->v[0] + F->v[5] + G->v[6] + H->v[7];
        I->v[0]++;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys10WriteJ
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompJ);
    
    ECS_ITER(CompJ *J)
    {
        J->v[0]++;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys11ReadAWithArchTag
static _Atomic(int) Sys11ReadAWithArchTagCounter = ATOMIC_VAR_INIT(0);
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    CHECK_SIZE(CompA);
    
    const size_t Count = CCArrayGetCount(ECS_GET(CompA));
    const ECSProxyEntity Base = ECSMutationStageEntityCreate(Context, Count);
    ECSMutableAddComponentState *AddComponentState = ECSMutationStageEntityAddComponents(Context, Count);
    ECSTypedComponent *SharedComponent = ECSMutationSetSharedData(Context, sizeof(ECSTypedComponent));

    SharedComponent->id = DESTROY_ME_TAG;
    SharedComponent->data = NULL;
    
    ECS_ITER(const CompA *A)
    {
        atomic_fetch_add_explicit(&Sys11ReadAWithArchTagCounter, 1, memory_order_relaxed);
        
        AddComponentState[ECSIterIndex].entity = Base + ECSIterIndex;
        AddComponentState[ECSIterIndex].count = 1;
        AddComponentState[ECSIterIndex].components = SharedComponent;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    ECS_ASSERT(LocalH, Local);
    ECS_ASSERT_NOT(LocalH, Duplicate, Tag);
    ECS_ASSERT(DuplicateA, Packed, Duplicate);
    ECS_ASSERT_NOT(DuplicateA, Tag);
    ECS_ASSERT(LocalDuplicateB, Local, Duplicate);
    ECS_ASSERT_NOT(LocalDuplicateB, Tag);
    
    if (!ECS_GET(DuplicateA)) return;
    
    if (CCArrayGetElementSize(ECS_GET(DuplicateA)) != sizeof(CCArray)) atomic_store(&CorrectAccessors, FALSE);
    else if ((CCArrayGetCount(ECS_GET(DuplicateA))) && (CCArrayGetElementSize(*(CCArray*)CCArrayGetElementAtIndex(ECS_GET(DuplicateA), 0)) != sizeof(CompA))) atomic_store(&CorrectAccessors, FALSE);
    
    int Multiplier = 1;
    ECS_ITER(ARRAY(DuplicateA) *DupA, const LocalH *LocH, const ARRAY(LocalDuplicateB) *DupB)
    {
        if ((CCArrayGetCount(*DupA) != LocH->v[0]) || (CCArrayGetCount(*DupB) != LocH->v[1]) || (ECSIterCount != 1))
        {
            Multiplier = 0;
            break;
        }
    }
    
    size_t Index = 0;
    ECS_ITER(DuplicateA *DupA, const LocalH *LocH, const LocalDuplicateB *DupB)
    {
        if ((CCArrayGetCount(*DupAArray) != LocH->v[0]) || (CCArrayGetCount(*DupBArray) != LocH->v[1]) || (ECSIterCount != 1) || (((*DupAIndex * 10) + *DupBIndex) != LocH->v[2 + Index++])) Multiplier = 0;
        
        DupA->v[0] += DupB->v[0] + DupB->v[1];
        DupA->v[0] *= Multiplier;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys13ReadDestroyMeTag
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    ECS_ASSERT(DestroyMeTag, Packed, Tag);
    
    ECS_ITER(const DestroyMeTag *Tag)
    {
        ECSEntity *Entity = ECSMutationStageEntityDestroy(ECS_CONTEXT_VAR, 1);
        *Entity = ECSIterEntity;
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys14ReadAJ
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    ECS_ITER(const CompJ *J, const CompA *A)
    {
        
    }
}
#undef ECS_SYSTEM_NAME

#define ECS_SYSTEM_NAME Sys15ReadH
static ECS_SYSTEM_FUN()
{
    RECORD_RUN;
    
    ECS_ITER(const CompH *H)
    {
        
    }
}
#undef ECS_SYSTEM_NAME

static _Bool RunCheckError = 0;

#define ECS_SYSTEM_NAME Sys16ReadCheckRunStateTag
static ECS_SYSTEM_FUN()
{
    static const int ExclusionTable[] = {
        RUN_Sys1ReadA_WriteB, RUN_Sys2ReadAC_WriteB, RUN_Sys7WriteB, -1,
        RUN_Sys2ReadAC_WriteB, RUN_Sys1ReadA_WriteB, RUN_Sys7WriteB, -1,
        RUN_Sys3ReadAC_WriteD, RUN_Sys8ReadD_WriteC, -1,
        //RUN_Sys4ReadA
        //RUN_Sys5ReadC
        //RUN_Sys6ReadAC
        RUN_Sys7WriteB, RUN_Sys1ReadA_WriteB, RUN_Sys2ReadAC_WriteB, -1,
        RUN_Sys8ReadD_WriteC, RUN_Sys2ReadAC_WriteB, RUN_Sys3ReadAC_WriteD, RUN_Sys5ReadC, RUN_Sys6ReadAC, -1,
        RUN_Sys9ReadFGH_WriteAI, RUN_Sys1ReadA_WriteB, RUN_Sys2ReadAC_WriteB, RUN_Sys3ReadAC_WriteD, RUN_Sys4ReadA, RUN_Sys6ReadAC, RUN_Sys11ReadAWithArchTag, RUN_Sys14ReadAJ, -1,
        RUN_Sys10WriteJ, RUN_Sys14ReadAJ, -1,
        //RUN_Sys11ReadAWithArchTag
        RUN_Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA, -1,
        //RUN_Sys13ReadDestroyMeTag
        //RUN_Sys14ReadAJ
        //RUN_Sys15ReadH
    };
    
    static const char * const NameTable[] = {
        "Sys1ReadA_WriteB",
        "Sys2ReadAC_WriteB",
        "Sys3ReadAC_WriteD",
        "Sys4ReadA",
        "Sys5ReadC",
        "Sys6ReadAC",
        "Sys7WriteB",
        "Sys8ReadD_WriteC",
        "Sys9ReadFGH_WriteAI",
        "Sys10WriteJ",
        "Sys11ReadAWithArchTag",
        "Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA",
        "Sys13ReadDestroyMeTag",
        "Sys14ReadAJ",
        "Sys15ReadH"
    };
    
    for (size_t Loop = 0, Count = sizeof(ExclusionTable) / sizeof(*ExclusionTable); Loop < Count; Loop++)
    {
        int Subject = ExclusionTable[Loop], Test;
        const size_t First = atomic_load(&RunStats[Subject].first);
        const size_t Last = atomic_load(&RunStats[Subject].last);
        
        while ((++Loop < Count) && ((Test = ExclusionTable[Loop]) != -1))
        {
            const size_t TestFirst = atomic_load(&RunStats[Test].first);
            const size_t TestLast = atomic_load(&RunStats[Test].last);
            
            if ((First < TestLast) && (Last > TestFirst))
            {
                printf("systems \"%s\" and \"%s\" should not execute at the same time\n", NameTable[Subject], NameTable[Test]);
                RunCheckError = TRUE;
            }
        }
    }
    
    for (size_t Loop = 0; Loop < sizeof(RunStats) / sizeof(*RunStats); Loop++)
    {
        atomic_store(&RunStats[Loop].first, SIZE_MAX);
        atomic_store(&RunStats[Loop].last, 0);
    }
}
#undef ECS_SYSTEM_NAME


static int TestDestructionCount = 0;
static void TestDestructor(void *Data, ECSComponentID ID)
{
    TestDestructionCount++;
}

static int MutationDestructionCount = 0;
static void MutationDestructor(void *Data, ECSComponentID ID)
{
    MutationDestructionCount++;
}

static size_t MutationCallbackEntityCount = 0;
static ECSEntity MutationCallbackEntityFirst = 0;
static size_t MutationCallbackSum = 0;

static void MutationCallback(ECSContext *Context, void *Data, ECSEntity *NewEntities, size_t NewEntityCount)
{
    MutationCallbackEntityCount = NewEntityCount;
    MutationCallbackEntityFirst = NewEntities[0];
    
    if (((ECSTypedComponent*)Data)->id == COMP_A)
    {
        MutationCallbackSum += ((CompA*)((ECSTypedComponent*)Data)->data)->v[0];
    }
    
    else if (((ECSTypedComponent*)Data)->id == COMP_B)
    {
        MutationCallbackSum += ((CompB*)((ECSTypedComponent*)Data)->data)->v[0] + ((CompB*)((ECSTypedComponent*)Data)->data)->v[1];
    }
}

@interface ECSTests : XCTestCase
@end

@implementation ECSTests

static ECSContext Context;

ECSMutableState MutableState = ECS_MUTABLE_STATE_CREATE(4096, 4096, 4096, 4096, 4096, 1048576);

+(void) setUp
{
    ECSComponentIDs = ComponentIDs;
    
    ECSArchetypeComponentIndexes = ArchetypeComponentIndexes;
    
    ECSArchetypeComponentSizes = ArchetypeComponentSizes;
    ECSPackedComponentSizes = PackedComponentSizes;
    ECSIndexedComponentSizes = IndexedComponentSizes;
    ECSLocalComponentSizes = LocalComponentSizes;
    ECSDuplicateArchetypeComponentSizes = DuplicateArchetypeComponentSizes;
    ECSDuplicatePackedComponentSizes = DuplicatePackedComponentSizes;
    ECSDuplicateIndexedComponentSizes = DuplicateIndexedComponentSizes;
    ECSDuplicateLocalComponentSizes = DuplicateLocalComponentSizes;
    
    ECSArchetypeComponentDestructors = ArchetypeComponentDestructors;
    ECSPackedComponentDestructors = PackedComponentDestructors;
    ECSIndexedComponentDestructors = IndexedComponentDestructors;
    ECSLocalComponentDestructors = LocalComponentDestructors;
    ECSDuplicateArchetypeComponentDestructors = DuplicateArchetypeComponentDestructors;
    ECSDuplicatePackedComponentDestructors = DuplicatePackedComponentDestructors;
    ECSDuplicateIndexedComponentDestructors = DuplicateIndexedComponentDestructors;
    ECSDuplicateLocalComponentDestructors = DuplicateLocalComponentDestructors;
    
    ECSMutableStateEntitiesMax = 4096;
    ECSMutableStateReplaceRegistryMax = 4096;
    ECSMutableStateAddComponentMax = 4069;
    ECSMutableStateRemoveComponentMax = 4069;
    ECSMutableStateCustomCallbackMax = 4096;
    ECSMutableStateSharedDataMax = 1048576;
    
    ECSInit();
    ECSWorkerCreate();
    ECSWorkerCreate();
    ECSWorkerCreate();
    ECSWorkerCreate();
    
    Context.mutations = &MutableState;
    
    Context.manager.map = CCArrayCreate(CC_ALIGNED_ALLOCATOR(ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), CC_ALIGN(sizeof(ECSComponentRefs) + LOCAL_STORAGE_SIZE, ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), 16);
    Context.manager.available = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(size_t), 16);
}

-(void) setUp
{
    [super setUp];
    
    Context.registry.id = NULL;
    
    if (Context.registry.registeredEntities)
    {
        CCDictionaryDestroy(Context.registry.registeredEntities);
        Context.registry.registeredEntities = NULL;
    }
    
    if (Context.registry.uniqueEntityIDs)
    {
        CCArrayDestroy(Context.registry.uniqueEntityIDs);
        Context.registry.uniqueEntityIDs = NULL;
    }
    
    if (Context.links.associations)
    {
        for (size_t Loop = 0, Count = CCArrayGetCount(Context.links.associations); Loop < Count; Loop++)
        {
            CCDictionary Assoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context.links.associations, Loop);
            if (Assoc) CCDictionaryDestroy(Assoc);
        }
        
        CCArrayDestroy(Context.links.associations);
        Context.links.associations = NULL;
    }
    
    ECSRegistryInit(&Context, CC_BIG_INT_FAST_0);
    
    ECSLinkMapInit(&Context);
}

-(void) checkRun
{
    XCTAssertFalse(RunCheckError, @"should not execute systems with conflicting read/write access at the same time");
    
    RunCheckError = FALSE;
}

-(void) testExample
{
    const size_t GroupCount = sizeof(Groups) / sizeof(*Groups);
    ECSExecutionGroup State[GroupCount];
    uint8_t ExecState[GroupCount][10];
    
    memset(ExecState, 0, sizeof(ExecState));
    
    for (size_t Loop = 0; Loop < GroupCount; Loop++)
    {
        State[Loop] = (ECSExecutionGroup){
            .executing = 0,
            .state = ExecState[Loop],
            .time = 0,
            .running = 0
        };
    }
    
    ECSEntity TestEntity;
    ECSEntityCreate(&Context, &TestEntity, 1);
    ECSEntityAddComponent(&Context, TestEntity, NULL, CHECK_RUN_STATE_TAG);
    
    ECSEntity EntitiesABC[5];
    ECSEntityCreate(&Context, EntitiesABC, 5);
    for (size_t Loop = 0; Loop < 5; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesABC[Loop], (ECSTypedComponent[3]){
            { COMP_A, &(CompA){ { 0 + (int)Loop } } },
            { COMP_B, &(CompB){ { 0 + (int)Loop, 1 } } },
            { COMP_C, &(CompC){ { 0 + (int)Loop, 1, 2 } } }
        }, 3);
    }
    
    ECSEntity EntitiesBCD[2];
    ECSEntityCreate(&Context, EntitiesBCD, 2);
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesBCD[Loop], (ECSTypedComponent[3]){
            { COMP_B, &(CompB){ { 5 + (int)Loop, 1 } } },
            { COMP_C, &(CompC){ { 5 + (int)Loop, 1, 2 } } },
            { COMP_D, &(CompD){ { 5 + (int)Loop, 1, 2, 3 } } }
        }, 3);
    }
    
    ECSEntity EntitiesAB[2];
    ECSEntityCreate(&Context, EntitiesAB, 2);
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesAB[Loop], (ECSTypedComponent[2]){
            { COMP_A, &(CompA){ { 7 + (int)Loop } } },
            { COMP_B, &(CompB){ { 7 + (int)Loop, 1 } } }
        }, 2);
    }
    
    ECSEntity EntitiesACD[1];
    ECSEntityCreate(&Context, EntitiesACD, 1);
    for (size_t Loop = 0; Loop < 1; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesACD[Loop], (ECSTypedComponent[3]){
            { COMP_A, &(CompA){ { 9 + (int)Loop } } },
            { COMP_C, &(CompC){ { 9 + (int)Loop, 1, 2 } } },
            { COMP_D, &(CompD){ { 9 + (int)Loop, 1, 2, 3 } } }
        }, 3);
    }
    
    ECSEntity EntitiesAFGHIJ[3];
    ECSEntityCreate(&Context, EntitiesAFGHIJ, 3);
    for (size_t Loop = 0; Loop < 3; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesAFGHIJ[Loop], (ECSTypedComponent[6]){
            { COMP_A, &(CompA){ { 10 + (int)Loop } } },
            { COMP_F, &(CompF){ { 10 + (int)Loop, 1, 2, 3, 4, 5 } } },
            { COMP_G, &(CompG){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6 } } },
            { COMP_H, &(CompH){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6, 7 } } },
            { COMP_I, &(CompI){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6, 7, 8 } } },
            { COMP_J, &(CompJ){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6, 7, 8, 9 } } },
        }, 6);
    }
    
    ECSEntityAddComponent(&Context, EntitiesABC[0], NULL, ARCH_TAG);
    ECSEntityAddComponent(&Context, EntitiesBCD[0], NULL, ARCH_TAG);
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], NULL, ARCH_TAG);
    
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(DuplicateA){ { 123 } }, DUPLICATE_A);
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(DuplicateA){ { 456 } }, DUPLICATE_A);
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(DuplicateA){ { 789 } }, DUPLICATE_A);
    
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(LocalH){ { 3, 2, 0, 1, 10, 11, 20, 21 } }, LOCAL_H);
    
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(LocalDuplicateB){ { 111, 1 } }, LOCAL_DUPLICATE_B);
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &(LocalDuplicateB){ { 222, 2 } }, LOCAL_DUPLICATE_B);
    
    ECSMonitorComponent Monitor = ECSBinaryMonitorCreate(CC_STD_ALLOCATOR, COMP_A, 3, 4, sizeof(int));
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &Monitor, ECS_MONITOR_COMPONENT);
    Monitor = ECSBinaryMonitorCreate(CC_STD_ALLOCATOR, COMP_J, 3, 4, sizeof(int));
    ECSEntityAddComponent(&Context, EntitiesAFGHIJ[0], &Monitor, ECS_MONITOR_COMPONENT);
    
    ECSTick(&Context, Groups, GroupCount, State, ECS_TIME_FROM_SECONDS(1.0 / 60.0));
    
    [self checkRun];
    
    XCTAssertTrue(atomic_load(&CorrectAccessors), @"component accessors are correct");
    XCTAssertEqual(atomic_load(&Sys11ReadAWithArchTagCounter), 2, @"correct number of entities are tagged");
    XCTAssertEqual(MutationDestructionCount, 0, @"should not have applied any mutations");
    
    const CompA *A = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_A);
    XCTAssertEqual(A->v[0], 0, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_A);
    XCTAssertEqual(A->v[0], 1, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_A);
    XCTAssertEqual(A->v[0], 2, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_A);
    XCTAssertEqual(A->v[0], 3, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_A);
    XCTAssertEqual(A->v[0], 4, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_A);
    XCTAssertEqual(A->v[0], 7, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_A);
    XCTAssertEqual(A->v[0], 8, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_A);
    XCTAssertEqual(A->v[0], 9, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    XCTAssertEqual(A->v[0], 10, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_A);
    XCTAssertEqual(A->v[0], 11, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_A);
    XCTAssertEqual(A->v[0], 12, @"should remain unchanged");
    
    const CompB *B = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_B);
    XCTAssertEqual(B->v[0], 0, @"should have changed");
    XCTAssertEqual(B->v[1], 1, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_B);
    XCTAssertEqual(B->v[0], 3, @"should have changed");
    XCTAssertEqual(B->v[1], 2, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_B);
    XCTAssertEqual(B->v[0], 6, @"should have changed");
    XCTAssertEqual(B->v[1], 3, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_B);
    XCTAssertEqual(B->v[0], 9, @"should have changed");
    XCTAssertEqual(B->v[1], 4, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_B);
    XCTAssertEqual(B->v[0], 12, @"should have changed");
    XCTAssertEqual(B->v[1], 5, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_B);
    XCTAssertEqual(B->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_B);
    XCTAssertEqual(B->v[0], 7, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 8, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_B);
    XCTAssertEqual(B->v[0], 8, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 9, @"should have changed");
    
    const CompC *C = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_C);
    XCTAssertEqual(C->v[0], 0, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_C);
    XCTAssertEqual(C->v[0], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_C);
    XCTAssertEqual(C->v[0], 2, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_C);
    XCTAssertEqual(C->v[0], 3, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_C);
    XCTAssertEqual(C->v[0], 4, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_C);
    XCTAssertEqual(C->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 50, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_C);
    XCTAssertEqual(C->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 60, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_C);
    XCTAssertEqual(C->v[0], 9, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 180, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    
    const CompD *D = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_D);
    XCTAssertEqual(D->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_D);
    XCTAssertEqual(D->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_D);
    XCTAssertEqual(D->v[0], 18, @"should have changed");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    
    const CompF *F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_F);
    XCTAssertEqual(F->v[0], 10, @"should remain unchanged");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_F);
    XCTAssertEqual(F->v[0], 11, @"should remain unchanged");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_F);
    XCTAssertEqual(F->v[0], 12, @"should remain unchanged");
    
    const CompG *G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_G);
    XCTAssertEqual(G->v[0], 10, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_G);
    XCTAssertEqual(G->v[0], 11, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_G);
    XCTAssertEqual(G->v[0], 12, @"should remain unchanged");
    
    const CompH *H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_H);
    XCTAssertEqual(H->v[0], 10, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_H);
    XCTAssertEqual(H->v[0], 11, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_H);
    XCTAssertEqual(H->v[0], 12, @"should remain unchanged");
    
    const CompI *I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_I);
    XCTAssertEqual(I->v[0], 10, @"should remain unchanged");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_I);
    XCTAssertEqual(I->v[0], 11, @"should remain unchanged");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_I);
    XCTAssertEqual(I->v[0], 12, @"should remain unchanged");
    
    const CompJ *J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    XCTAssertEqual(J->v[0], 11, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_J);
    XCTAssertEqual(J->v[0], 12, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_J);
    XCTAssertEqual(J->v[0], 13, @"should have changed");
    
    const CCArray *DupAArray = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    const DuplicateA *DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 459, @"should have changed");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 792, @"should have changed");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 1125, @"should have changed");
    
    
    ECSTick(&Context, Groups, GroupCount, State, ECS_TIME_FROM_SECONDS(1.0 / 60.0));
    
    [self checkRun];
    
    XCTAssertTrue(atomic_load(&CorrectAccessors), @"component accessors are correct");
    XCTAssertEqual(atomic_load(&Sys11ReadAWithArchTagCounter), 4, @"correct number of entities are tagged");
    XCTAssertEqual(MutationDestructionCount, 0, @"should not have applied any mutations");
    
    A = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_A);
    XCTAssertEqual(A->v[0], 0, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_A);
    XCTAssertEqual(A->v[0], 1, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_A);
    XCTAssertEqual(A->v[0], 2, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_A);
    XCTAssertEqual(A->v[0], 3, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_A);
    XCTAssertEqual(A->v[0], 4, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_A);
    XCTAssertEqual(A->v[0], 7, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_A);
    XCTAssertEqual(A->v[0], 8, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_A);
    XCTAssertEqual(A->v[0], 9, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    XCTAssertEqual(A->v[0], 58, @"should have changed");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_A);
    XCTAssertEqual(A->v[0], 62, @"should have changed");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_A);
    XCTAssertEqual(A->v[0], 66, @"should have changed");
    
    B = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_B);
    XCTAssertEqual(B->v[0], 0, @"should have changed");
    XCTAssertEqual(B->v[1], 1, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should have changed");
    XCTAssertEqual(B->v[1], 3, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_B);
    XCTAssertEqual(B->v[0], 10, @"should have changed");
    XCTAssertEqual(B->v[1], 5, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_B);
    XCTAssertEqual(B->v[0], 15, @"should have changed");
    XCTAssertEqual(B->v[1], 7, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_B);
    XCTAssertEqual(B->v[0], 20, @"should have changed");
    XCTAssertEqual(B->v[1], 9, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_B);
    XCTAssertEqual(B->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_B);
    XCTAssertEqual(B->v[0], 7, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 15, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_B);
    XCTAssertEqual(B->v[0], 8, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 17, @"should have changed");
    
    C = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_C);
    XCTAssertEqual(C->v[0], 0, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_C);
    XCTAssertEqual(C->v[0], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_C);
    XCTAssertEqual(C->v[0], 2, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_C);
    XCTAssertEqual(C->v[0], 3, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_C);
    XCTAssertEqual(C->v[0], 4, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_C);
    XCTAssertEqual(C->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 50, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_C);
    XCTAssertEqual(C->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 60, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_C);
    XCTAssertEqual(C->v[0], 9, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 16380, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    
    D = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_D);
    XCTAssertEqual(D->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_D);
    XCTAssertEqual(D->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_D);
    XCTAssertEqual(D->v[0], 1638, @"should have changed");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_F);
    XCTAssertEqual(F->v[0], 10, @"should remain unchanged");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_F);
    XCTAssertEqual(F->v[0], 11, @"should remain unchanged");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_F);
    XCTAssertEqual(F->v[0], 12, @"should remain unchanged");
    
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_G);
    XCTAssertEqual(G->v[0], 10, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_G);
    XCTAssertEqual(G->v[0], 11, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_G);
    XCTAssertEqual(G->v[0], 12, @"should remain unchanged");
    
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_H);
    XCTAssertEqual(H->v[0], 10, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_H);
    XCTAssertEqual(H->v[0], 11, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_H);
    XCTAssertEqual(H->v[0], 12, @"should remain unchanged");
    
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_I);
    XCTAssertEqual(I->v[0], 11, @"should have changed");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_I);
    XCTAssertEqual(I->v[0], 12, @"should have changed");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_I);
    XCTAssertEqual(I->v[0], 13, @"should have changed");
    
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    XCTAssertEqual(J->v[0], 12, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_J);
    XCTAssertEqual(J->v[0], 13, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_J);
    XCTAssertEqual(J->v[0], 14, @"should have changed");
    
    DupAArray = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 795, @"should have changed");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1128, @"should have changed");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 1461, @"should have changed");
    
    CompA CopyA = *(CompA*)ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    ECSMonitorTransform(CCArrayGetElementAtIndex(*(CCArray*)ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], ECS_MONITOR_COMPONENT), 0), &CopyA, 1);
    XCTAssertEqual(CopyA.v[0], 10, @"should revert change back to previous");
    
    CompJ CopyJ = *(CompJ*)ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    ECSMonitorTransform(CCArrayGetElementAtIndex(*(CCArray*)ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], ECS_MONITOR_COMPONENT), 1), &CopyJ, 1);
    XCTAssertEqual(CopyJ.v[0], 11, @"should revert change back to previous");
    
    for (size_t Loop = 0; Loop < 5; Loop++)
    {
        ECSEntityRemoveComponents(&Context, EntitiesABC[Loop], (ECSComponentID[3]){
            COMP_A,
            COMP_B,
            COMP_C,
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityRemoveComponents(&Context, EntitiesBCD[Loop], (ECSComponentID[3]){
            COMP_B,
            COMP_C,
            COMP_D,
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityRemoveComponents(&Context, EntitiesAB[Loop], (ECSComponentID[2]){
            COMP_A,
            COMP_B,
        }, 2);
    }
    
    for (size_t Loop = 0; Loop < 1; Loop++)
    {
        ECSEntityRemoveComponents(&Context, EntitiesACD[Loop], (ECSComponentID[3]){
            COMP_A,
            COMP_C,
            COMP_D,
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 3; Loop++)
    {
        ECSEntityRemoveComponents(&Context, EntitiesAFGHIJ[Loop], (ECSComponentID[5]){
            COMP_A,
            COMP_F,
            COMP_G,
            COMP_H,
            COMP_I,
        }, 5);
    }
    
    ECSEntityRemoveComponent(&Context, EntitiesABC[0], ARCH_TAG);
    
    ECSEntityRemoveComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    
    LocalH *LocH = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], LOCAL_H);
    LocH->v[0] = 2;
    LocH->v[6] = 0;
    LocH->v[7] = 0;
    
    ECSTick(&Context, Groups, GroupCount, State, ECS_TIME_FROM_SECONDS(1.0 / 60.0));
    
    [self checkRun];
    
    XCTAssertTrue(atomic_load(&CorrectAccessors), @"component accessors are correct");
    XCTAssertEqual(atomic_load(&Sys11ReadAWithArchTagCounter), 4, @"correct number of entities are tagged");
    XCTAssertEqual(MutationDestructionCount, 0, @"should not have applied any mutations");
    
    A = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_A);
    XCTAssertEqual(A, NULL, @"should have removed component");
    
    B = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    B = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_B);
    XCTAssertEqual(B, NULL, @"should have removed component");
    
    C = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    C = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_C);
    XCTAssertEqual(C, NULL, @"should have removed component");
    
    D = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_D);
    XCTAssertEqual(D, NULL, @"should have removed component");
    D = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_D);
    XCTAssertEqual(D, NULL, @"should have removed component");
    D = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_D);
    XCTAssertEqual(D, NULL, @"should have removed component");
    
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_F);
    XCTAssertEqual(F, NULL, @"should have removed component");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_F);
    XCTAssertEqual(F, NULL, @"should have removed component");
    F = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_F);
    XCTAssertEqual(F, NULL, @"should have removed component");
    
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_G);
    XCTAssertEqual(G, NULL, @"should have removed component");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_G);
    XCTAssertEqual(G, NULL, @"should have removed component");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_G);
    XCTAssertEqual(G, NULL, @"should have removed component");
    
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_H);
    XCTAssertEqual(H, NULL, @"should have removed component");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_H);
    XCTAssertEqual(H, NULL, @"should have removed component");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_H);
    XCTAssertEqual(H, NULL, @"should have removed component");
    
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_I);
    XCTAssertEqual(I, NULL, @"should have removed component");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_I);
    XCTAssertEqual(I, NULL, @"should have removed component");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_I);
    XCTAssertEqual(I, NULL, @"should have removed component");
    
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    XCTAssertEqual(J->v[0], 13, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_J);
    XCTAssertEqual(J->v[0], 14, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_J);
    XCTAssertEqual(J->v[0], 15, @"should have changed");
    
    DupAArray = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 1131, @"should have changed");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1464, @"should have changed");
    
    
    for (size_t Loop = 0; Loop < 5; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesABC[Loop], (ECSTypedComponent[3]){
            { COMP_A, &(CompA){ { 0 + (int)Loop } } },
            { COMP_B, &(CompB){ { 0 + (int)Loop, 1 } } },
            { COMP_C, &(CompC){ { 0 + (int)Loop, 1, 2 } } }
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesBCD[Loop], (ECSTypedComponent[3]){
            { COMP_B, &(CompB){ { 5 + (int)Loop, 1 } } },
            { COMP_C, &(CompC){ { 5 + (int)Loop, 1, 2 } } },
            { COMP_D, &(CompD){ { 5 + (int)Loop, 1, 2, 3 } } }
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesAB[Loop], (ECSTypedComponent[2]){
            { COMP_A, &(CompA){ { 7 + (int)Loop } } },
            { COMP_B, &(CompB){ { 7 + (int)Loop, 1 } } }
        }, 2);
    }
    
    for (size_t Loop = 0; Loop < 1; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesACD[Loop], (ECSTypedComponent[3]){
            { COMP_A, &(CompA){ { 9 + (int)Loop } } },
            { COMP_C, &(CompC){ { 9 + (int)Loop, 1, 2 } } },
            { COMP_D, &(CompD){ { 9 + (int)Loop, 1, 2, 3 } } }
        }, 3);
    }
    
    for (size_t Loop = 0; Loop < 3; Loop++)
    {
        ECSEntityAddComponents(&Context, EntitiesAFGHIJ[Loop], (ECSTypedComponent[5]){
            { COMP_A, &(CompA){ { 10 + (int)Loop } } },
            { COMP_F, &(CompF){ { 10 + (int)Loop, 1, 2, 3, 4, 5 } } },
            { COMP_G, &(CompG){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6 } } },
            { COMP_H, &(CompH){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6, 7 } } },
            { COMP_I, &(CompI){ { 10 + (int)Loop, 1, 2, 3, 4, 5, 6, 7, 8 } } },
        }, 5);
    }
    
    ECSEntityRemoveDuplicateComponent(&Context, EntitiesAFGHIJ[0], LOCAL_DUPLICATE_B, 0, 2);
    
    ECSMutationApply(&Context);
    
    ECSTick(&Context, Groups, GroupCount, State, ECS_TIME_FROM_SECONDS(1.0 / 60.0) * 2);
    ECSTick(&Context, Groups, GroupCount, State, 0);
    
    [self checkRun];
    
    XCTAssertTrue(atomic_load(&CorrectAccessors), @"component accessors are correct");
    XCTAssertEqual(atomic_load(&Sys11ReadAWithArchTagCounter), 6, @"correct number of entities are tagged");
    XCTAssertEqual(MutationDestructionCount, 0, @"should not have applied the destruction mutations");
    
    ECSMutationApply(&Context);
    XCTAssertEqual(MutationDestructionCount, 4, @"should have applied destruction mutations");
    
    A = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_A);
    XCTAssertEqual(A->v[0], 0, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_A);
    XCTAssertEqual(A->v[0], 1, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_A);
    XCTAssertEqual(A->v[0], 2, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_A);
    XCTAssertEqual(A->v[0], 3, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_A);
    XCTAssertEqual(A->v[0], 4, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_A);
    XCTAssertEqual(A->v[0], 7, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_A);
    XCTAssertEqual(A->v[0], 8, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_A);
    XCTAssertEqual(A->v[0], 9, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    XCTAssertEqual(A->v[0], 58, @"should have changed");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_A);
    XCTAssertEqual(A->v[0], 62, @"should have changed");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_A);
    XCTAssertEqual(A->v[0], 66, @"should have changed");
    
    B = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_B);
    XCTAssertEqual(B->v[0], 0, @"should have changed");
    XCTAssertEqual(B->v[1], 1, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should have changed");
    XCTAssertEqual(B->v[1], 3, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_B);
    XCTAssertEqual(B->v[0], 10, @"should have changed");
    XCTAssertEqual(B->v[1], 5, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_B);
    XCTAssertEqual(B->v[0], 15, @"should have changed");
    XCTAssertEqual(B->v[1], 7, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_B);
    XCTAssertEqual(B->v[0], 20, @"should have changed");
    XCTAssertEqual(B->v[1], 9, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_B);
    XCTAssertEqual(B->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_B);
    XCTAssertEqual(B->v[0], 7, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 15, @"should have changed");
    B = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_B);
    XCTAssertEqual(B->v[0], 8, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 17, @"should have changed");
    
    C = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_C);
    XCTAssertEqual(C->v[0], 0, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_C);
    XCTAssertEqual(C->v[0], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_C);
    XCTAssertEqual(C->v[0], 2, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_C);
    XCTAssertEqual(C->v[0], 3, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_C);
    XCTAssertEqual(C->v[0], 4, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_C);
    XCTAssertEqual(C->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 50, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_C);
    XCTAssertEqual(C->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 60, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_C);
    XCTAssertEqual(C->v[0], 9, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 16380, @"should have changed");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    
    D = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_D);
    XCTAssertEqual(D->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_D);
    XCTAssertEqual(D->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_D);
    XCTAssertEqual(D->v[0], 1638, @"should have changed");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_G);
    XCTAssertEqual(G->v[0], 10, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_G);
    XCTAssertEqual(G->v[0], 11, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_G);
    XCTAssertEqual(G->v[0], 12, @"should remain unchanged");
    
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_H);
    XCTAssertEqual(H->v[0], 10, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_H);
    XCTAssertEqual(H->v[0], 11, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_H);
    XCTAssertEqual(H->v[0], 12, @"should remain unchanged");
    
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_I);
    XCTAssertEqual(I->v[0], 11, @"should have changed");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_I);
    XCTAssertEqual(I->v[0], 12, @"should have changed");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_I);
    XCTAssertEqual(I->v[0], 13, @"should have changed");
    
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    XCTAssertEqual(J->v[0], 15, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_J);
    XCTAssertEqual(J->v[0], 16, @"should have changed");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_J);
    XCTAssertEqual(J->v[0], 17, @"should have changed");
    
    DupAArray = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 1131, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1464, @"should remain unchanged");
    
    
    ECSEntityRemoveComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    ECSEntityRemoveComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    
    ECSTick(&Context, Groups, GroupCount, State, 0);
    
    [self checkRun];
    
    XCTAssertTrue(atomic_load(&CorrectAccessors), @"component accessors are correct");
    XCTAssertEqual(atomic_load(&Sys11ReadAWithArchTagCounter), 6, @"correct number of entities are tagged");
    XCTAssertEqual(MutationDestructionCount, 4, @"should not have applied the destruction mutations");
    
    A = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_A);
    XCTAssertEqual(A->v[0], 0, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_A);
    XCTAssertEqual(A->v[0], 1, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_A);
    XCTAssertEqual(A->v[0], 2, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_A);
    XCTAssertEqual(A->v[0], 3, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_A);
    XCTAssertEqual(A->v[0], 4, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_A);
    XCTAssertEqual(A->v[0], 7, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_A);
    XCTAssertEqual(A->v[0], 8, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_A);
    XCTAssertEqual(A->v[0], 9, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_A);
    XCTAssertEqual(A->v[0], 58, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_A);
    XCTAssertEqual(A->v[0], 62, @"should remain unchanged");
    A = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_A);
    XCTAssertEqual(A->v[0], 66, @"should remain unchanged");
    
    B = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_B);
    XCTAssertEqual(B->v[0], 0, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 3, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_B);
    XCTAssertEqual(B->v[0], 10, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 5, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_B);
    XCTAssertEqual(B->v[0], 15, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 7, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_B);
    XCTAssertEqual(B->v[0], 20, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 9, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_B);
    XCTAssertEqual(B->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_B);
    XCTAssertEqual(B->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 1, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesAB[0], COMP_B);
    XCTAssertEqual(B->v[0], 7, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 15, @"should remain unchanged");
    B = ECSEntityGetComponent(&Context, EntitiesAB[1], COMP_B);
    XCTAssertEqual(B->v[0], 8, @"should remain unchanged");
    XCTAssertEqual(B->v[1], 17, @"should remain unchanged");
    
    C = ECSEntityGetComponent(&Context, EntitiesABC[0], COMP_C);
    XCTAssertEqual(C->v[0], 0, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[1], COMP_C);
    XCTAssertEqual(C->v[0], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[2], COMP_C);
    XCTAssertEqual(C->v[0], 2, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[3], COMP_C);
    XCTAssertEqual(C->v[0], 3, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesABC[4], COMP_C);
    XCTAssertEqual(C->v[0], 4, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_C);
    XCTAssertEqual(C->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 50, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_C);
    XCTAssertEqual(C->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 60, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    C = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_C);
    XCTAssertEqual(C->v[0], 9, @"should remain unchanged");
    XCTAssertEqual(C->v[1], 16380, @"should remain unchanged");
    XCTAssertEqual(C->v[2], 2, @"should remain unchanged");
    
    D = ECSEntityGetComponent(&Context, EntitiesBCD[0], COMP_D);
    XCTAssertEqual(D->v[0], 5, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesBCD[1], COMP_D);
    XCTAssertEqual(D->v[0], 6, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    D = ECSEntityGetComponent(&Context, EntitiesACD[0], COMP_D);
    XCTAssertEqual(D->v[0], 1638, @"should remain unchanged");
    XCTAssertEqual(D->v[1], 1, @"should remain unchanged");
    XCTAssertEqual(D->v[2], 2, @"should remain unchanged");
    XCTAssertEqual(D->v[3], 3, @"should remain unchanged");
    
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_G);
    XCTAssertEqual(G->v[0], 10, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_G);
    XCTAssertEqual(G->v[0], 11, @"should remain unchanged");
    G = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_G);
    XCTAssertEqual(G->v[0], 12, @"should remain unchanged");
    
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_H);
    XCTAssertEqual(H->v[0], 10, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_H);
    XCTAssertEqual(H->v[0], 11, @"should remain unchanged");
    H = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_H);
    XCTAssertEqual(H->v[0], 12, @"should remain unchanged");
    
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_I);
    XCTAssertEqual(I->v[0], 11, @"should remain unchanged");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_I);
    XCTAssertEqual(I->v[0], 12, @"should remain unchanged");
    I = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_I);
    XCTAssertEqual(I->v[0], 13, @"should remain unchanged");
    
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], COMP_J);
    XCTAssertEqual(J->v[0], 15, @"should remain unchanged");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[1], COMP_J);
    XCTAssertEqual(J->v[0], 16, @"should remain unchanged");
    J = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[2], COMP_J);
    XCTAssertEqual(J->v[0], 17, @"should remain unchanged");
    
    DupAArray = ECSEntityGetComponent(&Context, EntitiesAFGHIJ[0], DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    XCTAssertEqual(TestDestructionCount, 3, @"should be the corrent number of destroyed components with destructors");
    
    
    ECSTick(&Context, Groups, GroupCount, State, ECS_TIME_FROM_SECONDS(1.0 / 60.0));
    [self checkRun];
    
    XCTAssertEqual(MutationDestructionCount, 4, @"should not have applied the destruction mutations");
    ECSMutationApply(&Context);
    XCTAssertEqual(MutationDestructionCount, 6, @"should have applied the destruction mutations");
    
    
    ECSTypedComponent DuplicateInit[9] = {
        { DUPLICATE_A, &(DuplicateA){ { 0 } } },
        { DUPLICATE_A, &(DuplicateA){ { 1 } } },
        { DUPLICATE_A, &(DuplicateA){ { 2 } } },
        { DUPLICATE_A, &(DuplicateA){ { 3 } } },
        { DUPLICATE_A, &(DuplicateA){ { 4 } } },
        { DUPLICATE_A, &(DuplicateA){ { 5 } } },
        { LOCAL_DUPLICATE_B, &(LocalDuplicateB){ { 10, 11 } } },
        { DUPLICATE_A, &(DuplicateA){ { 6 } } },
        { LOCAL_DUPLICATE_B, &(LocalDuplicateB){ { 12, 13 } } },
    };
    
    ECSEntity EntityDups;
    ECSEntityCreate(&Context, &EntityDups, 1);
    
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 4);
    XCTAssertEqual(TestDestructionCount, 7, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 4, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 1, 4);
    XCTAssertEqual(TestDestructionCount, 13, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 2, 4);
    XCTAssertEqual(TestDestructionCount, 19, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 3, 4);
    XCTAssertEqual(TestDestructionCount, 24, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 2, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 3);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, -1, 4);
    XCTAssertEqual(TestDestructionCount, 31, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, -2, 4);
    XCTAssertEqual(TestDestructionCount, 37, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, -3, 4);
    XCTAssertEqual(TestDestructionCount, 43, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 2, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 4, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 2);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, -4, 4);
    XCTAssertEqual(TestDestructionCount, 48, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 3, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 4, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 3);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, -5, 4);
    XCTAssertEqual(TestDestructionCount, 53, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 4, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 4, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 2, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 3);
    XCTAssertEqual(DupA->v[0], 3, @"should remain unchanged");
    
    ECSEntityRemoveDuplicateComponent(&Context, EntityDups, DUPLICATE_A, 0, 4);
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[3]){
        DUPLICATE_A,
        DUPLICATE_A,
        DUPLICATE_A,
    }, 3);
    XCTAssertEqual(TestDestructionCount, 60, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 2, @"should remain unchanged");
    
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[3]){
        DUPLICATE_A,
        DUPLICATE_A,
        DUPLICATE_A,
    }, 3);
    XCTAssertEqual(TestDestructionCount, 63, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[3]){
        DUPLICATE_A,
        DUPLICATE_A,
        DUPLICATE_A,
    }, 3);
    XCTAssertEqual(TestDestructionCount, 63, @"should be the corrent number of destroyed components with destructors");
    
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityAddComponent(&Context, EntityDups, &(LocalDuplicateB){ 10, 11 }, LOCAL_DUPLICATE_B);
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[4]){
        DUPLICATE_A,
        DUPLICATE_A,
        LOCAL_DUPLICATE_B,
        DUPLICATE_A,
    }, 4);
    XCTAssertEqual(TestDestructionCount, 66, @"should be the corrent number of destroyed components with destructors");
    
    CCArray *DupBArray = ECSEntityGetComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    XCTAssertEqual(DupBArray, NULL, @"should not have the duplicate component array");
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 3, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 2, @"should remain unchanged");
    
    ECSEntityAddComponent(&Context, EntityDups, &(LocalDuplicateB){ 10, 11 }, LOCAL_DUPLICATE_B);
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[4]){
        DUPLICATE_A,
        DUPLICATE_A,
        LOCAL_DUPLICATE_B,
        DUPLICATE_A,
    }, 4);
    XCTAssertEqual(TestDestructionCount, 69, @"should be the corrent number of destroyed components with destructors");
    
    DupBArray = ECSEntityGetComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    XCTAssertEqual(DupBArray, NULL, @"should not have the duplicate component array");
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 6);
    ECSEntityAddComponent(&Context, EntityDups, &(LocalDuplicateB){ 10, 11 }, LOCAL_DUPLICATE_B);
    ECSEntityAddComponent(&Context, EntityDups, &(LocalDuplicateB){ 12, 13 }, LOCAL_DUPLICATE_B);
    
    ECSEntityRemoveComponents(&Context, EntityDups, (ECSComponentID[7]){
        DUPLICATE_A,
        DUPLICATE_A,
        LOCAL_DUPLICATE_B,
        DUPLICATE_A,
        DUPLICATE_A,
        DUPLICATE_A,
        DUPLICATE_A,
    }, 7);
    XCTAssertEqual(TestDestructionCount, 75, @"should be the corrent number of destroyed components with destructors");
    
    DupBArray = ECSEntityGetComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    XCTAssertNotEqual(DupBArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupBArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupBArray), 1, @"should have the correct number of duplicate components");
    LocalDuplicateB *DupB = CCArrayGetElementAtIndex(*DupBArray, 0);
    XCTAssertEqual(DupB->v[0], 10, @"should remain unchanged");
    XCTAssertEqual(DupB->v[1], 11, @"should remain unchanged");
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    ECSEntityRemoveComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    
    DupBArray = ECSEntityGetComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    XCTAssertEqual(DupBArray, NULL, @"should not have the duplicate component array");
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertEqual(DupAArray, NULL, @"should not have the duplicate component array");
    
    ECSEntityAddComponents(&Context, EntityDups, DuplicateInit, 9);
    
    DupBArray = ECSEntityGetComponent(&Context, EntityDups, LOCAL_DUPLICATE_B);
    XCTAssertNotEqual(DupBArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupBArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupBArray), 2, @"should have the correct number of duplicate components");
    DupB = CCArrayGetElementAtIndex(*DupBArray, 0);
    XCTAssertEqual(DupB->v[0], 10, @"should remain unchanged");
    XCTAssertEqual(DupB->v[1], 11, @"should remain unchanged");
    DupB = CCArrayGetElementAtIndex(*DupBArray, 1);
    XCTAssertEqual(DupB->v[0], 12, @"should remain unchanged");
    XCTAssertEqual(DupB->v[1], 13, @"should remain unchanged");
    DupAArray = ECSEntityGetComponent(&Context, EntityDups, DUPLICATE_A);
    XCTAssertNotEqual(DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertNotEqual(*DupAArray, NULL, @"should have the duplicate component array");
    XCTAssertEqual(CCArrayGetCount(*DupAArray), 7, @"should have the correct number of duplicate components");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 0);
    XCTAssertEqual(DupA->v[0], 0, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 1);
    XCTAssertEqual(DupA->v[0], 1, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 2);
    XCTAssertEqual(DupA->v[0], 2, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 3);
    XCTAssertEqual(DupA->v[0], 3, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 4);
    XCTAssertEqual(DupA->v[0], 4, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 5);
    XCTAssertEqual(DupA->v[0], 5, @"should remain unchanged");
    DupA = CCArrayGetElementAtIndex(*DupAArray, 6);
    XCTAssertEqual(DupA->v[0], 6, @"should remain unchanged");
    
    size_t CompCount = 0;
    ECSEntityGetComponents(&Context, EntityDups, NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"should not get any components");
    
    CompCount = 1;
    ECSEntityGetComponents(&Context, EntityDups, NULL, &CompCount);
    XCTAssertEqual(CompCount, 1, @"should only count 1 component");
    
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, EntityDups, NULL, &CompCount);
    XCTAssertEqual(CompCount, 2, @"should only count all components");
    
    ECSTypedComponent Comps[2];
    ECSEntityGetComponents(&Context, EntityDups, Comps, &CompCount);
    XCTAssertEqual(Comps[0].id, DUPLICATE_A, @"should retrieve the correct component");
    XCTAssertEqual(Comps[1].id, LOCAL_DUPLICATE_B, @"should retrieve the correct component");
    
    ECSEntityDestroy(&Context, EntitiesABC, sizeof(EntitiesABC) / sizeof(*EntitiesABC));
    ECSEntityDestroy(&Context, EntitiesBCD, sizeof(EntitiesBCD) / sizeof(*EntitiesBCD));
    ECSEntityDestroy(&Context, EntitiesAB, sizeof(EntitiesAB) / sizeof(*EntitiesAB));
    ECSEntityDestroy(&Context, EntitiesACD, sizeof(EntitiesACD) / sizeof(*EntitiesACD));
    ECSEntityDestroy(&Context, EntitiesAFGHIJ, sizeof(EntitiesAFGHIJ) / sizeof(*EntitiesAFGHIJ));
    ECSEntityDestroy(&Context, &EntityDups, 1);
    
    XCTAssertEqual(TestDestructionCount, 83, @"should be the corrent number of destroyed components with destructors");
    
    ECSEntity Entities[6];
    ECSEntityCreate(&Context, Entities, 6);
    
    XCTAssertEqual(Entities[0], EntityDups, @"should reuse old entities");
    XCTAssertEqual(Entities[1], EntitiesAFGHIJ[2], @"should reuse old entities");
    XCTAssertEqual(Entities[2], EntitiesAFGHIJ[1], @"should reuse old entities");
    XCTAssertEqual(Entities[3], EntitiesAFGHIJ[0], @"should reuse old entities");
    XCTAssertEqual(Entities[4], EntitiesACD[0], @"should reuse old entities");
    XCTAssertEqual(Entities[5], EntitiesAB[1], @"should reuse old entities");
    
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, Entities[0], NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"reusing a destroyed entity should have no components");
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, Entities[1], NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"reusing a destroyed entity should have no components");
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, Entities[2], NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"reusing a destroyed entity should have no components");
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, Entities[3], NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"reusing a destroyed entity should have no components");
    CompCount = SIZE_MAX;
    ECSEntityGetComponents(&Context, Entities[4], NULL, &CompCount);
    XCTAssertEqual(CompCount, 0, @"reusing a destroyed entity should have no components");
    
    ECSEntityDestroy(&Context, Entities, sizeof(Entities) / sizeof(*Entities));
    
    XCTAssertEqual(TestDestructionCount, 83, @"should be the corrent number of destroyed components with destructors");
    
    
    ECSProxyEntity Base = ECSMutationStageEntityCreate(&Context, 2);
    ECSProxyEntity Base2 = ECSMutationStageEntityCreate(&Context, 1);
    ECSMutableCustomCallbackState *CallbackState = ECSMutationStageCustomCallback(&Context, 2);
    ECSMutableAddComponentState *AddComponentState = ECSMutationStageEntityAddComponents(&Context, 2);
    ECSTypedComponent *TypedComponents = ECSMutationSetSharedData(&Context, sizeof(ECSTypedComponent) * 3);
    CompA *CompAData = ECSMutationSetSharedData(&Context, sizeof(CompAData) * 2);
    CompB *CompBData = ECSMutationSetSharedData(&Context, sizeof(CompBData));
    
    AddComponentState[0].entity = Base + 0;
    AddComponentState[0].count = 2;
    AddComponentState[0].components = &TypedComponents[0];
    
    AddComponentState[1].entity = Base + 1;
    AddComponentState[1].count = 2;
    AddComponentState[1].components = &TypedComponents[1];
    
    TypedComponents[0].id = COMP_A;
    TypedComponents[0].data = &CompAData[0];
    TypedComponents[1].id = COMP_B;
    TypedComponents[1].data = CompBData;
    TypedComponents[2].id = COMP_A;
    TypedComponents[2].data = &CompAData[1];
    
    CompAData[0].v[0] = 1;
    CompAData[1].v[0] = 2;
    CompBData->v[0] = 10;
    CompBData->v[1] = 11;
    
    CallbackState[0].callback = MutationCallback;
    CallbackState[0].data = &TypedComponents[0].id;
    CallbackState[1].callback = MutationCallback;
    CallbackState[1].data = &TypedComponents[1].id;
    
    AddComponentState = ECSMutationStageEntityAddComponents(&Context, 1);
    ECSTypedComponent *TypedComponents2 = ECSMutationSetSharedData(&Context, sizeof(ECSTypedComponent));
    AddComponentState->entity = Base + 0;
    AddComponentState->count = 1;
    AddComponentState->components = TypedComponents2;
    TypedComponents2->id = COMP_C;
    TypedComponents2->data = NULL;
    
    ECSMutableRemoveComponentState *RemoveComponentState = ECSMutationStageEntityRemoveComponents(&Context, 1);
    RemoveComponentState->entity = Base + 0;
    RemoveComponentState->count = 1;
    ECSComponentID CompCID = COMP_C;
    RemoveComponentState->ids = &CompCID;
    
    
    XCTAssertEqual(ECSMutationInspectEntityCreate(&Context), 3, @"should retrieve the number of created entities");
    
    size_t Count;
    AddComponentState = ECSMutationInspectEntityAddComponents(&Context, &Count);
    XCTAssertEqual(Count, 3, @"should retrieve the number of add component states");
    XCTAssertEqual(AddComponentState[0].entity, Base + 0, @"should have the correct data");
    XCTAssertEqual(AddComponentState[1].entity, Base + 1, @"should have the correct data");
    XCTAssertEqual(AddComponentState[2].entity, Base + 0, @"should have the correct data");
    
    CallbackState = ECSMutationInspectCustomCallback(&Context, &Count);
    XCTAssertEqual(Count, 2, @"should retrieve the number of callback states");
    XCTAssertEqual(CallbackState[0].data, &TypedComponents[0].id, @"should have the correct data");
    XCTAssertEqual(CallbackState[1].data, &TypedComponents[1].id, @"should have the correct data");
    
    RemoveComponentState = ECSMutationInspectEntityRemoveComponents(&Context, &Count);
    XCTAssertEqual(Count, 1, @"should retrieve the number of remove component states");
    XCTAssertEqual(RemoveComponentState[0].entity, Base + 0, @"should have the correct data");
    XCTAssertEqual(RemoveComponentState[0].ids[0], COMP_C, @"should have the correct data");
    
    XCTAssertEqual(MutationCallbackEntityCount, 0, @"should not have called callback yet");
    XCTAssertEqual(MutationCallbackSum, 0, @"should not have called callback yet");
    
    ECSMutationApply(&Context);
    
    XCTAssertEqual(MutationCallbackEntityCount, 3, @"should have the correct data");
    XCTAssertTrue(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_A), @"should have the correct data");
    XCTAssertEqual(((CompA*)ECSEntityGetComponent(&Context, MutationCallbackEntityFirst, COMP_A))->v[0], 1, @"should have the correct data");
    XCTAssertTrue(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_B), @"should have the correct data");
    XCTAssertEqual(((CompB*)ECSEntityGetComponent(&Context, MutationCallbackEntityFirst, COMP_B))->v[0], 10, @"should have the correct data");
    XCTAssertEqual(MutationCallbackSum, 1 + 10 + 11, @"should have the correct data");
    
    AddComponentState = ECSMutationInspectEntityAddComponents(&Context, &Count);
    XCTAssertEqual(Count, 0, @"should no longer have anymore staged add component states");
    
    CallbackState = ECSMutationInspectCustomCallback(&Context, &Count);
    XCTAssertEqual(Count, 0, @"should no longer have anymore staged callback states");
    
    RemoveComponentState = ECSMutationInspectEntityRemoveComponents(&Context, &Count);
    XCTAssertEqual(Count, 0, @"should no longer have anymore staged remove component states");
    
    XCTAssertTrue(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_C), @"should not have removed the component that was added");
    
    
    RemoveComponentState = ECSMutationStageEntityRemoveComponents(&Context, 1);
    RemoveComponentState->entity = MutationCallbackEntityFirst;
    RemoveComponentState->count = 1;
    RemoveComponentState->ids = &CompCID;
    
    ECSMutationApply(&Context);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_C), @"should have removed the component that was previously added");
    XCTAssertTrue(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_A), @"should still have the remaining components");
    XCTAssertTrue(ECSEntityHasComponent(&Context, MutationCallbackEntityFirst, COMP_B), @"should still have the remaining components");
}

-(void) testTime
{
    ECSTime Time = ECS_TIME_FROM_HOURS(2) + ECS_TIME_FROM_MINUTES(90);
    uint64_t Hours = ECS_TIME_TO_HOURS(Time);
    uint64_t Minutes = ECS_TIME_TO_MINUTES(Time - ECS_TIME_FROM_HOURS(Hours));
    
    XCTAssertEqual(Hours, 3, @"Should extract the hours rounded down");
    XCTAssertEqual(Minutes, 30, @"Should extract the remaining minutes");
}

-(void) testMonitor
{
    ECSMonitor Monitor = ECSBinaryMonitorCreate(CC_STD_ALLOCATOR, COMP_C, 4, 3, sizeof(int));
    
    CompC C = { { 0, 1, 2 } };
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[1] = 10;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[0] = 5;
    C.v[2] = 20;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    ECSMonitorRecord(&Monitor, NULL);
    ECSMonitorRecord(&Monitor, NULL);
    
    C.v[0] = 100;
    C.v[1] = 200;
    C.v[2] = 300;
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[2] = 303;
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[1] = 202;
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[0] = 101;
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
#define TEST_COMP_C_TRANSFORM_VALUES_FALSE(...)
#define TEST_COMP_C_TRANSFORM_VALUES_TRUE(v0, v1, v2) \
XCTAssertEqual(OldC.v[0], v0, @"Should have the correct value"); \
XCTAssertEqual(OldC.v[1], v1, @"Should have the correct value"); \
XCTAssertEqual(OldC.v[2], v2, @"Should have the correct value");
    
#define TEST_COMP_C_TRANSFORM(r, exists, ...) \
{\
CompC OldC = C; \
_Bool Result = ECSMonitorTransform(&Monitor, &OldC, r); \
XCTAssertEqual(Result, exists, @"Should %@", exists ? @"exist" : @"be removed"); \
TEST_COMP_C_TRANSFORM_VALUES_##exists(__VA_ARGS__) \
}
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(1, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(2, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(3, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(4, FALSE);
    TEST_COMP_C_TRANSFORM(5, TRUE, 5, 10, 20);
    TEST_COMP_C_TRANSFORM(6, TRUE, 0, 10, 2);
    TEST_COMP_C_TRANSFORM(7, TRUE, 0, 1, 2);
    TEST_COMP_C_TRANSFORM(8, FALSE);
    TEST_COMP_C_TRANSFORM(9, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[0] = 1010;
    C.v[1] = 2020;
    C.v[2] = 3030;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(1, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(2, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(3, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(4, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(5, FALSE);
    TEST_COMP_C_TRANSFORM(6, TRUE, 5, 10, 20);
    TEST_COMP_C_TRANSFORM(7, TRUE, 0, 10, 2);
    TEST_COMP_C_TRANSFORM(8, TRUE, 0, 1, 2);
    TEST_COMP_C_TRANSFORM(9, FALSE);
    TEST_COMP_C_TRANSFORM(10, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[0] = 1011;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[1] = 2021;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(3, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(4, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(5, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(6, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(7, FALSE);
    TEST_COMP_C_TRANSFORM(8, TRUE, 5, 10, 20);
    TEST_COMP_C_TRANSFORM(9, TRUE, 0, 10, 2);
    TEST_COMP_C_TRANSFORM(10, TRUE, 0, 1, 2);
    TEST_COMP_C_TRANSFORM(11, FALSE);
    TEST_COMP_C_TRANSFORM(12, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[2] = 3031;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(4, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(5, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(6, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(7, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(8, FALSE);
    TEST_COMP_C_TRANSFORM(9, FALSE);
    TEST_COMP_C_TRANSFORM(10, FALSE);
    TEST_COMP_C_TRANSFORM(11, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[0] = 1012;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(5, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(6, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(7, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(8, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(9, FALSE);
    TEST_COMP_C_TRANSFORM(10, FALSE);
    TEST_COMP_C_TRANSFORM(11, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[1] = 2022;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1012, 2022, 3031);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(5, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(6, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(7, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(8, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(9, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(10, FALSE);
    TEST_COMP_C_TRANSFORM(11, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[2] = 3032;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1012, 2022, 3032);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1012, 2022, 3031);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(5, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(6, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(7, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(8, TRUE, 100, 202, 303);
    TEST_COMP_C_TRANSFORM(9, TRUE, 100, 200, 303);
    TEST_COMP_C_TRANSFORM(10, TRUE, 100, 200, 300);
    TEST_COMP_C_TRANSFORM(11, FALSE);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, FALSE);
    
    C.v[0] = 1013;
    
    ECSMonitorRecord(&Monitor, &C);
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1013, 2022, 3032);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1012, 2022, 3032);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1012, 2022, 3031);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(5, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(6, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(7, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(8, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(9, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(10, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(11, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, TRUE, 101, 202, 303);
    
    C.v[1] = 2023;
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[2] = 3033;
    ECSMonitorRecord(&Monitor, &C);
    
    C.v[0] = 1014;
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1014, 2023, 3033);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1013, 2023, 3033);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1013, 2023, 3032);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1013, 2022, 3032);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1012, 2022, 3032);
    TEST_COMP_C_TRANSFORM(5, TRUE, 1012, 2022, 3031);
    TEST_COMP_C_TRANSFORM(6, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(7, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(8, TRUE, 1011, 2021, 3030);
    TEST_COMP_C_TRANSFORM(9, TRUE, 1011, 2020, 3030);
    TEST_COMP_C_TRANSFORM(10, TRUE, 1010, 2020, 3030);
    TEST_COMP_C_TRANSFORM(11, TRUE, 101, 202, 303);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, TRUE, 101, 202, 303);
    
    C.v[1] = 2024;
    ECSMonitorRecord(&Monitor, &C);
    
    TEST_COMP_C_TRANSFORM(0, TRUE, 1014, 2024, 3033);
    TEST_COMP_C_TRANSFORM(1, TRUE, 1014, 2023, 3033);
    TEST_COMP_C_TRANSFORM(2, TRUE, 1013, 2023, 3033);
    TEST_COMP_C_TRANSFORM(3, TRUE, 1013, 2023, 3032);
    TEST_COMP_C_TRANSFORM(4, TRUE, 1013, 2022, 3032);
    TEST_COMP_C_TRANSFORM(5, TRUE, 1012, 2022, 3032);
    TEST_COMP_C_TRANSFORM(6, TRUE, 1012, 2022, 3031);
    TEST_COMP_C_TRANSFORM(7, TRUE, 1012, 2021, 3031);
    TEST_COMP_C_TRANSFORM(8, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(9, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(10, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(11, TRUE, 1011, 2021, 3031);
    TEST_COMP_C_TRANSFORM(SIZE_MAX, TRUE, 1011, 2021, 3031);
    
    ECSMonitorDestroy(&Monitor);
    
    
    
    Monitor = ECSBinaryMonitorCreate(CC_STD_ALLOCATOR, LOCAL_DUPLICATE_B, 4, 3, sizeof(int));
    
    CCArray DupB = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(LocalDuplicateB), 4);
    
    CCArrayAppendElement(DupB, &(LocalDuplicateB){
        { 1, 2 }
    });
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0] *= 10;
    CCArrayAppendElements(DupB, (LocalDuplicateB[3]){
        { { 3, 4 } },
        { { 5, 6 } },
        { { 7, 8 } },
    }, 3);
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0] *= 10;
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 3))->v[1] *= 10;
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
    CCArrayRemoveElementAtIndex(DupB, 2);
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
    ECSMonitorRecord(&Monitor, NULL);
    ECSMonitorRecord(&Monitor, NULL);
    
    CCArrayRemoveElementAtIndex(DupB, 2);
    CCArrayRemoveElementAtIndex(DupB, 1);
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
    CCArrayRemoveAllElements(DupB);
    
    ECSMonitorRecord(&Monitor, &DupB);
    ECSMonitorRecord(&Monitor, &DupB);
    
#define TEST_LOCAL_DUPLICATE_B_TRANSFORM_VALUES_FALSE(...)
#define TEST_LOCAL_DUPLICATE_B_TRANSFORM_VALUES_TRUE(x, c) \
XCTAssertEqual(CCArrayGetCount(OldDupB), c, @"Should have the number of elements"); \
if (CCArrayGetCount(OldDupB) == c) \
{ \
for (size_t Loop = 0; Loop < c; Loop++) \
{ \
XCTAssertEqual(((LocalDuplicateB*)CCArrayGetElementAtIndex(OldDupB, Loop))->v[0], x[Loop].v[0], @"Should have the correct value"); \
XCTAssertEqual(((LocalDuplicateB*)CCArrayGetElementAtIndex(OldDupB, Loop))->v[1], x[Loop].v[1], @"Should have the correct value"); \
} \
}
    
#define TEST_LOCAL_DUPLICATE_B_TRANSFORM(r, exists, ...) \
{\
CCArray OldDupB = NULL; \
if (CCArrayGetCount(DupB)) \
{ \
OldDupB = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(LocalDuplicateB), 4); \
CCArrayAppendElements(OldDupB, CCArrayGetData(DupB), CCArrayGetCount(DupB)); \
} \
_Bool Result = ECSMonitorTransform(&Monitor, &OldDupB, r); \
XCTAssertEqual(Result, exists, @"Should %@", exists ? @"exist" : @"be removed"); \
TEST_LOCAL_DUPLICATE_B_TRANSFORM_VALUES_##exists(__VA_ARGS__) \
if (OldDupB) CCArrayDestroy(OldDupB); \
}
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[4]){ { 100, 2 }, { 3, 4 }, { 5, 6 }, { 7, 80 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[4]){ { 10, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 1, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, FALSE);
    
    CCArrayAppendElements(DupB, (LocalDuplicateB[2]){
        { { 10, 20 } },
        { { 30, 40 } },
    }, 2);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    CCArrayInsertElementAtIndex(DupB, 1, &(LocalDuplicateB){ 50, 60 });
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[4]){ { 100, 2 }, { 3, 4 }, { 5, 6 }, { 7, 80 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[4]){ { 10, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 1, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, FALSE);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0] *= 10;
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[1] *= 10;
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 1))->v[0] *= 10;
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 2))->v[0] *= 10;
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[4]){ { 100, 2 }, { 3, 4 }, { 5, 6 }, { 7, 80 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[4]){ { 10, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 1, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, FALSE);
    
    CCArrayRemoveElementAtIndex(DupB, 0);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[4]){ { 100, 2 }, { 3, 4 }, { 5, 6 }, { 7, 80 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[4]){ { 10, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } }), 4);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[1]){ { 1, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, FALSE);
    
    CCArrayRemoveElementAtIndex(DupB, 1);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    
    CCArrayAppendElements(DupB, (LocalDuplicateB[2]){
        { { 12, 34 } },
        { { 56, 78 } },
    }, 2);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    
    CCArrayRemoveAllElements(DupB);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    
    ECSMonitorRecord(&Monitor, NULL);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    
    CCArrayAppendElements(DupB, (LocalDuplicateB[2]){
        { { 120, 340 } },
        { { 560, 780 } },
    }, 2);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[2]){ { 120, 340 }, { 560, 780 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 100, 2 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 100, 2 }, { 3, 4 }, { 7, 80 } }), 3);
    
    CCArrayRemoveElementAtIndex(DupB, 1);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[1]){ { 120, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[2]){ { 120, 340 }, { 560, 780 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0] *= 10;
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[1]){ { 1200, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 120, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[2]){ { 120, 340 }, { 560, 780 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, FALSE);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[3]){ { 500, 60 }, { 12, 34 }, { 56, 78 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 500, 60 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[2]){ { 500, 60 }, { 300, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[3]){ { 100, 200 }, { 500, 60 }, { 300, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[3]){ { 10, 20 }, { 50, 60 }, { 30, 40 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[2]){ { 10, 20 }, { 30, 40 } }), 2);
    
    for (size_t Loop = 0; Loop < 10; Loop++)
    {
        ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0]++;
        
        ECSMonitorRecord(&Monitor, &DupB);
    }
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[1]){ { 1210, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 1209, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 1208, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 1207, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[1]){ { 1206, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 1205, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 1204, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 1202, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 1201, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[1]){ { 1200, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[1]){ { 120, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[1]){ { 120, 340 } }), 1);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0]++;
    
    CCArrayAppendElements(DupB, (LocalDuplicateB[2]){
        { { 0, 1 } },
        { { 2, 3 } },
    }, 2);
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[3]){ { 1211, 340 }, { 0, 1 }, { 2, 3 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 1210, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 1209, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 1208, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[1]){ { 1207, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 1206, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 1205, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[1]){ { 1204, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[1]){ { 1203, 340 } }), 1);
    
    CCArrayRemoveElementsAtIndex(DupB, 1, 2);
    
    for (size_t Loop = 0; Loop < 10; Loop++)
    {
        ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0]++;
        
        ECSMonitorRecord(&Monitor, &DupB);
    }
    
    CCArrayAppendElements(DupB, (LocalDuplicateB[2]){
        { { 1, 2 } },
        { { 3, 4 } },
    }, 2);
    
    ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0]++;
    
    ECSMonitorRecord(&Monitor, &DupB);
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[3]){ { 1222, 340 }, { 1, 2 }, { 3, 4 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 1221, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 1220, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 1219, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[1]){ { 1218, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 1217, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 1216, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[1]){ { 1215, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 1214, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 1213, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[1]){ { 1212, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[3]){ { 1211, 340 }, { 0, 1 }, { 2, 3 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[3]){ { 1211, 340 }, { 0, 1 }, { 2, 3 } }), 3);
    
    CCArrayRemoveElementsAtIndex(DupB, 1, 2);
    
    for (size_t Loop = 0; Loop < 4; Loop++)
    {
        ((LocalDuplicateB*)CCArrayGetElementAtIndex(DupB, 0))->v[0]++;
        
        ECSMonitorRecord(&Monitor, &DupB);
    }
    
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(0, TRUE, ((LocalDuplicateB[1]){ { 1226, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(1, TRUE, ((LocalDuplicateB[1]){ { 1225, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(2, TRUE, ((LocalDuplicateB[1]){ { 1224, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(3, TRUE, ((LocalDuplicateB[1]){ { 1223, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(4, TRUE, ((LocalDuplicateB[3]){ { 1222, 340 }, { 1, 2 }, { 3, 4 } }), 3);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(5, TRUE, ((LocalDuplicateB[1]){ { 1221, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(6, TRUE, ((LocalDuplicateB[1]){ { 1220, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(7, TRUE, ((LocalDuplicateB[1]){ { 1219, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(8, TRUE, ((LocalDuplicateB[1]){ { 1218, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(9, TRUE, ((LocalDuplicateB[1]){ { 1217, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(10, TRUE, ((LocalDuplicateB[1]){ { 1216, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(11, TRUE, ((LocalDuplicateB[1]){ { 1215, 340 } }), 1);
    TEST_LOCAL_DUPLICATE_B_TRANSFORM(SIZE_MAX, TRUE, ((LocalDuplicateB[1]){ { 1215, 340 } }), 1);
    
    ECSMonitorDestroy(&Monitor);
}

-(void) testRegistry
{
    ECSEntity Entities[21];
    ECSEntityCreate(&Context, Entities, 21);
    
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[0]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[1]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[2]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[3]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[4]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[5]), NULL, @"Should not be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    
    
    ECSRegistryDeregister(&Context, Entities[0]);
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[1]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[0]), CC_BIG_INT_FAST_2), @"Should be registered");
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_2), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[1]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[2]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[3]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[5]), NULL, @"Should not be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entities[1], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    
    
    ECSRegistryDeregister(&Context, Entities[0]);
    
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[0]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[1]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[2]), NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[3]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[5]), NULL, @"Should not be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entities[1], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[0]), CC_BIG_INT_FAST_4), @"Should be registered");
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_4), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[1]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[2]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[5]), NULL, @"Should not be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entities[1], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), Entities[3], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    
    
    ECSRegistryReregister(&Context, Entities[0], CC_BIG_INT_FAST_2, FALSE);
    ECSRegistryReregister(&Context, Entities[5], CC_BIG_INT_FAST_8, FALSE);
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[2]), CC_BIG_INT_FAST_9), @"Should be registered");
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_2), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[1]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[2]), CC_BIG_INT_FAST_9), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[5]), CC_BIG_INT_FAST_8), @"Should be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entities[1], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), Entities[3], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_8), Entities[5], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_9), Entities[2], @"Should be registered");
    
    
    ECSRegistryReregister(&Context, Entities[0], CC_BIG_INT_FAST_0, TRUE);
        
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_0), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[1]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[2]), CC_BIG_INT_FAST_9), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[5]), CC_BIG_INT_FAST_8), @"Should be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), Entities[3], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_8), Entities[5], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_9), Entities[2], @"Should be registered");

    
    ECSRegistryReregister(&Context, Entities[0], CC_BIG_INT_FAST_NEGATIVE_3, FALSE);
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_NEGATIVE_3), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[1]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[2]), CC_BIG_INT_FAST_9), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[4]), CC_BIG_INT_FAST_1), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[5]), CC_BIG_INT_FAST_8), @"Should be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_NEGATIVE_3), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[4], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), Entities[3], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_8), Entities[5], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_9), Entities[2], @"Should be registered");
    
    
    ECSRegistryInit(&Context, CC_BIG_INT_FAST_14);
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[10]), CC_BIG_INT_FAST_14), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryRegister(&Context, Entities[11]), CC_BIG_INT_FAST_15), @"Should be registered");
    
    ECSRegistryReregister(&Context, Entities[20], CC_BIG_INT_FAST_1, TRUE);
    
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[0]), CC_BIG_INT_FAST_NEGATIVE_3), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[1]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[2]), CC_BIG_INT_FAST_9), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[3]), CC_BIG_INT_FAST_3), @"Should be registered");
    XCTAssertEqual(ECSRegistryGetID(&Context, Entities[4]), NULL, @"Should not be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[5]), CC_BIG_INT_FAST_8), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[10]), CC_BIG_INT_FAST_14), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[11]), CC_BIG_INT_FAST_15), @"Should be registered");
    XCTAssertTrue(CCBigIntFastCompareEqual(ECSRegistryGetID(&Context, Entities[20]), CC_BIG_INT_FAST_1), @"Should be registered");
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_NEGATIVE_3), Entities[0], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entities[20], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_3), Entities[3], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_4), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_5), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_8), Entities[5], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_9), Entities[2], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_14), Entities[10], @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_15), Entities[11], @"Should be registered");
    
    ECSEntityDestroy(&Context, Entities, 21);
    
    
    Context.registry.id = NULL;
    CCDictionaryDestroy(Context.registry.registeredEntities);
    Context.registry.registeredEntities = NULL;
    CCArrayDestroy(Context.registry.uniqueEntityIDs);
    Context.registry.uniqueEntityIDs = NULL;
    
    ECSRegistryInit(&Context, CC_BIG_INT_FAST_0);
    
    ECSEntity Entity;
    ECSEntityCreate(&Context, &Entity, 1);
    
    ECSProxyEntity *StagedEntities = ECSMutationStageRegistryRegister(&Context, 2);
    ECSProxyEntity NewEntity = ECSMutationStageEntityCreate(&Context, 2);
    
    StagedEntities[0] = NewEntity;
    StagedEntities[1] = Entity;
    
    ECSProxyEntity *StagedEntities2 = ECSMutationStageRegistryRegister(&Context, 1);
    
    *StagedEntities2 = NewEntity + 1;
    
    size_t Count;
    ECSProxyEntity *RegisteredEntities = ECSMutationInspectRegistryRegister(&Context, &Count);
    
    XCTAssertEqual(Count, 3, @"Should be the correct value");
    XCTAssertEqual(RegisteredEntities[0], NewEntity, @"Should be the correct entity");
    XCTAssertEqual(RegisteredEntities[1], Entity, @"Should be the correct entity");
    XCTAssertEqual(RegisteredEntities[2], NewEntity + 1, @"Should be the correct entity");
    
    ECSMutationApply(&Context);
    
    XCTAssertNotEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entity, @"Should be registered");
    XCTAssertNotEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should be registered");
    
    ECSEntity Entity0 = ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0);
    ECSEntity Entity1 = ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1);
    ECSEntity Entity2 = ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2);
    
    ECSEntity *DeregisterEntities = ECSMutationStageRegistryDeregister(&Context, 2);
    
    DeregisterEntities[0] = Entity0;
    DeregisterEntities[1] = Entity1;
    
    ECSEntity *DeregisterEntities2 = ECSMutationStageRegistryDeregister(&Context, 1);
    
    *DeregisterEntities2 = Entity2;
    
    ECSEntity *DeregisteredEntities = ECSMutationInspectRegistryDeregister(&Context, &Count);
    
    XCTAssertEqual(Count, 3, @"Should be the correct value");
    XCTAssertEqual(DeregisteredEntities[0], Entity0, @"Should be the correct entity");
    XCTAssertEqual(DeregisteredEntities[1], Entity1, @"Should be the correct entity");
    XCTAssertEqual(DeregisteredEntities[2], Entity2, @"Should be the correct entity");
    
    ECSMutationApply(&Context);
    
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), ECS_ENTITY_NULL, @"Should not be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), ECS_ENTITY_NULL, @"Should not be registered");
    
    ECSMutableReplaceRegistryState *State = ECSMutationStageRegistryReregister(&Context, 2);
    
    State[0].entity = Entity0;
    State[0].id = CC_BIG_INT_FAST_0;
    State[0].acquire = FALSE;
    
    State[1].entity = Entity1;
    State[1].id = CC_BIG_INT_FAST_1;
    State[1].acquire = FALSE;
    
    NewEntity = ECSMutationStageEntityCreate(&Context, 1);
    ECSMutableReplaceRegistryState *State2 = ECSMutationStageRegistryReregister(&Context, 2);
    
    State2[0].entity = Entity2;
    State2[0].id = CC_BIG_INT_FAST_2;
    State2[0].acquire = FALSE;
    
    State2[1].entity = NewEntity;
    State2[1].id = CC_BIG_INT_FAST_0;
    State2[1].acquire = TRUE;
    
    ECSMutableReplaceRegistryState *ReregisteredEntities = ECSMutationInspectRegistryReregister(&Context, &Count);
    
    XCTAssertEqual(Count, 4, @"Should be the correct value");
    XCTAssertEqual(ReregisteredEntities[0].entity, Entity0, @"Should be the correct entity");
    XCTAssertEqual(ReregisteredEntities[1].entity, Entity1, @"Should be the correct entity");
    XCTAssertEqual(ReregisteredEntities[2].entity, Entity2, @"Should be the correct entity");
    XCTAssertEqual(ReregisteredEntities[3].entity, NewEntity, @"Should be the correct entity");
    
    XCTAssertEqual(ReregisteredEntities[0].id, CC_BIG_INT_FAST_0, @"Should be the correct id");
    XCTAssertEqual(ReregisteredEntities[1].id, CC_BIG_INT_FAST_1, @"Should be the correct id");
    XCTAssertEqual(ReregisteredEntities[2].id, CC_BIG_INT_FAST_2, @"Should be the correct id");
    XCTAssertEqual(ReregisteredEntities[3].id, CC_BIG_INT_FAST_0, @"Should be the correct id");
    
    XCTAssertEqual(ReregisteredEntities[0].acquire, FALSE, @"Should be the correct value");
    XCTAssertEqual(ReregisteredEntities[1].acquire, FALSE, @"Should be the correct value");
    XCTAssertEqual(ReregisteredEntities[2].acquire, FALSE, @"Should be the correct value");
    XCTAssertEqual(ReregisteredEntities[3].acquire, TRUE, @"Should be the correct value");
    
    ECSMutationApply(&Context);
    
    XCTAssertNotEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), ECS_ENTITY_NULL, @"Should be registered");
    XCTAssertNotEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_0), Entity0, @"Should replace entity");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_1), Entity1, @"Should be registered");
    XCTAssertEqual(ECSRegistryLookup(&Context, CC_BIG_INT_FAST_2), Entity2, @"Should be registered");
}

static ECSEntity TestCallbackEntity = 0;
static int TestCallbackAddCount = 0;
static void TestLinkAddCallback(ECSContext *Context, ECSEntity Entity, void *Data)
{
    TestCallbackAddCount += Data ? *(int*)Data : 1;
    TestCallbackEntity |= (1 << Entity);
}

static int TestCallbackRemoveCount = 0;
static void TestLinkRemoveCallback(ECSContext *Context, ECSEntity Entity)
{
    TestCallbackRemoveCount++;
    TestCallbackEntity |= (1 << Entity);
}

static const ECSLink TestOneToOne = { .type = ECSLinkTypeRelationshipOneToOne };
static const ECSLink TestOneToMany = { .type = ECSLinkTypeRelationshipOneToMany };
static const ECSLink TestManyToMany = { .type = ECSLinkTypeRelationshipManyToMany };

static const CompA TestDefaultCompA_10 = { { 10 } };
static const CompA TestDefaultCompA_20 = { { 20 } };
static const CompB TestDefaultCompB_10_20 = { { 10, 20 } };

static const ECSLink TestOneToOneLeftCompA = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_10 }
    }
};
static const ECSLink TestOneToOneRightCompA = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate[1] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_20 }
    }
};
static const ECSLink TestOneToOneCompA = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft) | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate = {
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_10 } },
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_20 } }
    }
};
static const ECSLink TestOneToOneLeftCompB = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_B, .data = &TestDefaultCompB_10_20 }
    }
};
static const ECSLink TestOneToOneLeftCallback = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithLeft),
    .associate[0] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};
static const ECSLink TestOneToOneRightCallback = {
    .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithRight),
    .associate[1] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};

static const ECSLink TestOneToManyLeftCompA = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_10 }
    }
};
static const ECSLink TestOneToManyRightCompA = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate[1] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_20 }
    }
};
static const ECSLink TestOneToManyCompA = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft) | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate = {
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_10 } },
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_20 } }
    }
};
static const ECSLink TestOneToManyLeftCompB = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_B, .data = &TestDefaultCompB_10_20 }
    }
};
static const ECSLink TestOneToManyLeftCallback = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithLeft),
    .associate[0] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};
static const ECSLink TestOneToManyRightCallback = {
    .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithRight),
    .associate[1] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};

static const ECSLink TestManyToManyLeftCompA = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_10 }
    }
};
static const ECSLink TestManyToManyRightCompA = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate[1] = {
        .component = { .id = COMP_A, .data = &TestDefaultCompA_20 }
    }
};
static const ECSLink TestManyToManyCompA = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft) | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithRight),
    .associate = {
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_10 } },
        { .component = { .id = COMP_A, .data = &TestDefaultCompA_20 } }
    }
};
static const ECSLink TestManyToManyLeftCompB = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateComponent << ECSLinkTypeWithLeft),
    .associate[0] = {
        .component = { .id = COMP_B, .data = &TestDefaultCompB_10_20 }
    }
};
static const ECSLink TestManyToManyLeftCallback = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithLeft),
    .associate[0] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};
static const ECSLink TestManyToManyRightCallback = {
    .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeAssociateCallback << ECSLinkTypeWithRight),
    .associate[1] = {
        .callback = { .add = TestLinkAddCallback, .remove = TestLinkRemoveCallback }
    }
};

static const ECSLink TestOneToOneLeftCascade = { .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithLeft) };
static const ECSLink TestOneToOneRightCascade = { .type = ECSLinkTypeRelationshipOneToOne | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithRight) };
static const ECSLink TestOneToManyLeftCascade = { .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithLeft) };
static const ECSLink TestOneToManyRightCascade = { .type = ECSLinkTypeRelationshipOneToMany | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithRight) };
static const ECSLink TestManyToManyLeftCascade = { .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithLeft) };
static const ECSLink TestManyToManyRightCascade = { .type = ECSLinkTypeRelationshipManyToMany | (ECSLinkTypeDeletionCascading << ECSLinkTypeWithRight) };

-(void) assertEntitiesA: (ECSEntity*)entitiesA Count: (size_t)countA Linked: (const ECSLink*)link ToEntitiesB: (ECSEntity*)entitiesB Count: (size_t)countB
{
    size_t Count;
    const ECSEntity *LinkedEntities;
    
    for (size_t LoopA = 0; LoopA < countA; LoopA++)
    {
        for (size_t LoopB = 0; LoopB < countB; LoopB++)
        {
            if ((entitiesA[LoopA] != -1) && (entitiesB[LoopB] != -1))
            {
                XCTAssertTrue(ECSLinked(&Context, entitiesA[LoopA], link, entitiesB[LoopB]), @"Entities should be linked");
                XCTAssertTrue(ECSLinked(&Context, entitiesB[LoopB], ECS_LINK_INVERT(link), entitiesA[LoopA]), @"Entities should be linked");
            }
        }
    }
    
    for (size_t LoopA = 0; LoopA < countA; LoopA++)
    {
        if (entitiesA[LoopA] != -1)
        {
            LinkedEntities = ECSLinkGet(&Context, entitiesA[LoopA], link, &Count);
            XCTAssertEqual(Count, countB, @"Should have the correct number of linked entities");
            XCTAssertNotEqual(LinkedEntities, NULL, @"Should have linked entities");
            
            if (LinkedEntities)
            {
                for (size_t LoopB = 0, Check = CCMin(Count, countB); LoopB < Check; LoopB++)
                {
                    if (entitiesB[LoopB] != -1)
                    {
                        XCTAssertEqual(LinkedEntities[LoopB], entitiesB[LoopB], @"Should get the linked entity");
                    }
                }
            }
        }
    }
    
    for (size_t LoopB = 0; LoopB < countB; LoopB++)
    {
        if (entitiesB[LoopB] != -1)
        {
            LinkedEntities = ECSLinkGet(&Context, entitiesB[LoopB], ECS_LINK_INVERT(link), &Count);
            XCTAssertEqual(Count, countA, @"Should have the correct number of linked entities");
            XCTAssertNotEqual(LinkedEntities, NULL, @"Should have linked entities");
            
            if (LinkedEntities)
            {
                for (size_t LoopA = 0, Check = CCMin(Count, countA); LoopA < Check; LoopA++)
                {
                    if (entitiesA[LoopA] != -1)
                    {
                        XCTAssertEqual(LinkedEntities[LoopA], entitiesA[LoopA], @"Should get the linked entity");
                    }
                }
            }
        }
    }
}

-(void) assertEntityA: (ECSEntity)entityA Linked: (const ECSLink*)link ToEntitiesB: (ECSEntity*)entitiesB Count: (size_t)countB
{
    [self assertEntitiesA: &entityA Count: 1 Linked: link ToEntitiesB: entitiesB Count: countB];
}

-(void) assertEntityA: (ECSEntity)entityA Linked: (const ECSLink*)link ToEntityB: (ECSEntity)entityB
{
    [self assertEntitiesA: &entityA Count: 1 Linked: link ToEntitiesB: &entityB Count: 1];
}

-(void) assertEntityA: (ECSEntity)entityA WithCount: (size_t)countA NotLinked: (const ECSLink*)link ToEntityB: (ECSEntity)entityB WithCount: (size_t)countB
{
    size_t Count;
    const ECSEntity *LinkedEntities;
    
    XCTAssertFalse(ECSLinked(&Context, entityA, link, entityB), @"Entities should be linked");
    XCTAssertFalse(ECSLinked(&Context, entityB, ECS_LINK_INVERT(link), entityA), @"Entities should be linked");
    
    LinkedEntities = ECSLinkGet(&Context, entityA, link, &Count);
    if (countA)
    {
        XCTAssertEqual(Count, countA, @"Should have the correct number of linked entities");
        XCTAssertNotEqual(LinkedEntities, NULL, @"Should have linked entities");
    }
    
    else
    {
        XCTAssertEqual(Count, 0, @"Should not have any linked entities");
        XCTAssertEqual(LinkedEntities, NULL, @"Should not get any linked entity");
    }
    
    LinkedEntities = ECSLinkGet(&Context, entityB, ECS_LINK_INVERT(link), &Count);
    if (countB)
    {
        XCTAssertEqual(Count, countB, @"Should have the correct number of linked entities");
        XCTAssertNotEqual(LinkedEntities, NULL, @"Should have linked entities");
    }
    
    else
    {
        XCTAssertEqual(Count, 0, @"Should not have any linked entities");
        XCTAssertEqual(LinkedEntities, NULL, @"Should not get any linked entity");
    }
}

-(void) assertEntityA: (ECSEntity)entityA NotLinked: (const ECSLink*)link ToEntityB: (ECSEntity)entityB
{
    [self assertEntityA: entityA WithCount: 0 NotLinked: link ToEntityB: entityB WithCount: 0];
}

static int EntityCompare(const void *a, const void *b)
{
    if (*(ECSEntity*)a < *(ECSEntity*)b) return -1;
    else if (*(ECSEntity*)a > *(ECSEntity*)b) return 1;
    else return 0;
}

-(void) testLinks
{
    ECSEntity Entities[6];
    ECSEntityCreate(&Context, Entities, 6);
    
    qsort(Entities, 6, sizeof(ECSEntity), EntityCompare);
    
    XCTAssertFalse(ECSLinked(&Context, Entities[0], &TestOneToOne, Entities[1]), @"Entities should not be linked");
    
    size_t Count;
    const ECSEntity *LinkedEntities = ECSLinkGet(&Context, Entities[0], &TestOneToOne, &Count);
    
    XCTAssertEqual(Count, 0, @"Should have the correct number of linked entities");
    XCTAssertEqual(LinkedEntities, NULL, @"Should not get any linked entities");
    
    
    // Add link: 0 -> 0
    // Links: 0 -> 0
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> 1
    // Links: 0 -> 1
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Replace link: 0 -> 1 with: 0 -> 2
    // Links: 0 -> 2
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToOne ToEntityB: Entities[1] WithCount: 0];
    
    
    // Add link: 0 -> 1 (reversed)
    // Links: 0 -> 1
    ECSLinkAdd(&Context, Entities[1], NULL, ECS_LINK_INVERT(&TestOneToOne), Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToOne ToEntityB: Entities[2] WithCount: 0];
    
    
    // Add link: 1 -> 2
    // Links: 0 -> 1 -> 2
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[1] WithCount: 1 NotLinked: &TestOneToOne ToEntityB: Entities[0] WithCount: 0];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToOne ToEntityB: Entities[1] WithCount: 1];
    
    
    // Add link: 2 -> 0
    // Links: 0 -> 1 -> 2 -> 0
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 1 -> 0
    // Links: 0 -> 1 -> 0
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToOne ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 2 -> 0
    // Add link: 1 -> 2
    // Links: 0 -> 1 -> 2 -> 0
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[0], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Remove link: 1 -> 0
    // Links: 0 -> 1 -> 2 -> 0
    ECSLinkRemove(&Context, Entities[1], &TestOneToOne, Entities[0]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Remove link: 1 -> 2
    // Links: 0 -> 1,  2 -> 0
    ECSLinkRemove(&Context, Entities[1], &TestOneToOne, Entities[2]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 1 -> 2
    // Remove link: 0 -> ?
    // Links: 1 -> 2 -> 0
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOne);
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> 1
    // Remove link: ? -> 0
    // Links: 0 -> 1 -> 2
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], ECS_LINK_INVERT(&TestOneToOne));
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 2 -> 0
    // Remove all links: 2
    // Links: 0 -> 1
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[0], NULL);
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[2]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 1 -> 2
    // Add link: 2 -> 0
    // Remove link: ? -> ?
    // Links:
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[0], NULL);
    ECSLinkRemoveLink(&Context, &TestOneToOne);
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> 1
    // Add link: 1 -> 2
    // Add link: 2 -> 0
    // Remove link: ? -> ? (reversed)
    // Links:
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[0], NULL);
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToOne));
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[0]];
    
    
    
    
    // Add link: 0 -> [1]
    // Links: 0 -> [1]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[1], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> [2]
    // Links: 0 -> [1, 2]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2] } Count: 2];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> [5]
    // Links: 0 -> [1, 2, 5]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[5], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2], Entities[5] } Count: 3];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> [4]
    // Links: 0 -> [1, 2, 4, 5]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[4], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2], Entities[4], Entities[5] } Count: 4];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> [4]
    // Links: 0 -> [1, 2, 3, 4, 5]
    ECSLinkAdd(&Context, Entities[3], NULL, ECS_LINK_INVERT(&TestOneToMany), Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2], Entities[3], Entities[4], Entities[5] } Count: 5];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 0 -> [0]
    // Links: 0 -> [0, 1, 2, 3, 4, 5]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[1], Entities[2], Entities[3], Entities[4], Entities[5] } Count: 6];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[4] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[5] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Remove link: 0 -> [0]
    // Links: 0 -> [1, 2, 3, 4, 5]
    ECSLinkRemove(&Context, Entities[0], &TestOneToMany, Entities[0]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2], Entities[3], Entities[4], Entities[5] } Count: 5];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 1 -> [0]
    // Links: 0 -> [1 -> [0], 2, 3, 4, 5]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[0], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[2], Entities[3], Entities[4], Entities[5] } Count: 5];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[4] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[5] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 1 -> [2]
    // Links: 0 -> [1 -> [0, 2], 3, 4, 5]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3], Entities[4], Entities[5] } Count: 4];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2] } Count: 2];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[4] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[5] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 4 -> [5]
    // Add link: 1 -> [4]
    // Links: 0 -> [1 -> [0, 2, 4 -> [5]], 3]
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[5], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[4], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[5] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 5 -> [3]
    // Links: 0 -> [1 -> [0, 2, 4 -> [5 -> [3]]]]
    ECSLinkAdd(&Context, Entities[5], NULL, &TestOneToMany, Entities[3], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 5 -> [3]
    // Links: 0 -> [1 -> [0, 2, 4 -> [5 -> [3]]]]
    ECSLinkAdd(&Context, Entities[5], NULL, &TestOneToMany, Entities[3], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Add link: 5 -> [3] (reversed)
    // Links: 0 -> [1 -> [0, 2, 4 -> [5 -> [3]]]]
    ECSLinkAdd(&Context, Entities[3], NULL, ECS_LINK_INVERT(&TestOneToMany), Entities[5], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Remove link: 1 -> [0]
    // Links: 0 -> [1 -> [2, 4 -> [5 -> [3]]]]
    ECSLinkRemove(&Context, Entities[1], &TestOneToMany, Entities[0]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[2], Entities[4] } Count: 2];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Remove link: 2 -> [1]
    // Links: 0 -> [1 -> [2, 4 -> [5 -> [3]]]]
    ECSLinkRemove(&Context, Entities[2], &TestOneToMany, Entities[1]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[2], Entities[4] } Count: 2];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Remove link: 1 -> [2] (reversed)
    // Links: 0 -> [1 -> [4 -> [5 -> [3]]]]
    ECSLinkRemove(&Context, Entities[2], ECS_LINK_INVERT(&TestOneToMany), Entities[1]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[4] } Count: 1];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 1 -> [0]
    // Add link: 1 -> [2]
    // Links: 0 -> [1 -> [0, 2, 4 -> [5 -> [3]]]]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[0], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[2], NULL);
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOne);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Remove link: 0 -> ?
    // Links: 1 -> [0, 2, 4 -> [5 -> [3]]]
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToMany);
    
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[0], Entities[2], Entities[4] } Count: 3];
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[0] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[1] WithCount: 0];
    [self assertEntityA: Entities[2] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    [self assertEntityA: Entities[3] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[0] WithCount: 1];
    
    
    // Remove link: 1 -> ?
    // Links: 4 -> [5 -> [3]]
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], &TestOneToMany);
    
    [self assertEntityA: Entities[4] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[5] } Count: 1];
    [self assertEntityA: Entities[5] Linked: &TestOneToMany ToEntitiesB: (ECSEntity[]){ Entities[3] } Count: 1];
    [self assertEntityA: Entities[0] NotLinked: &TestOneToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[4]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[3] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    
    
    // Add link: 4 -> 0
    // Add link: 4 -> 2
    // Remove link: ? -> ?
    // Links:
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[0], NULL);
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[2], NULL);
    ECSLinkRemoveLink(&Context, &TestOneToMany);
    
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[5]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[3]];
    
    
    // Add link: 4 -> 0
    // Add link: 4 -> 2
    // Add link: 4 -> 5
    // Add link: 5 -> 3
    // Remove link: ? -> ?
    // Links:
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[0], NULL);
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[4], NULL, &TestOneToMany, Entities[5], NULL);
    ECSLinkAdd(&Context, Entities[5], NULL, &TestOneToMany, Entities[3], NULL);
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToMany));
    
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[4] NotLinked: &TestOneToMany ToEntityB: Entities[5]];
    [self assertEntityA: Entities[5] NotLinked: &TestOneToMany ToEntityB: Entities[3]];
    
    
    
    
    // Add link: [0] -> [1]
    // Links: [0] -> [1]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToMany, Entities[1], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestManyToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[0]];
    
    
    // Add link: [1] -> [1]
    // Links: [0, 1] -> [1]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[1], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Add link: [1] -> [1]
    // Links: [0, 1] -> [1]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[1], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntityA: Entities[1] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Add link: [1] -> [2]
    // Links: [0, 1] -> [1], [1] -> [2]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[2], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1] } Count: 1];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1 } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2] } Count: 2];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Add link: [1] -> [3]
    // Add link: [0] -> [3]
    // Links: [0, 1] -> [1, 3], [1] -> [2]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[3], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToMany, Entities[3], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1, Entities[3] } Count: 3];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2], -1 } Count: 3];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 3 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Add link: [1] -> [4] (reversed)
    // Links: [0, 1] -> [1, 3], [1] -> [2, 4]
    ECSLinkAdd(&Context, Entities[4], NULL, ECS_LINK_INVERT(&TestManyToMany), Entities[1], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1, Entities[3], -1 } Count: 4];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2], -1, Entities[4] } Count: 4];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[4] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 4 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Remove link: [1] -> [4]
    // Links: [0, 1] -> [1, 3], [1] -> [2]
    ECSLinkRemove(&Context, Entities[1], &TestManyToMany, Entities[4]);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1, Entities[3] } Count: 3];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2], -1 } Count: 3];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 3 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    [self assertEntityA: Entities[1] WithCount: 3 NotLinked: &TestManyToMany ToEntityB: Entities[4] WithCount: 0];
    
    
    // Remove link: [1] -> [3]
    // Links: [0, 1] -> [1], [0] -> [3], [1] -> [2]
    ECSLinkRemove(&Context, Entities[1], &TestManyToMany, Entities[3]);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1 } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ Entities[0] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1 } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2] } Count: 2];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    [self assertEntityA: Entities[1] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[3] WithCount: 1];
    
    
    // Add link: [1] -> [3]
    // Add link: [1] -> [4]
    // Links: [0, 1] -> [1, 3], [1] -> [2, 4]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[3], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[4], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1, Entities[3], -1 } Count: 4];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2], -1, Entities[4] } Count: 4];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[4] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 4 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Remove link: [1] -> ?
    // Links: [0] -> [1, 3]
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], &TestManyToMany);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 0];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[4] WithCount: 0];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestManyToMany ToEntityB: Entities[1] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 0];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestManyToMany ToEntityB: Entities[3] WithCount: 1];
    
    
    // Add link: [1] -> [3]
    // Add link: [1] -> [4]
    // Add link: [1] -> [2]
    // Add link: [1] -> [1]
    // Links: [0, 1] -> [1, 3], [1] -> [2, 4]
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[3], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[4], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[1], NULL);
    
    [self assertEntitiesA: (ECSEntity[]){ Entities[0], -1 } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], Entities[3] } Count: 2];
    [self assertEntitiesA: (ECSEntity[]){ -1, Entities[1] } Count: 2 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ Entities[1], -1, Entities[3], -1 } Count: 4];
    [self assertEntitiesA: (ECSEntity[]){ Entities[1] } Count: 1 Linked: &TestManyToMany ToEntitiesB: (ECSEntity[]){ -1, Entities[2], -1, Entities[4] } Count: 4];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[2] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 2 NotLinked: &TestManyToMany ToEntityB: Entities[4] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 4 NotLinked: &TestManyToMany ToEntityB: Entities[0] WithCount: 0];
    
    
    // Remove link: ? -> ?
    // Links:
    ECSLinkRemoveLink(&Context, &TestManyToMany);
    
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[4]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[0]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[4]];
    
    
    
    
    // Add link: 0 -> 1
    // Add link: 1 -> 2
    // Add link: 2 -> 1
    // Add link: 0 -> [1]
    // Add link: 0 -> [3]
    // Add link: 1 -> [2]
    // Add link: 2 -> [1]
    // Add link: [0] -> [1]
    // Add link: [0] -> [3]
    // Add link: [1] -> [2]
    // Add link: [2] -> [1]
    // Remove link: 1? -> 0?
    // Links: 1 -> 2, 2 -> 1, 0 -> [3], 1 -> [2], 2 -> [1], [0] -> [3], [1] -> [2], [2] -> [1]
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToOne, Entities[1], NULL);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToMany, Entities[3], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToMany, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestOneToMany, Entities[1], NULL);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToMany, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToMany, Entities[3], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToMany, Entities[2], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestManyToMany, Entities[1], NULL);
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[1] Linked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToMany ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] Linked: &TestManyToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[1] Linked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestManyToMany ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] WithCount: 0 NotLinked: &TestOneToOne ToEntityB: Entities[1] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToMany ToEntityB: Entities[1] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[1] WithCount: 1];
    
    
    // Remove link: 1 -> [?]
    // Links: 1 -> 2, 2 -> 1, 0 -> [3], 2 -> [1], [0] -> [3], [1] -> [2], [2] -> [1]
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], &TestOneToMany);
    
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestOneToOne ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[2] Linked: &TestOneToMany ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] Linked: &TestManyToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[1] Linked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] Linked: &TestManyToMany ToEntityB: Entities[1]];
    
    [self assertEntityA: Entities[0] WithCount: 0 NotLinked: &TestOneToOne ToEntityB: Entities[1] WithCount: 1];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToMany ToEntityB: Entities[1] WithCount: 1];
    [self assertEntityA: Entities[1] WithCount: 0 NotLinked: &TestOneToMany ToEntityB: Entities[2] WithCount: 0];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[1] WithCount: 1];
    
    
    // Remove link: 1? -> ?
    // Links: 0 -> [3], [0] -> [3]
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntityB: Entities[3]];
    
    [self assertEntityA: Entities[0] Linked: &TestManyToMany ToEntityB: Entities[3]];
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToMany ToEntityB: Entities[1] WithCount: 0];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestManyToMany ToEntityB: Entities[1] WithCount: 0];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestManyToMany ToEntityB: Entities[1]];
    
    
    // Remove link: [?] -> [?]
    // Links: 0 -> [3]
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    ECSLinkRemoveLink(&Context, &TestManyToMany);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToMany ToEntityB: Entities[3]];
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] WithCount: 1 NotLinked: &TestOneToMany ToEntityB: Entities[1] WithCount: 0];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestOneToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[1]];
    [self assertEntityA: Entities[0] NotLinked: &TestManyToMany ToEntityB: Entities[3]];
    [self assertEntityA: Entities[1] NotLinked: &TestManyToMany ToEntityB: Entities[2]];
    [self assertEntityA: Entities[2] NotLinked: &TestManyToMany ToEntityB: Entities[1]];
    
    ECSLinkRemoveLink(&Context, &TestOneToOne);
    
    ECSEntityDestroy(&Context, Entities, 6);
    
    
    ECSEntityCreate(&Context, Entities, 3);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    
    [self assertEntityA: Entities[0] Linked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    
    ECSEntityDestroy(&Context, Entities, 3);
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    
    
    ECSEntityCreate(&Context, Entities, 3);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOne, Entities[2], NULL);
    
    ECSEntityDestroy(&Context, Entities, 1);
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] Linked: &TestOneToOne ToEntityB: Entities[2]];
    
    
    ECSEntityCreate(&Context, Entities, 1);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOne, Entities[1], NULL);
    
    ECSEntityDestroy(&Context, &Entities[1], 1);
    
    [self assertEntityA: Entities[0] NotLinked: &TestOneToOne ToEntityB: Entities[1]];
    [self assertEntityA: Entities[1] NotLinked: &TestOneToOne ToEntityB: Entities[2]];
    
    
    ECSEntityCreate(&Context, &Entities[1], 1);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOneRightCompA, Entities[2], NULL);
    
    const CompA *A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Middle entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCompA);
    ECSLinkRemoveLink(&Context, &TestOneToOneRightCompA);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestOneToOneLeftCompA, Entities[1], &(CompA){ { 2 } });
    ECSLinkAdd(&Context, Entities[1], &(CompA){ { 1 } }, &TestOneToOneRightCompA, Entities[2], &(CompA){ { 2 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Middle entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCompA);
    ECSLinkRemoveLink(&Context, &TestOneToOneRightCompA);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToOneCompA);
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestOneToOneCompA, Entities[1], &(CompA){ { 2 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToOneCompA);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompB, Entities[1], NULL);
    
    const CompB *B = ECSEntityGetComponent(&Context, Entities[0], COMP_B);
    XCTAssertNotEqual(B, NULL, "Left entity should have component");
    XCTAssertEqual(B->v[0], 10, "Left entity should have component with correct value");
    XCTAssertEqual(B->v[1], 20, "Left entity should have component with correct value");
    B = ECSEntityGetComponent(&Context, Entities[1], COMP_B);
    XCTAssertEqual(B, NULL, "Right entity should not have component");
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCompB);
    ECSLinkAdd(&Context, Entities[0], &(CompB){ { 1, 2 } }, &TestOneToOneLeftCompB, Entities[1], NULL);
    
    B = ECSEntityGetComponent(&Context, Entities[0], COMP_B);
    XCTAssertNotEqual(B, NULL, "Left entity should have component");
    XCTAssertEqual(B->v[0], 1, "Left entity should have component with correct value");
    XCTAssertEqual(B->v[1], 2, "Left entity should have component with correct value");
    B = ECSEntityGetComponent(&Context, Entities[1], COMP_B);
    XCTAssertEqual(B, NULL, "Right entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 1, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Left entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[0], &(int){ 10 }, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 11, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    TestCallbackAddCount = 0;
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOneRightCallback, Entities[2], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 1, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Right entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneRightCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[2]), "Right entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToOneRightCallback, Entities[2], &(int){ 10 });
    
    XCTAssertEqual(TestCallbackAddCount, 11, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSEntityDestroy(&Context, Entities, 3);
    ECSEntityCreate(&Context, Entities, 3);
    
    ECSEntityAddComponent(&Context, Entities[1], NULL, COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToOneLeftCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneLeftCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToOneLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOneLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSEntityAddComponent(&Context, Entities[0], NULL, COMP_A);
    ECSEntityRemoveComponent(&Context, Entities[1], COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToOneRightCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneRightCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToOneRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOneRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToOneLeftCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneLeftCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToOneLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOneLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToOneRightCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneRightCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToOneRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToOneRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToOneRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToOneRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToOneRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    
    
    ECSEntityDestroy(&Context, Entities, 3);
    ECSEntityCreate(&Context, Entities, 3);
    
    TestCallbackAddCount = 0;
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Right entity should not have component");
    
    ECSLinkRemoveLink(&Context, &TestOneToManyLeftCompA);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Right entity should not have component");
    
    ECSLinkRemoveLink(&Context, &TestOneToManyLeftCompA);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[2], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Left entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToManyRightCompA);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], &(CompA){ { 2 } });
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[2], &(CompA){ { 3 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Left entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 3, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToManyRightCompA);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestOneToManyCompA);
    
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestOneToManyCompA, Entities[1], &(CompA){ { 2 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[2], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 2, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Left entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[0], &(int){ 10 }, &TestOneToManyLeftCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], &(int){ 100 }, &TestOneToManyLeftCallback, Entities[2], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 112, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    TestCallbackAddCount = 0;
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[2], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 2, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Right entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyRightCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToManyRightCallback, Entities[1], &(int){ 10 });
    ECSLinkAdd(&Context, Entities[1], NULL, &TestOneToManyRightCallback, Entities[2], &(int){ 100 });
    
    XCTAssertEqual(TestCallbackAddCount, 112, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Right entity should not have called the remove callback");
    
    ECSEntityDestroy(&Context, Entities, 3);
    ECSEntityCreate(&Context, Entities, 3);
    
    ECSEntityAddComponent(&Context, Entities[1], NULL, COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToManyLeftCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyLeftCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToManyLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToManyLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSEntityAddComponent(&Context, Entities[0], NULL, COMP_A);
    ECSEntityRemoveComponent(&Context, Entities[1], COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToManyRightCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyRightCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToManyRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToManyRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToManyLeftCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyLeftCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToManyLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestOneToManyRightCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyRightCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestOneToManyRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestOneToManyRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestOneToManyRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestOneToManyRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestOneToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    
    
    ECSEntityDestroy(&Context, Entities, 3);
    ECSEntityCreate(&Context, Entities, 3);
    
    TestCallbackAddCount = 0;
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Right entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestManyToManyLeftCompA);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestManyToManyLeftCompA, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[2], &(CompA){ { 2 } }, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Right entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 2, "Left entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestManyToManyLeftCompA);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[2], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Left entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestManyToManyRightCompA);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertEqual(A, NULL, "Entity should not have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], &(CompA){ { 2 } });
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[2], &(CompA){ { 3 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertEqual(A, NULL, "Left entity should not have component");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[2], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 3, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestManyToManyRightCompA);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyCompA, Entities[1], NULL);
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 10, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 20, "Right entity should have component with correct value");
    
    ECSLinkRemoveLink(&Context, &TestManyToManyCompA);
    
    ECSLinkAdd(&Context, Entities[0], &(CompA){ { 1 } }, &TestManyToManyCompA, Entities[1], &(CompA){ { 2 } });
    
    A = ECSEntityGetComponent(&Context, Entities[0], COMP_A);
    XCTAssertNotEqual(A, NULL, "Left entity should have component");
    XCTAssertEqual(A->v[0], 1, "Left entity should have component with correct value");
    A = ECSEntityGetComponent(&Context, Entities[1], COMP_A);
    XCTAssertNotEqual(A, NULL, "Right entity should have component");
    XCTAssertEqual(A->v[0], 2, "Right entity should have component with correct value");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[2], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 2, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]) | (1 << Entities[2]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Left entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]) | (1 << Entities[2]), "Left entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[0], &(int){ 10 }, &TestManyToManyLeftCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[2], &(int){ 100 }, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 112, "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]) | (1 << Entities[2]), "Left entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Left entity should not have called the remove callback");
    
    TestCallbackAddCount = 0;
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[2], NULL);
    
    XCTAssertEqual(TestCallbackAddCount, 2, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 0, "Right entity should not have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyRightCallback);
    
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the remove callback");
    
    TestCallbackEntity = 0;
    
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToManyRightCallback, Entities[1], &(int){ 10 });
    ECSLinkAdd(&Context, Entities[1], NULL, &TestManyToManyRightCallback, Entities[2], &(int){ 100 });
    
    XCTAssertEqual(TestCallbackAddCount, 112, "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]) | (1 << Entities[2]), "Right entity should have called the add callback");
    XCTAssertEqual(TestCallbackRemoveCount, 2, "Right entity should not have called the remove callback");
    
    ECSEntityDestroy(&Context, Entities, 3);
    ECSEntityCreate(&Context, Entities, 3);
    
    ECSEntityAddComponent(&Context, Entities[1], NULL, COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestManyToManyLeftCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyLeftCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestManyToManyLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestManyToManyLeftCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyLeftCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should have component");
    
    ECSEntityAddComponent(&Context, Entities[0], NULL, COMP_A);
    ECSEntityRemoveComponent(&Context, Entities[1], COMP_A);
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestManyToManyRightCompA, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyRightCompA), Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestManyToManyRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestManyToManyRightCompA);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyRightCompA));
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCompA, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertFalse(ECSEntityHasComponent(&Context, Entities[1], COMP_A), "Entity should not have component");
    XCTAssertTrue(ECSEntityHasComponent(&Context, Entities[0], COMP_A), "Entity should have component");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestManyToManyLeftCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyLeftCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestManyToManyLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestManyToManyLeftCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyLeftCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyLeftCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[0]), "Left entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Left entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[0], &TestManyToManyRightCallback, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemove(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyRightCallback), Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, &TestManyToManyRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLink(&Context, ECS_LINK_INVERT(&TestManyToManyRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[0], &TestManyToManyRightCallback);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveLinkForEntity(&Context, Entities[1], ECS_LINK_INVERT(&TestManyToManyRightCallback));
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksForEntity(&Context, Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[0], Entities[1]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
    
    ECSLinkAdd(&Context, Entities[0], NULL, &TestManyToManyRightCallback, Entities[1], NULL);
    
    TestCallbackEntity = 0;
    TestCallbackRemoveCount = 0;
    
    ECSLinkRemoveAllLinksBetweenEntities(&Context, Entities[1], Entities[0]);
    
    XCTAssertEqual(TestCallbackEntity, (1 << Entities[1]), "Right entity should have called the remove callback");
    XCTAssertEqual(TestCallbackRemoveCount, 1, "Right entity should not have called the remove callback");
}

@end
