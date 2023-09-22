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

/*!
 * @define ECS_LOCAL_COMPONENT
 * @abstract Mark a local component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_LOCAL_COMPONENT(...)

/*!
 * @define ECS_PACKED_COMPONENT
 * @abstract Mark a packed component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_PACKED_COMPONENT(...)

/*!
 * @define ECS_INDEXED_COMPONENT
 * @abstract Mark an indexed component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_INDEXED_COMPONENT(...)

/*!
 * @define ECS_ARCHETYPE_COMPONENT
 * @abstract Mark an archetype component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_ARCHETYPE_COMPONENT(...)

/*!
 * @define ECS_LOCAL_DUPLICATE_COMPONENT
 * @abstract Mark a local duplicate component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_LOCAL_DUPLICATE_COMPONENT(...)

/*!
 * @define ECS_PACKED_DUPLICATE_COMPONENT
 * @abstract Mark a packed duplicate component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_PACKED_DUPLICATE_COMPONENT(...)

/*!
 * @define ECS_INDEXED_DUPLICATE_COMPONENT
 * @abstract Mark an indexed duplicate component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_INDEXED_DUPLICATE_COMPONENT(...)

/*!
 * @define ECS_ARCHETYPE_DUPLICATE_COMPONENT
 * @abstract Mark an archetype duplicate component for the ecs_tool.
 * @param type The type name of the component.
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_ARCHETYPE_DUPLICATE_COMPONENT(...)

/*!
 * @define ECS_LOCAL_TAG
 * @abstract Mark a local tag for the ecs_tool.
 * @param type The type name of the tag (generally should be of type void).
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_LOCAL_TAG(...)

/*!
 * @define ECS_PACKED_TAG
 * @abstract Mark a packed tag for the ecs_tool.
 * @param type The type name of the tag (generally should be of type void).
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_PACKED_TAG(...)

/*!
 * @define ECS_INDEXED_TAG
 * @abstract Mark an indexed tag for the ecs_tool.
 * @param type The type name of the tag (generally should be of type void).
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_INDEXED_TAG(...)

/*!
 * @define ECS_ARCHETYPE_TAG
 * @abstract Mark an archetype tag for the ecs_tool.
 * @param type The type name of the tag (generally should be of type void).
 * @param index The optional explicit index of the component. If not provided one will be automatically assigned.
 */
#define ECS_ARCHETYPE_TAG(...)

/*!
 * @define ECS_DESTRUCTOR
 * @abstract Mark a component destructor for the ecs_tool.
 * @param destructor The @b ECSComponentDestructor callback to be used. Note if a callback is already marked this will override the previous one.
 * @param va_arg The type names of components that should use this destructor.
 */
#define ECS_DESTRUCTOR(destructor, ...)

/*!
 * @define ECS_SYSTEM
 * @abstract Mark a system for the ecs_tool.
 * @param system The @b ECSSystemUpdateCallback callback for the system.
 * @param read The list of components that the system needs read access to. Wrap the list in parantheses, empty parantheses will indicate no read components.
 *             Note: tag components should typically only require read access, additionally if the component data will be modified in a threadsafe manner handled
 *             internally by the system (say an atomic member on the component data) then it may also be marked as read.
 *
 * @param write The list of components that the system needs write access to. Wrap the list in parantheses, empty parantheses will indicate no write components.
 *              Note: mutation functions do not require write access, write access is only required if the component data will be modified or if you'll be writing
 *              to some other private non-threadsafe data that you wish to use the component as a "lock" to.
 *
 * @return Returns the system function declaration.
 */
#define ECS_SYSTEM(system, ...) void system(ECSContext *ECS_CONTEXT_VAR, ECSArchetype *ECS_ARCHETYPE_VAR, const size_t *ECS_ARCHETYPE_COMPONENT_INDEXES_VAR, const size_t *ECS_COMPONENT_OFFSETS_VAR, ECSRange ECS_RANGE_VAR, ECSTime ECS_TIME_VAR)

