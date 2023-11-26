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

#ifndef CommonGameKit_ECSContext_h
#define CommonGameKit_ECSContext_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSConfig.h>
#include <CommonGameKit/ECSArchetype.h>
#include <CommonGameKit/ECSComponent.h>
#include <CommonGameKit/ECSRegistry.h>


#if ECS_ARCHETYPE_COMPONENT_MAX < UINT8_MAX
typedef uint8_t ECSArchetypeComponentID;

#if CC_HARDWARE_VECTOR_SUPPORT_SSSE3
#define ECS_ARCHETYPE_COMPONENT_ID_SIMD_LOOKUP 1

#define ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT 16
#define ECS_ARCHETYPE_COMPONENT_ID_COUNT(x) CC_ALIGN((x), ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT)
#define ECS_ARCHETYPE_COMPONENT_IDS_ALIGNAS _Alignas(ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT)
#endif

#else
typedef size_t ECSArchetypeComponentID;
#endif

#ifndef ECS_ARCHETYPE_COMPONENT_ID_COUNT
#define ECS_ARCHETYPE_COMPONENT_ID_COUNT(x) (x)
#endif

#ifndef ECS_ARCHETYPE_COMPONENT_IDS_ALIGNAS
#define ECS_ARCHETYPE_COMPONENT_IDS_ALIGNAS
#endif

#ifndef ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT
#define ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT 1
#endif

typedef struct {
    struct {
        ECSArchetype *ptr;
        size_t index;
        struct {
            ECS_ARCHETYPE_COMPONENT_IDS_ALIGNAS ECSArchetypeComponentID ids[ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX)];
            size_t count;
        } component;
    } archetype;
    
    CCBits(uint8_t, ECS_COMPONENT_MAX) has;
    
    struct {
        size_t indexes[ECS_PACKED_COMPONENT_MAX];
    } packed;
    
    uint8_t local[];
} ECSComponentRefs;

_Static_assert((offsetof(ECSComponentRefs, archetype.component.ids) % ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT) == 0, "Needs to be correctly aligned");

typedef struct {
    CCArray(ECSComponentRefs) map;
    CCArray(size_t) available;
} ECSEntityManager;

typedef struct ECSContext {
    ECSRegistry registry;
    struct ECSMutableState *mutations;
    ECSEntityManager manager;
    
#define ECS_ARCHETYPE_MEMBER(x, index) ECSArchetype(index) archetypes##index[ECS_COMPONENT_ARCHETYPE##index##_MAX]
#define ECS_ARCHETYPE_DECLARE_MEMBERS(count) CC_SOFT_JOIN(;, CC_REPEAT(1, count, ECS_ARCHETYPE_MEMBER))
    ECS_ARCHETYPE_DECLARE_MEMBERS(ECS_ARCHETYPE_MAX);
    ECSPackedComponent packed[ECS_PACKED_COMPONENT_MAX];
    ECSIndexedComponent indexed[ECS_INDEXED_COMPONENT_MAX];
} ECSContext;

#endif
