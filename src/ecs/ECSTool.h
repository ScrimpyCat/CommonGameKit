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

#ifndef CommonGameKit_ECSTool_h
#define CommonGameKit_ECSTool_h

//TODO: document
#define ECS_LOCAL_COMPONENT(...)
#define ECS_PACKED_COMPONENT(...)
#define ECS_INDEXED_COMPONENT(...)
#define ECS_ARCHETYPE_COMPONENT(...)
#define ECS_LOCAL_DUPLICATE_COMPONENT(...)
#define ECS_PACKED_DUPLICATE_COMPONENT(...)
#define ECS_INDEXED_DUPLICATE_COMPONENT(...)
#define ECS_ARCHETYPE_DUPLICATE_COMPONENT(...)
#define ECS_LOCAL_TAG(...)
#define ECS_PACKED_TAG(...)
#define ECS_INDEXED_TAG(...)
#define ECS_ARCHETYPE_TAG(...)

#define ECS_DESTRUCTOR(destructor, ...)

#define ECS_SYSTEM(system, ...) void system(ECSContext *ECS_CONTEXT_VAR, ECSArchetype *ECS_ARCHETYPE_VAR, const size_t *ECS_ARCHETYPE_COMPONENT_INDEXES_VAR, const size_t *ECS_COMPONENT_OFFSETS_VAR, void *ECS_CHANGES_VAR, ECSTime ECS_TIME_VAR)
#define ECS_PARALLEL_SYSTEM(...) ECS_SYSTEM(__VA_ARGS__)

#define ECS_SYSTEM_GROUP(...)

#define ECS_COMPONENT_ACCESS(system, component) ECS_COMPONENT_ACCESS_(system##_##component)
#define ECS_COMPONENT_ACCESS_(...) ECS_COMPONENT_ACCESS__(__VA_ARGS__)
#define ECS_COMPONENT_ACCESS__(components, _) components

#define ECS_ENTITY_ACCESS(system, component) ECS_ENTITY_ACCESS_(system##_##component)
#define ECS_ENTITY_ACCESS_(...) ECS_ENTITY_ACCESS__(__VA_ARGS__)
#define ECS_ENTITY_ACCESS__(_, entities) entities

#define ECS_GET(component) ECS_GET_(ECS_SYSTEM_NAME, component)
#define ECS_GET_(system, component) ECS_COMPONENT_ACCESS(system, component)

#define ECS_ENTITIES(component) ECS_ENTITIES_(ECS_SYSTEM_NAME, component)
#define ECS_ENTITIES_(system, component) ECS_ENTITY_ACCESS(system, component)

#define ECS_CONTEXT_VAR Context
#define ECS_ARCHETYPE_VAR Archetype
#define ECS_ARCHETYPE_COMPONENT_INDEXES_VAR ArchetypeComponentIndexes
#define ECS_COMPONENT_OFFSETS_VAR ComponentOffsets
#define ECS_CHANGES_VAR Changes
#define ECS_TIME_VAR Time

#define ECS_SYSTEM_FUN(...) ECS_SYSTEM(ECS_SYSTEM_NAME)

#define ECS_LOCAL_INDEX_MASK  ECS_MASK_FOR_VALUE(ECS_LOCAL_COMPONENT_MAX)

#define ECS_MASK_FOR_VALUE(x) ((x) & 0x80000000? UINT32_MAX : ECS_MASK_FOR_LOWER_POWER_OF_2(ECS_MASK_HIGHEST_BIT(x) << 1))
#define ECS_MASK_FOR_LOWER_POWER_OF_2(x) ((x)? ~-(x) : 0)
#define ECS_MASK_HIGHEST_BIT(x) ECS_MASK_HIGHEST_BIT_0(x)
#define ECS_MASK_HIGHEST_BIT_0(x) ECS_MASK_HIGHEST_BIT_1((x) | (x) >> 1)
#define ECS_MASK_HIGHEST_BIT_1(x) ECS_MASK_HIGHEST_BIT_2((x) | (x) >> 2)
#define ECS_MASK_HIGHEST_BIT_2(x) ECS_MASK_HIGHEST_BIT_3((x) | (x) >> 4)
#define ECS_MASK_HIGHEST_BIT_3(x) ECS_MASK_HIGHEST_BIT_4((x) | (x) >> 8)
#define ECS_MASK_HIGHEST_BIT_4(x) ECS_MASK_HIGHEST_BIT_5((x) | (x) >> 16)
#define ECS_MASK_HIGHEST_BIT_5(x) ((x) ^ ((x) >> 1))