/*!
 * @define ECS_PARALLEL_SYSTEM
 * @abstract Mark a parallel system for the ecs_tool.
 * @warning Parallel systems guarantee they can run in parallel without threading issues.
 * @param system The @b ECSSystemUpdateCallback callback for the system.
 * @param read The list of components that the system needs read access to. Wrap the list in parantheses, empty parantheses will indicate no read components.
 * @param write The list of components that the system needs write access to. Wrap the list in parantheses, empty parantheses will indicate no write components.
 *              Note: mutation functions do not require write access, write access is only required if the component data will be modified or if you'll be writing
 *              to some other private non-threadsafe data that you wish to use the component as a "lock" to.
 *
 * @param parallelism Optionally specify how the system should be parallelised. By default systems are parallelised by archetype with no chunking, but they may also
 *                    be parallelised by a specific component type and number (chunks) of components of that type. Valid options are indicating the size of the archetype
 *                    component chunks (this is SIZE_MAX by default) by using an integer (may also be a macro to an integer), or specifying the compone and chunk size
 *                    by providing a tuple with the component type and chunk size @b (component, @b size).
 *
 * @return Returns the system function declaration.
 */
#define ECS_PARALLEL_SYSTEM(...) ECS_SYSTEM(__VA_ARGS__)

/*!
 * @define ECS_SYSTEM_GROUP
 * @abstract Mark a system group for the ecs_tool.
 * @param group The unique name of the group.
 * @param frequency The frequency for when to process the group during an @b ECSTick. The frequency is an integer time in nanoseconds (can use the ECS_TIME macros for
 *                  for convenience).
 *
 * @param va_arg The system priority/groupings. This should take the form of @b PRIORITY(index, @b systems, @b dependency) where @b index is the order of the priority
 *               group, @b systems is the list of systems to be run during that priority (wrap them in parantheses, or empty parantheses to indicate no systems to be
 *               executed), and @b dependency is the optional group depdency requirement (this should be a tuple of @b (group_name, @b priority_index) by default it
 *               depends on the previous priority in the same group having completed.
 */
#define ECS_SYSTEM_GROUP(...)

#define ECS_COMPONENT_ACCESS(system, component) ECS_COMPONENT_ACCESS_(system##_##component)
#define ECS_COMPONENT_ACCESS_(...) ECS_COMPONENT_ACCESS__(__VA_ARGS__)
#define ECS_COMPONENT_ACCESS__(components, _e, _q) components

#define ECS_ENTITY_ACCESS(system, component) ECS_ENTITY_ACCESS_(system##_##component)
#define ECS_ENTITY_ACCESS_(...) ECS_ENTITY_ACCESS__(__VA_ARGS__)
#define ECS_ENTITY_ACCESS__(_c, entities, _q) entities

#define ECS_QUALIFIER_ACCESS(system, component) ECS_QUALIFIER_ACCESS_(system##_##component)
#define ECS_QUALIFIER_ACCESS_(...) ECS_QUALIFIER_ACCESS__(__VA_ARGS__)
#define ECS_QUALIFIER_ACCESS__(_c, _e, qualifier) qualifier

/*!
 * @define ECS_GET
 * @abstract Get the component array for the given component type.
 * @note This should only be used within a system update function, and should only reference a component that the system has marked as needing read or write access to.
 * @param component The type name of the component to get the component data array for.
 * @return Returns the @b CCArray of the components, or NULL if there is none. Note: local components do not have a component array.
 */
#define ECS_GET(component) ECS_GET_(ECS_SYSTEM_NAME, component)
#define ECS_GET_(system, component) ECS_COMPONENT_ACCESS(system, component)

/*!
 * @define ECS_ENTITIES
 * @abstract Get the entity array for the given component type.
 * @note This should only be used within a system update function, and should only reference a component that the system has marked as needing read or write access to.
 * @param component The type name of the component to get the entities for.
 * @return Returns the @b CCArray of the entities, or NULL if there is none. Note: indexed and local components do not have an entity array.
 */
#define ECS_ENTITIES(component) ECS_ENTITIES_(ECS_SYSTEM_NAME, component)
#define ECS_ENTITIES_(system, component) ECS_ENTITY_ACCESS(system, component)

/*!
 * @define ECS_QUALIFIER
 * @abstract Get access qualifier the given component type.
 * @note This should only be used within a system update function, and should only reference a component that the system has marked as needing read or write access to.
 * @param component The type name of the component to get the qualifier for.
 * @return Returns the @b const if the component as read access, otherwise returns no qualifier if it has write access.
 */
