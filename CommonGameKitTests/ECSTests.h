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

static void TestDestructor(void *Data, ECSComponentID ID);

static void MutationDestructor(void *Data, ECSComponentID ID);

#endif