#define ECS_COMPONENT_BASE_INDEX(id) ECS_COMPONENT_BASE_INDEX_0((id) & ECSComponentStorageTypeMask, (id) & ~ECSComponentStorageMask, SIZE_MAX, 0)
#define ECS_COMPONENT_BASE_INDEX_0(type, index, next, offset) ECS_COMPONENT_BASE_INDEX_1(type, index, (type) == ECSComponentStorageTypeArchetype ? ((index) + (offset)) : (next), (offset) + ECS_ARCHETYPE_COMPONENT_MAX)
#define ECS_COMPONENT_BASE_INDEX_1(type, index, next, offset) ECS_COMPONENT_BASE_INDEX_2(type, index, (type) == ECSComponentStorageTypePacked ? ((index) + (offset)) : (next), (offset) + ECS_PACKED_COMPONENT_MAX)
#define ECS_COMPONENT_BASE_INDEX_2(type, index, next, offset) ECS_COMPONENT_BASE_INDEX_3(type, index, (type) == ECSComponentStorageTypeIndexed ? ((index) + (offset)) : (next), (offset) + ECS_INDEXED_COMPONENT_MAX)
#define ECS_COMPONENT_BASE_INDEX_3(type, index, next, offset) ((type) == ECSComponentStorageTypeLocal ? (((index) & ECS_LOCAL_INDEX_MASK) + (offset)) : (next))

#pragma mark - Iterator

#define ECS_ITER_CONSUME(...) __VA_ARGS__
#define ECS_ITER_IGNORE(...)
#define ECS_ITER_POP(x, ...) x
#define ECS_ITER_PREPEND(suffix, prefix) CC_CAT(prefix, suffix)

#define ECS_ITER_TYPE(x) ECS_ITER_TYPE_(x)
#define ECS_ITER_TYPE_(x) ECS_ITER_TYPE_##x )
#define ECS_ITER_TYPE_0(x) ECS_ITER_TYPE_##x )
#define ECS_ITER_TYPE_const ECS_ITER_TYPE_0(

#define ECS_ITER_KIND(type) ECS_ITER_KIND_(type)
#define ECS_ITER_KIND_(type) ECS_ITER_KIND_##type

#define ECS_ITER_ARCHETYPE_PRE_INIT(type, i) *ECS_ITER_PRIVATE__Ptr##i = CCArrayGetData(ECS_GET(type))
#define ECS_ITER_PACKED_PRE_INIT(type, i) ECS_ITER_ARCHETYPE_PRE_INIT(type, i)

#define ECS_ITER_ARCHETYPE_FETCH(type, i) &((type*)ECS_ITER_PRIVATE__Ptr##i)[ECS_ITER_INDEX], ECS_ITER_IGNORE
#define ECS_ITER_PACKED_FETCH(type, i) CC_GET(i, ECS_ITER_ARCHETYPE_FETCH, CC_REPEAT(0, 20, ECS_ITER_POP, ECS_ITER_FALLBACK_FETCH))(type, i)
#define ECS_ITER_FALLBACK_FETCH(type, i) ECSEntityGetComponent(ECS_CONTEXT_VAR, ECS_ITER_ENTITY, ECS_ID_##type), ECS_ITER_CONSUME
#define ECS_ITER_WARNING_FETCH(type, i) CC_WARNING("Unsafe iteration of archetype component: " #type) ECS_ITER_FALLBACK_FETCH(type, i)

#define ECS_ITER_INIT(type) ECS_ITER_INIT_(type)
#define ECS_ITER_INIT_(type) ECS_ITER_INIT__(type, ECS_ITER_KIND(type))
#define ECS_ITER_INIT__(type, kind) ECS_ITER_INIT___(type, kind)
#define ECS_ITER_INIT___(type, kind) ECS_ITER_INIT_##kind(type)

#define ECS_ITER_INIT_X(type) ECS_ENTITIES(type), ,                                       (() ECS_ITER_IGNORE,              () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_FALLBACK_FETCH,   ECS_ITER_FALLBACK_FETCH,   ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)