#define ECS_QUALIFIER(component) ECS_QUALIFIER_(ECS_SYSTEM_NAME, component)
#define ECS_QUALIFIER_(system, component) ECS_QUALIFIER_ACCESS(system, component)

/*!
 * @define ECS_CONTEXT_VAR
 * @abstract The variable name to be used for the context argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b Context.
 */
#ifndef ECS_CONTEXT_VAR
#define ECS_CONTEXT_VAR Context
#endif

/*!
 * @define ECS_ARCHETYPE_VAR
 * @abstract The variable name to be used for the archetype argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b Archetype.
 */
#ifndef ECS_ARCHETYPE_VAR
#define ECS_ARCHETYPE_VAR Archetype
#endif

/*!
 * @define ECS_ARCHETYPE_COMPONENT_INDEXES_VAR
 * @abstract The variable name to be used for the archetype component indexes argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b ArchetypeComponentIndexes.
 */
#ifndef ECS_ARCHETYPE_COMPONENT_INDEXES_VAR
#define ECS_ARCHETYPE_COMPONENT_INDEXES_VAR ArchetypeComponentIndexes
#endif

/*!
 * @define ECS_COMPONENT_OFFSETS_VAR
 * @abstract The variable name to be used for the component offsets argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b ArchetypeCoComponentOffsetsmponentIndexes.
 */
#ifndef ECS_COMPONENT_OFFSETS_VAR
#define ECS_COMPONENT_OFFSETS_VAR ComponentOffsets
#endif

/*!
 * @define ECS_RANGE_VAR
 * @abstract The variable name to be used for the range argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b Range.
 */
#ifndef ECS_RANGE_VAR
#define ECS_RANGE_VAR Range
#endif

/*!
 * @define ECS_TIME_VAR
 * @abstract The variable name to be used for the time argument.
 * @description This can be defined to a custom name, otherwise it defaults to @b Time.
 */
#ifndef ECS_TIME_VAR
#define ECS_TIME_VAR Time
#endif

/*!
 * @define ECS_SYSTEM_FUN
 * @abstract When @b ECS_SYSTEM_NAME is defined, this macro can be used to create the function definition.
 */
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

// archetype
#define ECS_ITER_INIT_0(type) ,                        (ECS_ITER_ENTITY_ARRAY, ECS_ENTITIES(type)), void,                                   ((,) ECS_ITER_ARCHETYPE_PRE_INIT, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_ARCHETYPE_FETCH, ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)
// packed
#define ECS_ITER_INIT_1(type) if (ECS_ENTITIES(type)), (ECS_ITER_ENTITY_ARRAY, ECS_ENTITIES(type)), void ECS_ITER_PACKED_PRE_INIT(type, 0), (() ECS_ITER_IGNORE,              () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_WARNING_FETCH,   ECS_ITER_PACKED_FETCH,   ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)
// indexed
#define ECS_ITER_INIT_2(type) ,                        (ECS_ITER_ENTITY_FALLBACK, "indexed", type), ,                                       (() ECS_ITER_IGNORE,              () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_WARNING_FETCH,   ECS_ITER_FALLBACK_FETCH,   ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)
// local
#define ECS_ITER_INIT_3(type) ,                        (ECS_ITER_ENTITY_FALLBACK, "local", type), ,                                         (() ECS_ITER_IGNORE,              () ECS_ITER_IGNORE, () ECS_ITER_IGNORE, () ECS_ITER_IGNORE), (ECS_ITER_WARNING_FETCH,   ECS_ITER_FALLBACK_FETCH,   ECS_ITER_FALLBACK_FETCH, ECS_ITER_FALLBACK_FETCH)

#define ECS_ITER_FETCH_ENTITY(increment, ...) ECS_ITER_FETCH_ENTITY_(increment, __VA_ARGS__)
#define ECS_ITER_FETCH_ENTITY_(increment, fetch, ...) fetch(increment, __VA_ARGS__)

