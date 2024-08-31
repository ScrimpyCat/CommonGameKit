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

#ifndef ECSTests_h
#define ECSTests_h

typedef struct {
    int v[1];
} CompA;

typedef struct {
    int v[2];
} CompB;

typedef struct {
    int v[3];
} CompC;

typedef struct {
    int v[4];
} CompD;

typedef struct {
    int v[5];
} CompE;

typedef struct {
    int v[6];
} CompF;

typedef struct {
    int v[7];
} CompG;

typedef struct {
    int v[8];
} CompH;

typedef struct {
    int v[9];
} CompI;

typedef struct {
    int v[10];
} CompJ;

typedef void ArchTag;
typedef void PackedTag;
typedef void IndexedTag;
typedef void LocalTag;
typedef void DestroyMeTag;
typedef void CheckRunStateTag;

typedef CompA DuplicateA;
typedef CompB LocalDuplicateB;

typedef CompH LocalH;

typedef CompH ArchH;
typedef CompH PackedH;
typedef CompH IndexedH;

typedef int ArchK;
typedef int PackedK;
typedef int IndexedK;
typedef int LocalK;

ECS_ARCHETYPE_COMPONENT(CompA, 0);
ECS_ARCHETYPE_COMPONENT(CompB, 1);
ECS_ARCHETYPE_COMPONENT(CompC, 2);
ECS_ARCHETYPE_COMPONENT(CompD, 3);
ECS_ARCHETYPE_COMPONENT(CompE, 4);
ECS_PACKED_COMPONENT(CompF, 0);
ECS_PACKED_COMPONENT(CompG, 10);
ECS_PACKED_COMPONENT(CompJ, 2);
ECS_INDEXED_COMPONENT(CompH, 0);
ECS_INDEXED_COMPONENT(CompI, 1);
ECS_ARCHETYPE_TAG(ArchTag);
ECS_PACKED_TAG(PackedTag);
ECS_INDEXED_TAG(IndexedTag);
ECS_LOCAL_TAG(LocalTag);
ECS_PACKED_DUPLICATE_COMPONENT(DuplicateA);
ECS_LOCAL_DUPLICATE_COMPONENT(LocalDuplicateB);
ECS_LOCAL_COMPONENT(LocalH);
ECS_PACKED_TAG(DestroyMeTag);
ECS_PACKED_TAG(CheckRunStateTag);

ECS_ARCHETYPE_COMPONENT(ArchH);
ECS_PACKED_COMPONENT(PackedH);
ECS_INDEXED_COMPONENT(IndexedH);

ECS_ARCHETYPE_COMPONENT(ArchK);
ECS_PACKED_COMPONENT(PackedK);
ECS_INDEXED_COMPONENT(IndexedK);
ECS_LOCAL_COMPONENT(LocalK);

ECS_DESTRUCTOR(TrackDestructor, ArchK, PackedK, IndexedK, LocalK);

ECS_DESTRUCTOR(TestDestructor, DuplicateA, LocalH, ArchH, PackedH, IndexedH);

ECS_DESTRUCTOR(MutationDestructor, DestroyMeTag);

static ECS_SYSTEM(Sys1ReadA_WriteB, (CompA), (CompB));
static ECS_SYSTEM(Sys2ReadAC_WriteB, (CompA, CompC), (CompB));
static ECS_SYSTEM(Sys3ReadAC_WriteD, (CompA, CompC), (CompD));
static ECS_SYSTEM(Sys4ReadA, (CompA), ());
static ECS_SYSTEM(Sys5ReadC, (CompC), ());
static ECS_SYSTEM(Sys6ReadAC, (CompA, CompC), ());
static ECS_SYSTEM(Sys7WriteB, (), (CompB));
static ECS_PARALLEL_SYSTEM(Sys8ReadD_WriteC, (CompD), (CompC));
static ECS_SYSTEM(Sys9ReadFGH_WriteAI, (CompF, CompG, CompH), (CompA, CompI));
static ECS_PARALLEL_SYSTEM(Sys10WriteJ, (), (CompJ), (CompJ, SIZE_MAX));
static ECS_SYSTEM(Sys11ReadAWithArchTag, (CompA, ArchTag), ());
static ECS_SYSTEM(Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA, (LocalH, LocalDuplicateB), (DuplicateA));
static ECS_SYSTEM(Sys13ReadDestroyMeTag, (DestroyMeTag), ());
static ECS_PARALLEL_SYSTEM(Sys14ReadAJ, (CompA, CompJ), (), (CompJ, SIZE_MAX));
static ECS_PARALLEL_SYSTEM(Sys15ReadH, (CompH), (), (CompH, SIZE_MAX));
static ECS_SYSTEM(Sys16ReadCheckRunStateTag, (CheckRunStateTag), ());

ECS_SYSTEM_GROUP(MISC_GROUP, ECS_TIME_FROM_SECONDS(1.0 / 60.0), FALSE,
    PRIORITY(0, (Sys1ReadA_WriteB, Sys2ReadAC_WriteB, Sys3ReadAC_WriteD, Sys4ReadA, Sys5ReadC, Sys6ReadAC, Sys7WriteB, Sys13ReadDestroyMeTag, Sys14ReadAJ)),
    PRIORITY(1, (Sys8ReadD_WriteC, Sys10WriteJ, Sys11ReadAWithArchTag, Sys12ReadLocalHLocalDuplicateB_WriteDuplicateA, Sys15ReadH))
);