// archetype
#define ECS_ITER_INIT_0(type) ECS_ENTITIES(type), void,                                   ((,) ECS_ITER_ARCHETYPE_PRE_INIT, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_ARCHETYPE_FETCH, ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)
// packed
#define ECS_ITER_INIT_1(type) ECS_ENTITIES(type), void ECS_ITER_PACKED_PRE_INIT(type, 0), (() ECS_ITER_IGNORE,              () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_WARNING_FETCH,   ECS_ITER_PACKED_FETCH,   ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)
// indexed
#define ECS_ITER_INIT_2(type) CC_ERROR("Cannot iterate with leading indexed component: " #type) ECS_ITER_INIT_X(type)
// local
#define ECS_ITER_INIT_3(type) CC_ERROR("Cannot iterate with leading local component: " #type) ECS_ITER_INIT_X(type)

#define ECS_ITER_DECLARE_ASSIGN(e, i, v) e = v

#define ECS_ITER_DECLARE_ARRAY_VAR(e, i, v) CCArray *ECS_ITER_PRIVATE__fetch_array##i = v, CC_CAT(e, ECS_ITER_DUPLICATE_ARRAY_SUFFIX) = ECS_ITER_PRIVATE__fetch_array##i

#define ECS_ITER_DECLARE_VAR(x, i, v) ECS_ITER_DECLARE_##x, i, v)
#define ECS_ITER_DECLARE_VAR_0(x, i, v) ECS_ITER_DECLARE_##x, i, v)
#define ECS_ITER_DECLARE_const const ECS_ITER_DECLARE_VAR_0(

#define ECS_ITER_PRE(e, i, init) ECS_ITER_PRE_(ECS_ITER_TYPE(e), e, i, ECS_ITER_CONSUME init)
#define ECS_ITER_PRE_(type, e, i, ...) ECS_ITER_PRE__(ECS_ITER_KIND(type), type, e, i, __VA_ARGS__)
#define ECS_ITER_PRE__(kind, type, e, i, ...) CC_GET(kind, __VA_ARGS__)(type, i)

#define ECS_ITER_FETCH(e, i, fetch) ECS_ITER_FETCH_(ECS_ITER_TYPE(e), e, i, ECS_ITER_CONSUME fetch)
#define ECS_ITER_FETCH_(type, e, i, ...) ECS_ITER_FETCH__(ECS_ITER_KIND(type), type, e, i, __VA_ARGS__)
#define ECS_ITER_FETCH__(kind, type, e, i, ...) ECS_ITER_FETCH___(kind, type, e, i, CC_GET(kind, __VA_ARGS__)(type, i))
#define ECS_ITER_FETCH___(kind, type, e, i, ...) ECS_ITER_FETCH____(kind, type, e, i, __VA_ARGS__)
#define ECS_ITER_FETCH____(kind, type, e, i, fetch, valid) for (size_t ECS_ITER_PRIVATE__fetch##i = 0, ECS_ITER_PRIVATE__set##i = 0; !ECS_ITER_PRIVATE__fetch##i; ) for (void *ECS_ITER_PRIVATE__fetch_var##i = fetch; !ECS_ITER_PRIVATE__fetch##i++ valid(&& ECS_ITER_PRIVATE__fetch_var##i); ) for (ECS_ITER_DECLARE_VAR(e, i, ECS_ITER_PRIVATE__fetch_var##i); !ECS_ITER_PRIVATE__set##i++; )

#define ECS_ITER_NESTED(x) ECS_ITER_NESTED_(x)
#define ECS_ITER_NESTED_(x) ECS_ITER_NESTED_##x )
#define ECS_ITER_NESTED_0(x) ECS_ITER_NESTED_##x )
#define ECS_ITER_NESTED_const ECS_ITER_NESTED_0(

#define ECS_ITER_NESTED_NONE(e, i)