#define ECS_ITER_ENTITY_ARRAY(increment, entities) \
for (size_t ECS_ITER_PRIVATE__pre_ent = 0; !ECS_ITER_PRIVATE__pre_ent; ) for (void *ECS_ITER_PRIVATE__ArrayEntities = entities, *ECS_ITER_PRIVATE__PtrEntities = CCArrayGetData(ECS_ITER_PRIVATE__ArrayEntities); !ECS_ITER_PRIVATE__pre_ent++; ) \
for (size_t ECS_ITER_INDEX = ECS_RANGE_VAR.index, ECS_ITER_COUNT = CCMin(ECS_RANGE_VAR.count, CCArrayGetCount(ECS_ITER_PRIVATE__ArrayEntities)); ECS_ITER_INDEX < ECS_ITER_COUNT; ECS_ITER_INDEX += (increment)) \
for (size_t ECS_ITER_PRIVATE__ent = 0; !ECS_ITER_PRIVATE__ent; ) for (const ECSEntity *ECS_ITER_ENTITIES = &((ECSEntity*)ECS_ITER_PRIVATE__PtrEntities)[ECS_ITER_INDEX], ECS_ITER_ENTITY = *ECS_ITER_ENTITIES; !ECS_ITER_PRIVATE__ent++; (void)ECS_ITER_ENTITY)

