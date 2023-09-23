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

//ecs_tool setup -i CommonGameKitTests/ECSTests.h -a CommonGameKitTests/ECSTestAccessors.h --max-local 64 CommonGameKitTests/ECSTestData.h

#import <XCTest/XCTest.h>
#import "ECS.h"
#import "ECSTestAccessors.h"
#import "ECSTests.h"
#import "ECSTestData.h"


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
    ECSMutableStateAddComponentMax = 4069;
    ECSMutableStateRemoveComponentMax = 4069;
    ECSMutableStateCustomCallbackMax = 4096;
    ECSMutableStateSharedDataMax = 1048576;
    
    ECSInit();
    ECSWorkerCreate();
    ECSWorkerCreate();
    ECSWorkerCreate();
    ECSWorkerCreate();
}

static ECSContext Context;

ECSMutableState MutableState = ECS_MUTABLE_STATE_CREATE(4096, 4096, 4096, 4096, 1048576);

-(void) checkRun
{
    XCTAssertFalse(RunCheckError, @"should not execute systems with conflicting read/write access at the same time");
    
    RunCheckError = FALSE;
}

-(void) testExample
{
    Context.mutations = &MutableState;
    
    Context.manager.map = CCArrayCreate(CC_ALIGNED_ALLOCATOR(ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), CC_ALIGN(sizeof(ECSComponentRefs) + LOCAL_STORAGE_SIZE, ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), 16);
    Context.manager.available = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(size_t), 16);
    
    const size_t GroupCount = sizeof(Groups) / sizeof(*Groups);
    ECSExecutionGroup State[GroupCount];
    uint8_t ExecState[GroupCount][10];
    
    memset(ExecState, 0, sizeof(ExecState));
    
    ECSEntity TestEntity;
    ECSEntityCreate(&Context, &TestEntity, 1);
    ECSEntityAddComponent(&Context, TestEntity, NULL, CHECK_RUN_STATE_TAG);
    
    for (size_t Loop = 0; Loop < GroupCount; Loop++)
    {
        State[Loop] = (ECSExecutionGroup){
            .executing = 0,
            .state = ExecState[Loop],
            .time = 0,
            .running = 0
        };
    }
    
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

@end