#define ECS_ITER_NESTED_ARRAY_ITERATOR(e, i) \
for (size_t ECS_ITER_PRIVATE__fetch_duplicate_index##i = 0, ECS_ITER_DECLARE_ELEMENT_INDEX_VAR(e, i, &ECS_ITER_PRIVATE__fetch_duplicate_index##i), ECS_ITER_PRIVATE__fetch_duplicate_count##i = CCArrayGetCount(*ECS_ITER_PRIVATE__fetch_array##i), ECS_ITER_PRIVATE__duplicate_set##i = 0; ECS_ITER_PRIVATE__fetch_duplicate_index##i < ECS_ITER_PRIVATE__fetch_duplicate_count##i; ECS_ITER_PRIVATE__fetch_duplicate_index##i++, ECS_ITER_PRIVATE__duplicate_set##i = 0) \
for (ECS_ITER_DECLARE_ELEMENT_VAR(e, i, CCArrayGetElementAtIndex(*ECS_ITER_PRIVATE__fetch_array##i, ECS_ITER_PRIVATE__fetch_duplicate_index##i)); !ECS_ITER_PRIVATE__duplicate_set##i++; )

#define ECS_ITER_DECLARE_ELEMENT_VAR(x, i, v) ECS_ITER_DECLARE_ELEMENT_##x, i, v)
#define ECS_ITER_DECLARE_ELEMENT_VAR_0(x, i, v) ECS_ITER_DECLARE_ELEMENT_##x, i, v)
#define ECS_ITER_DECLARE_ELEMENT_const const ECS_ITER_DECLARE_ELEMENT_VAR_0(

#define ECS_ITER_DECLARE_ELEMENT_INDEX_VAR(x, i, v) ECS_ITER_DECLARE_ELEMENT_INDEX_##x), i, v)
#define ECS_ITER_DECLARE_ELEMENT_INDEX_VAR_0(x, i, v) ECS_ITER_DECLARE_ELEMENT_INDEX_VAR_0_(x, i, v)
#define ECS_ITER_DECLARE_ELEMENT_INDEX_VAR_0_(x, i, v) ECS_ITER_DECLARE_ELEMENT_INDEX_##x), i, v)
#define ECS_ITER_DECLARE_ELEMENT_INDEX_const ECS_ITER_DECLARE_ELEMENT_INDEX_VAR_0(ECS_ITER_CONSUME(

#define ECS_ITER_NESTED_FETCH(e, i) ECS_ITER_NESTED(e)(e, i)

#define ECS_ITER_TYPE_ARRAY(...) __VA_ARGS__ ECS_ITER_IGNORE(
#define ECS_ITER_DECLARE_ARRAY(...) CCArray ECS_ITER_DECLARE_ASSIGN(
#define ECS_ITER_NESTED_ARRAY(...) ECS_ITER_NESTED_NONE ECS_ITER_IGNORE(

#ifndef ECS_ITER_DUPLICATE_ARRAY_SUFFIX
#define ECS_ITER_DUPLICATE_ARRAY_SUFFIX Array
#endif

#ifndef ECS_ITER_DUPLICATE_ARRAY_INDEX_SUFFIX
#define ECS_ITER_DUPLICATE_ARRAY_INDEX_SUFFIX Index
#endif

#ifndef ECS_ITER_INDEX
#define ECS_ITER_INDEX ECSIterIndex
#endif

#ifndef ECS_ITER_COUNT
#define ECS_ITER_COUNT ECSIterCount
#endif

#ifndef ECS_ITER_ENTITIES
#define ECS_ITER_ENTITIES ECSIterEntities
#endif

#ifndef ECS_ITER_ENTITY
#define ECS_ITER_ENTITY ECSIterEntity
#endif

#define ECS_ITER(...) ECS_ITER_(1, __VA_ARGS__)
#define ECS_ITER_(count, ...) ECS_ITER__(count, ECS_ITER_INIT(ECS_ITER_TYPE(CC_GET(0, __VA_ARGS__))), __VA_ARGS__)
#define ECS_ITER__(count, ...) ECS_ITER___(count, __VA_ARGS__)
#define ECS_ITER___(count, entities, declare, pre, fetch, ...) \
for (size_t ECS_ITER_PRIVATE__pre = 0; !ECS_ITER_PRIVATE__pre; ) for (declare CC_EXPAND(ECS_ITER_IGNORE CC_SOFT_JOIN(ECS_ITER_CONSUME, CC_MAP_WITH(ECS_ITER_PRE, pre, __VA_ARGS__))); !ECS_ITER_PRIVATE__pre++; ) \
for (size_t ECS_ITER_PRIVATE__pre_ent = 0; !ECS_ITER_PRIVATE__pre_ent; ) for (void *ECS_ITER_PRIVATE__ArrayEntities = entities, *ECS_ITER_PRIVATE__PtrEntities = CCArrayGetData(ECS_ITER_PRIVATE__ArrayEntities); !ECS_ITER_PRIVATE__pre_ent++; ) \
for (size_t ECS_ITER_INDEX = 0, ECS_ITER_COUNT = CCArrayGetCount(ECS_ITER_PRIVATE__ArrayEntities); ECS_ITER_INDEX < ECS_ITER_COUNT; ECS_ITER_INDEX += (count)) \
for (size_t ECS_ITER_PRIVATE__ent = 0; !ECS_ITER_PRIVATE__ent; ) for (ECSEntity *ECS_ITER_ENTITIES = &((ECSEntity*)ECS_ITER_PRIVATE__PtrEntities)[ECS_ITER_INDEX], ECS_ITER_ENTITY = *ECS_ITER_ENTITIES; !ECS_ITER_PRIVATE__ent++; (void)ECS_ITER_ENTITY) \
CC_SOFT_JOIN(, CC_MAP_WITH(ECS_ITER_FETCH, fetch, __VA_ARGS__)) \
CC_SOFT_JOIN(, CC_MAP(ECS_ITER_NESTED_FETCH, __VA_ARGS__))

#define ECS_BATCH_ITER_ASSERT_1(x, ...) ECS_ITER_KIND(ECS_ITER_TYPE(x))
#define ECS_BATCH_ITER_ASSERT_2(x, ...) (1 << ECS_ITER_KIND(ECS_ITER_TYPE(x)))

#define ECS_BATCH_ITER_BIT_COUNT(x) ((((((((x) - (((x) >> 1) & 0x5555555555555555)) & 0x3333333333333333) + ((((x) - (((x) >> 1) & 0x5555555555555555)) >> 2) & 0x3333333333333333)) + (((((x) - (((x) >> 1) & 0x5555555555555555)) & 0x3333333333333333) + ((((x) - (((x) >> 1) & 0x5555555555555555)) >> 2) & 0x3333333333333333)) >> 4)) & 0x0f0f0f0f0f0f0f0f) * 0x0101010101010101) >> 56)

#define ECS_BATCH_ITER(count, ...) \
_Static_assert((CC_SOFT_JOIN(+, CC_MAP(ECS_BATCH_ITER_ASSERT_1, __VA_ARGS__)) <= 1) && (ECS_BATCH_ITER_BIT_COUNT(CC_SOFT_JOIN(|, CC_MAP(ECS_BATCH_ITER_ASSERT_2, __VA_ARGS__))) == 1), "ECS_BATCH_ITER can only be used on groups of components that can be batched together"); \
ECS_ITER_(count, __VA_ARGS__)

#pragma mark - Assertions & Conditions

#define ECS_IS_CONDITION(x, _, arg) ECS_IS_CHECK_##x(arg)
    
#define ECS_IS_CHECK_Archetype(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeArchetype)
#define ECS_IS_CHECK_Packed(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypePacked)
#define ECS_IS_CHECK_Indexed(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeIndexed)
#define ECS_IS_CHECK_Local(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeLocal)
    
#define ECS_IS_CHECK_Duplicate(arg) ((arg & ECSComponentStorageModifierMask) == ECSComponentStorageModifierDuplicate)
#define ECS_IS_CHECK_Tag(arg) ((arg & ECSComponentStorageModifierMask) == ECSComponentStorageModifierTag)
    
#define ECS_IS(arg, ...) CC_SOFT_JOIN(&&, CC_MAP_WITH(ECS_IS_CONDITION, ECS_ID_##arg, __VA_ARGS__))
#define ECS_IS_NOT(arg, ...) !CC_SOFT_JOIN(&& !, CC_MAP_WITH(ECS_IS_CONDITION, ECS_ID_##arg, __VA_ARGS__))
    
#define ECS_ASSERT(arg, ...) _Static_assert(ECS_IS(arg, __VA_ARGS__), "Expects component to be the following type: " #__VA_ARGS__)
#define ECS_ASSERT_NOT(arg, ...) _Static_assert(ECS_IS_NOT(arg, __VA_ARGS__), "Expects component to not be the following type: " #__VA_ARGS__)

#endif