#define ECS_ITER_ENTITY_FALLBACK(increment, kind, type) \
CC_WARNING("Iterating with a leading " kind " component (" #type ") may be slow. Use an archetype or packed component for faster iteration.") \
for (size_t ECS_ITER_INDEX = ECS_RANGE_VAR.index, ECS_ITER_COUNT = CCMin(ECS_RANGE_VAR.count, CCArrayGetCount(ECS_CONTEXT_VAR->manager.map)); ECS_ITER_INDEX < ECS_ITER_COUNT; ECS_ITER_INDEX += (increment)) \
for (size_t ECS_ITER_PRIVATE__ent = 0; !ECS_ITER_PRIVATE__ent; ) for (ECSEntity ECS_ITER_ENTITY = ECS_ITER_INDEX; !ECS_ITER_PRIVATE__ent++; (void)ECS_ITER_ENTITY)

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
#define ECS_ITER_FETCH____(kind, type, e, i, fetch, valid) for (size_t ECS_ITER_PRIVATE__fetch##i = 0, ECS_ITER_PRIVATE__set##i = 0; !ECS_ITER_PRIVATE__fetch##i; ) for (void *ECS_ITER_PRIVATE__fetch_var##i = fetch; !ECS_ITER_PRIVATE__fetch##i++ valid(&& ECS_ITER_PRIVATE__fetch_var##i); ) for (ECS_ITER_DECLARE_VAR(e, i, (ECS_QUALIFIER(type) void*)ECS_ITER_PRIVATE__fetch_var##i); !ECS_ITER_PRIVATE__set##i++; )

#define ECS_ITER_NESTED(x) ECS_ITER_NESTED_(x)
#define ECS_ITER_NESTED_(x) ECS_ITER_NESTED_##x )
#define ECS_ITER_NESTED_0(x) ECS_ITER_NESTED_##x )
#define ECS_ITER_NESTED_const ECS_ITER_NESTED_0(

#define ECS_ITER_NESTED_NONE(e, i)

#define ECS_ITER_NESTED_ARRAY_ITERATOR(e, i) \
for (size_t ECS_ITER_PRIVATE__fetch_duplicate_index##i = 0, ECS_ITER_DECLARE_ELEMENT_INDEX_VAR(e, i, &ECS_ITER_PRIVATE__fetch_duplicate_index##i), ECS_ITER_PRIVATE__fetch_duplicate_count##i = CCArrayGetCount(*ECS_ITER_PRIVATE__fetch_array##i), ECS_ITER_PRIVATE__duplicate_set##i = 0; ECS_ITER_PRIVATE__fetch_duplicate_index##i < ECS_ITER_PRIVATE__fetch_duplicate_count##i; ECS_ITER_PRIVATE__fetch_duplicate_index##i++, ECS_ITER_PRIVATE__duplicate_set##i = 0) \
for (ECS_ITER_DECLARE_ELEMENT_VAR(e, i, (ECS_QUALIFIER(ECS_ITER_TYPE(e)) void*)CCArrayGetElementAtIndex(*ECS_ITER_PRIVATE__fetch_array##i, ECS_ITER_PRIVATE__fetch_duplicate_index##i)); !ECS_ITER_PRIVATE__duplicate_set##i++; )

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

/*!
 * @define ECS_ITER_DUPLICATE_ARRAY_SUFFIX
 * @abstract The suffix to be used when naming duplicate array variables.
 * @description This can be defined to a custom name, otherwise it defaults to @b Array.
 */
#ifndef ECS_ITER_DUPLICATE_ARRAY_SUFFIX
#define ECS_ITER_DUPLICATE_ARRAY_SUFFIX Array
#endif

/*!
 * @define ECS_ITER_DUPLICATE_ARRAY_INDEX_SUFFIX
 * @abstract The suffix to be used when naming duplicate array index variables.
 * @description This can be defined to a custom name, otherwise it defaults to @b Index.
 */
#ifndef ECS_ITER_DUPLICATE_ARRAY_INDEX_SUFFIX
#define ECS_ITER_DUPLICATE_ARRAY_INDEX_SUFFIX Index
#endif

/*!
 * @define ECS_ITER_INDEX
 * @abstract The variable name to be used for the iteration index.
 * @description This can be defined to a custom name, otherwise it defaults to @b ECSIterIndex.
 */
#ifndef ECS_ITER_INDEX
#define ECS_ITER_INDEX ECSIterIndex
#endif

/*!
 * @define ECS_ITER_COUNT
 * @abstract The variable name to be used for the iteration count.
 * @description This can be defined to a custom name, otherwise it defaults to @b ECSIterCount.
 */
#ifndef ECS_ITER_COUNT
#define ECS_ITER_COUNT ECSIterCount
#endif

/*!
 * @define ECS_ITER_ENTITIES
 * @abstract The variable name to be used for the iteration entity array.
 * @description This can be defined to a custom name, otherwise it defaults to @b ECSIterEntities.
 */
#ifndef ECS_ITER_ENTITIES
#define ECS_ITER_ENTITIES ECSIterEntities
#endif

/*!
 * @define ECS_ITER_ENTITY
 * @abstract The variable name to be used for the iteration entity.
 * @description This can be defined to a custom name, otherwise it defaults to @b ECSIterEntity.
 */
#ifndef ECS_ITER_ENTITY
#define ECS_ITER_ENTITY ECSIterEntity
#endif

/*!
 * @define ECS_ITER
 * @abstract A component iterator for use in a system.
 * @param va_arg The component variable declarations for the components that should be iterated on. The first variable will be used as the leading
 *               iterator component, which will affect how the generated iterator will be optimised. e.g. When an archetype component is leading
 *               then all other archetype component variables will be iterated on optimally.
 *
 *               Variable declarations must be made const (or use @b ECS_QUALIFIER) when the system only requested read access to them. For write
 *               access they can be either const or non-const.
 *
 *               A parallel system should use the component that is parallelised as the leading component.
 *
 *               When iterating duplicate components, both the duplicate components and the array containing the duplicate components can be iterated
 *               on. To iterate the duplicate components just use the component type, to iterate the duplicate component arrays wrap the component type
 *               in ARRAY(type). e.g.
 *
 *                  Type *x // will iterate the duplicate component type
 *                  ARRAY(Type) *x // will iterate the duplicate component array
 */
#define ECS_ITER(...) ECS_ITER_(1, __VA_ARGS__)
#define ECS_ITER_(increment, ...) ECS_ITER__(increment, ECS_ITER_INIT(ECS_ITER_TYPE(CC_GET(0, __VA_ARGS__))), __VA_ARGS__)
#define ECS_ITER__(increment, ...) ECS_ITER___(increment, __VA_ARGS__)
#define ECS_ITER___(increment, guard, entities, declare, pre, fetch, ...) \
ECS_ITER_ASSERT(__VA_ARGS__) \
guard \
for (size_t ECS_ITER_PRIVATE__pre = 0; !ECS_ITER_PRIVATE__pre; ) for (declare CC_EXPAND(ECS_ITER_IGNORE CC_SOFT_JOIN(ECS_ITER_CONSUME, CC_MAP_WITH(ECS_ITER_PRE, pre, __VA_ARGS__))); !ECS_ITER_PRIVATE__pre++; ) \
ECS_ITER_FETCH_ENTITY(increment, ECS_ITER_CONSUME entities) \
CC_SOFT_JOIN(, CC_MAP_WITH(ECS_ITER_FETCH, fetch, __VA_ARGS__)) \
CC_SOFT_JOIN(, CC_MAP(ECS_ITER_NESTED_FETCH, __VA_ARGS__))

#define ECS_BATCH_ITER_ASSERT_1(x, ...) ECS_ITER_KIND(ECS_ITER_TYPE(x))
#define ECS_BATCH_ITER_ASSERT_2(x, ...) (1 << ECS_ITER_KIND(ECS_ITER_TYPE(x)))

#define ECS_BATCH_ITER_BIT_COUNT(x) ((((((((x) - (((x) >> 1) & 0x5555555555555555)) & 0x3333333333333333) + ((((x) - (((x) >> 1) & 0x5555555555555555)) >> 2) & 0x3333333333333333)) + (((((x) - (((x) >> 1) & 0x5555555555555555)) & 0x3333333333333333) + ((((x) - (((x) >> 1) & 0x5555555555555555)) >> 2) & 0x3333333333333333)) >> 4)) & 0x0f0f0f0f0f0f0f0f) * 0x0101010101010101) >> 56)

/*!
 * @define ECS_BATCH_ITER
 * @abstract A batched component iterator for use in a system.
 * @param count The number of items to retrieve per iteration.
 * @param va_arg The component variable declarations for the components that should be iterated on. The first variable will be used as the leading
 *               iterator component, which will affect how the generated iterator will be optimised. e.g. When an archetype component is leading
 *               then all other archetype component variables will be iterated on optimally.
 *
 *               Variable declarations must be made const (or use @b ECS_QUALIFIER) when the system only requested read access to them. For write
 *               access they can be either const or non-const.
 *
 *               A parallel system should use the component that is parallelised as the leading component.
 *
 *               When iterating duplicate components, both the duplicate components and the array containing the duplicate components can be iterated
 *               on. To iterate the duplicate components just use the component type, to iterate the duplicate component arrays wrap the component type
 *               in ARRAY(type). e.g.
 *
 *                  Type *x // will iterate the duplicate component type
 *                  ARRAY(Type) *x // will iterate the duplicate component array
 */
#define ECS_BATCH_ITER(count, ...) \
_Static_assert((CC_SOFT_JOIN(+, CC_MAP(ECS_BATCH_ITER_ASSERT_1, __VA_ARGS__)) <= 1) && (ECS_BATCH_ITER_BIT_COUNT(CC_SOFT_JOIN(|, CC_MAP(ECS_BATCH_ITER_ASSERT_2, __VA_ARGS__))) == 1), "ECS_BATCH_ITER can only be used on groups of components that can be batched together"); \
ECS_ITER_(count, __VA_ARGS__)

#define ECS_ITER_ASSERT(...) CC_CAT(ECS_ITER_ASSERT_, ECS_SYSTEM_NAME)(__VA_ARGS__)

#define ECS_ITER_ASSERT_TYPE(...) CC_CAT(ECS_ITER_ASSERT_, ECS_SYSTEM_NAME, _, ECS_ITER_TYPE(CC_GET(0, __VA_ARGS__)))

#define ECS_ITER_ASSERT_KIND(expect, ...) CC_CAT(ECS_ITER_ASSERT_KIND_, expect, _, ECS_ITER_KIND(ECS_ITER_TYPE(CC_GET(0, __VA_ARGS__))))

#define ECS_ITER_ASSERT_KIND_0_0
#define ECS_ITER_ASSERT_KIND_0_1 CC_ERROR("Must iterate with an archetype component as the leading component");
#define ECS_ITER_ASSERT_KIND_0_2 CC_ERROR("Must iterate with an archetype component as the leading component");
#define ECS_ITER_ASSERT_KIND_0_3 CC_ERROR("Must iterate with an archetype component as the leading component");

#define ECS_ITER_ASSERT_KIND_1_0
#define ECS_ITER_ASSERT_KIND_1_1 CC_ERROR("Must iterate with a packed component as the leading component");
#define ECS_ITER_ASSERT_KIND_1_2 CC_ERROR("Must iterate with a packed component as the leading component");
#define ECS_ITER_ASSERT_KIND_1_3 CC_ERROR("Must iterate with a packed component as the leading component");

#define ECS_ITER_ASSERT_KIND_2_0
#define ECS_ITER_ASSERT_KIND_2_1 CC_ERROR("Must iterate with an indexed component as the leading component");
#define ECS_ITER_ASSERT_KIND_2_2 CC_ERROR("Must iterate with an indexed component as the leading component");
#define ECS_ITER_ASSERT_KIND_2_3 CC_ERROR("Must iterate with an indexed component as the leading component");

#define ECS_ITER_ASSERT_KIND_3_0
#define ECS_ITER_ASSERT_KIND_3_1 CC_ERROR("Must iterate with a local component as the leading component");
#define ECS_ITER_ASSERT_KIND_3_2 CC_ERROR("Must iterate with a local component as the leading component");
#define ECS_ITER_ASSERT_KIND_3_3 CC_ERROR("Must iterate with a local component as the leading component");

#pragma mark - Assertions & Conditions

#define ECS_IS_CONDITION(x, _, arg) ECS_IS_CHECK_##x(arg)

#define ECS_IS_CHECK_Archetype(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeArchetype)
#define ECS_IS_CHECK_Packed(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypePacked)
#define ECS_IS_CHECK_Indexed(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeIndexed)
#define ECS_IS_CHECK_Local(arg) ((arg & ECSComponentStorageTypeMask) == ECSComponentStorageTypeLocal)

#define ECS_IS_CHECK_Duplicate(arg) (arg & ECSComponentStorageModifierDuplicate)
#define ECS_IS_CHECK_Tag(arg) (arg & ECSComponentStorageModifierTag)
#define ECS_IS_CHECK_Destructor(arg) (arg & ECSComponentStorageModifierDestructor)

/*!
 * @define ECS_IS
 * @abstract Check whether a component type is of a certain type or modification.
 * @param va_arg The type (@b Archetype, @b Packed, @b Indexed, @b Local) or modifiers (@b Duplicate, @b Tag, @b Destructor) to check.
 * @return Returns TRUE if all the arguments are true for that type or FALSE if any are not.
 */
#define ECS_IS(arg, ...) CC_SOFT_JOIN(&&, CC_MAP_WITH(ECS_IS_CONDITION, ECS_ID_##arg, __VA_ARGS__))

/*!
 * @define ECS_IS_NOT
 * @abstract Check whether a component type is not of a certain type or modification.
 * @param va_arg The type (@b Archetype, @b Packed, @b Indexed, @b Local) or modifiers (@b Duplicate, @b Tag, @b Destructor) to check.
 * @return Returns TRUE if any of the arguments are not true for that type or FALSE if all of them are true.
 */
#define ECS_IS_NOT(arg, ...) !CC_SOFT_JOIN(&& !, CC_MAP_WITH(ECS_IS_CONDITION, ECS_ID_##arg, __VA_ARGS__))

/*!
 * @define ECS_ASSERT
 * @abstract Statically assert whether a component type is of a certain type or modification.
 * @param va_arg The type (@b Archetype, @b Packed, @b Indexed, @b Local) or modifiers (@b Duplicate, @b Tag, @b Destructor) to check.
 */
#define ECS_ASSERT(arg, ...) _Static_assert(ECS_IS(arg, __VA_ARGS__), "Expects component to be the following type: " #__VA_ARGS__)

/*!
 * @define ECS_ASSERT_NOT
 * @abstract Statically assert whether a component type is not of a certain type or modification.
 * @param va_arg The type (@b Archetype, @b Packed, @b Indexed, @b Local) or modifiers (@b Duplicate, @b Tag, @b Destructor) to check.
 */
#define ECS_ASSERT_NOT(arg, ...) _Static_assert(ECS_IS_NOT(arg, __VA_ARGS__), "Expects component to not be the following type: " #__VA_ARGS__)

#endif