ECS_SYSTEM_GROUP(OTHER_GROUP, ECS_TIME_FROM_SECONDS(1.0 / 60.0) * 2, FALSE,
    PRIORITY(0, (Sys9ReadFGH_WriteAI))
);

ECS_SYSTEM_GROUP(TEST_GROUP, ECS_TIME_FROM_SECONDS(1.0 / 60.0), FALSE,
    PRIORITY(0, (), (MISC_GROUP, 1)),
    PRIORITY(1, (), (OTHER_GROUP, 0)),
    PRIORITY(2, (Sys16ReadCheckRunStateTag)),
    PRIORITY(3, (ECSMonitorSystem))
);

static void TrackDestructor(void *Data, ECSComponentID ID);

static void TestDestructor(void *Data, ECSComponentID ID);

static void MutationDestructor(void *Data, ECSComponentID ID);

#define CC_TYPE_CompA(...) CompA
#define CC_TYPE_0_CompA CC_TYPE_CompA,

#define CC_TYPE_CompB(...) CompB
#define CC_TYPE_0_CompB CC_TYPE_CompB,

#define CC_TYPE_CompC(...) CompC
#define CC_TYPE_0_CompC CC_TYPE_CompC,

#define CC_TYPE_CompD(...) CompD
#define CC_TYPE_0_CompD CC_TYPE_CompD,

#define CC_TYPE_CompE(...) CompE
#define CC_TYPE_0_CompE CC_TYPE_CompE,

#define CC_TYPE_CompF(...) CompF
#define CC_TYPE_0_CompF CC_TYPE_CompF,

#define CC_TYPE_CompG(...) CompG
#define CC_TYPE_0_CompG CC_TYPE_CompG,

#define CC_TYPE_CompH(...) CompH
#define CC_TYPE_0_CompH CC_TYPE_CompH,

#define CC_TYPE_CompI(...) CompI
#define CC_TYPE_0_CompI CC_TYPE_CompI,

#define CC_TYPE_CompJ(...) CompJ
#define CC_TYPE_0_CompJ CC_TYPE_CompJ,

#define CC_TYPE_ArchTag(...) void
#define CC_TYPE_0_ArchTag CC_TYPE_ArchTag,

#define CC_TYPE_PackedTag(...) void
#define CC_TYPE_0_PackedTag CC_TYPE_PackedTag,

#define CC_TYPE_IndexedTag(...) void
#define CC_TYPE_0_IndexedTag CC_TYPE_IndexedTag,

#define CC_TYPE_LocalTag(...) void
#define CC_TYPE_0_LocalTag CC_TYPE_LocalTag,

#define CC_TYPE_DuplicateA(...) CompA
#define CC_TYPE_0_DuplicateA CC_TYPE_DuplicateA,

#define CC_TYPE_LocalDuplicateB(...) CompB
#define CC_TYPE_0_LocalDuplicateB CC_TYPE_LocalDuplicateB,

#define CC_TYPE_LocalH(...) CompH
#define CC_TYPE_0_LocalH CC_TYPE_LocalH,

#define CC_TYPE_DestroyMeTag(...) void
#define CC_TYPE_0_DestroyMeTag CC_TYPE_DestroyMeTag,

#define CC_TYPE_CheckRunStateTag(...) void
#define CC_TYPE_0_CheckRunStateTag CC_TYPE_CheckRunStateTag,

#define CC_TYPE_ArchH(...) CompH
#define CC_TYPE_0_ArchH CC_TYPE_ArchH,

#define CC_TYPE_PackedH(...) CompH
#define CC_TYPE_0_PackedH CC_TYPE_PackedH,

#define CC_TYPE_IndexedH(...) CompH
#define CC_TYPE_0_IndexedH CC_TYPE_IndexedH,

#define CC_TYPE_ArchK(...) int
#define CC_TYPE_0_ArchK CC_TYPE_ArchK,

#define CC_TYPE_PackedK(...) int
#define CC_TYPE_0_PackedK CC_TYPE_PackedK,

#define CC_TYPE_IndexedK(...) int
#define CC_TYPE_0_IndexedK CC_TYPE_IndexedK,

#define CC_TYPE_LocalK(...) int
#define CC_TYPE_0_LocalK CC_TYPE_LocalK,

const CCReflectStruct1 CC_REFLECT(CompA);
const CCReflectStruct1 CC_REFLECT(CompB);
const CCReflectStruct1 CC_REFLECT(CompC);
const CCReflectStruct1 CC_REFLECT(CompD);
const CCReflectStruct1 CC_REFLECT(CompE);
const CCReflectStruct1 CC_REFLECT(CompF);
const CCReflectStruct1 CC_REFLECT(CompG);
const CCReflectStruct1 CC_REFLECT(CompH);
const CCReflectStruct1 CC_REFLECT(CompI);
const CCReflectStruct1 CC_REFLECT(CompJ);

#endif
