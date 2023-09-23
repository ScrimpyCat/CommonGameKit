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

/*!
 * @header ECS
 * @abstract The ECS (Entity Component System)
 * @discussion This overview for the ECS is split into a number of concepts. These are Setup, Contexts, Groups, Systems, Components, Entities, Mutations, Concurrency,
 *             ecs\_tool, Configuration.
 *
 *             ## Setup
 *             To setup the ECS you will need to do the following:
 *
 *             1) Set the following global variables:
 *
 *                  IDs
 *                  - @b ECSComponentIDs
 *
 *                  Archetype Component Indexes
 *                  - @b ECSArchetypeComponentIndexes
 *
 *                  Component Sizes
 *                  - @b ECSArchetypeComponentSizes
 *                  - @b ECSPackedComponentSizes
 *                  - @b ECSIndexedComponentSizes
 *                  - @b ECSLocalComponentSizes
 *                  - @b ECSDuplicateArchetypeComponentSizes
 *                  - @b ECSDuplicatePackedComponentSizes
 *                  - @b ECSDuplicateIndexedComponentSizes
 *                  - @b ECSDuplicateLocalComponentSizes
 *
 *                  Component Destructors
 *                  - @b ECSArchetypeComponentDestructors
 *                  - @b ECSPackedComponentDestructors
 *                  - @b ECSIndexedComponentDestructors
 *                  - @b ECSLocalComponentDestructors
 *                  - @b ECSDuplicateArchetypeComponentDestructors
 *                  - @b ECSDuplicatePackedComponentDestructors
 *                  - @b ECSDuplicateIndexedComponentDestructors
 *                  - @b ECSDuplicateLocalComponentDestructors
 *
 *                  Mutable State Sizes
 *                  - @b ECSMutableStateEntitiesMax
 *                  - @b ECSMutableStateAddComponentMax
 *                  - @b ECSMutableStateRemoveComponentMax
 *                  - @b ECSMutableStateCustomCallbackMax
 *                  - @b ECSMutableStateSharedDataMax
 *
 *             2) Call @b ECSInit
 *             3) Create at least one @b ECSWorkerCreate
 *
 *             ## Contexts
 *             Contexts represent the state of a simulation. Multiple contexts can be used when running multiple separate simulations is desired.
 *
 *             To initialise a context, it should be zeroed out with the @b mutation and @b manager fields being set. An example setup of these could be:
 *
 *             ```c
 *             ECSContext Context;
 *             ECSMutableState MutableState = ECS_MUTABLE_STATE_CREATE(ECSMutableStateEntitiesMax, ECSMutableStateAddComponentMax, ECSMutableStateRemoveComponentMax, ECSMutableStateCustomCallbackMax, ECSMutableStateSharedDataMax);
 *             memset(&Context, 0, sizeof(Context));
 *             Context.mutations = &MutableState;
 *             Context.manager.map = CCArrayCreate(CC_ALIGNED_ALLOCATOR(ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), CC_ALIGN(sizeof(ECSComponentRefs) + LOCAL_STORAGE_SIZE, ECS_ARCHETYPE_COMPONENT_IDS_ALIGNMENT), 16);
 *             Context.manager.available = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(size_t), 16);
 *             ```
 *
 *             ## Groups
 *             Groups define how systems should run. Both the frequency, the ordering of the systems, and their dependencies (what they need to wait for before they can run).
 *             To create a group use @b ECS_SYSTEM_GROUP and the ecs\_tool to generate the configuration.
 *
 *             Systems in a single priority (or those of another group) can be run in concurrently. Concurrent execution of the systems will depend on their component access
 *             requiremets. If two systems require write access to the same component then they won't be run at the same time, but if the two systems only require read access
 *             then they will.
 *
 *             ## Systems
 *             A system is processes a specific set of components. It is the primary way to interact with the @b Context and @b Components. To create a system use @b ECS_SYSTEM
 *             or @b ECS_PARALLEL_SYSTEM when the system can be parallelised.
 *
 *             Systems can manage their own internal state as long as they guarantee that it will still be threadsafe.
 *
 *             Performing mutations is the main way for a system to apply breaking changes to the current @b Context. All the mutations will be applied later when
 *             @b ECSMutationApply is called.
 *
 *             ## Components
 *             A component is a store of data that is attached to an entity. There are different storage types and modifiers that change the use case of the component.
 *
 *             #### Storage Types
 *                  These change how the component data is stored internally.
 *
 *                  ##### Archetype
 *                  Archetype storage will group other components of the same storage the entity has into an archetype. All the components in an archetype will have the same order
 *                  and are packed together which allows for easy vectorisation and iteration of multiple components. Adding or removal of components is more costly and ideally
 *                  should opt for batch adds/removes instead of doing it one by one.
 *
 *                  ##### Packed
 *                  Packed storage is similar to archetype storage with the exception that multiple component types are not grouped together. So each packed component is packed
 *                  together but the order of one packed component type vs another are not guaranteed to be the same. This storage type allows for vectorisation and iteration of
 *                  a single component.
 *
 *                  ##### Indexed
 *                  Indexed storage keeps the component data sepaarated by an Entity's ID. This means random lookups are faster than the archetype or packed variants, but iteration
 *                  is quite slow. If iteration is desired, it is best to parallelise the system by this component at a reasonable chunk size.
 *
 *                  ##### Local
 *                  Local storage stores the component data in the entity itself. Similar to indexed, this means lookups are fast but iteration is slow. This storage type offers
 *                  the fastest way to add/remove components, with the downside pre-allocate memory for them (regardless of if an entity has the component or not).
 *
 *             #### Storage Modifiers
 *                  These change the behaviour of components. Certain modifiers can be combined.
 *
 *                  ##### Duplicate
 *                  Duplicate components allow for multiple components of the same type to be added to an entity.
 *
 *                  ##### Tag
 *                  Tagged components allow for components with no data to be attached to an entity.
 *
 *                  ##### Destructor
 *                  Component destructors allow for custom cleanup of component data when a component is removed.
 *
 *             To register a component type use any of the following:
 *              - @b ECS_LOCAL_COMPONENT
 *              - @b ECS_PACKED_COMPONENT
 *              - @b ECS_INDEXED_COMPONENT
 *              - @b ECS_ARCHETYPE_COMPONENT
 *              - @b ECS_LOCAL_DUPLICATE_COMPONENT
 *              - @b ECS_PACKED_DUPLICATE_COMPONENT
 *              - @b ECS_INDEXED_DUPLICATE_COMPONENT
 *              - @b ECS_ARCHETYPE_DUPLICATE_COMPONENT
 *              - @b ECS_LOCAL_TAG
 *              - @b ECS_PACKED_TAG
 *              - @b ECS_INDEXED_TAG
 *              - @b ECS_ARCHETYPE_TAG
 *
 *             To register a destructor for a component use @b ECS_DESTRUCTOR.
 *
 *             ## Entities
 *             An entity is what groups associated component data together.
 *
 *             Entity destruction will remove the components from that entity and make it available for reuse. Certain operations can still be safely performed on a destroyed entity,
 *             but mutating the entity should be avoided.
 *
 *             ## Mutations
 *             Mutations allow for breaking changes to be deferred until a later point. As it can be unsafe for a system to add or remove components of a certain storage type, creating
 *             a mutation request with that change is the safe way to do that. These mutation requests can also be inspected to see what changes will be applied.
 *
 *             Applying a mutation could be done on a worker as long as you can make the guarantee that @b Context will not be modified on another at the same time.
 *
 *             ## Concurrency
 *             The core concurrency mechanism is through scheduling systems to a pool of worker threads. When threads are waiting for work to become available to them they will call into a
 *             waiting callback that can perform some custom work in the meantime.
 *
 *             ## ecs_tool
 *             The CLI tool for generating a lot of the boilerplate that is required.
 *
 *             ## Configuration
 *             These are the compile time configuation options for the ECS itself (will require recompilation of the framework).
 *
 *             #### Defines
 *                  ##### ECS_SYSTEM_EXECUTOR_COMPONENT_MAX 16
 *                  This should be set to at least the maximum number of components that a system will require access to. By default this is set to 16.
 *
 *                  ##### ECS_SYSTEM_EXECUTION_POOL_MAX 1024
 *                  This should be set to a size that's larger than the maximum number of systems that may be scheduled to the worker pool at any one time. By default this is set to 1024.
 *
 *                  ##### ECS_WORKER_THREAD_MAX
 *                  This can be defined to the hard maximum worker thread limit. By default this is set to 128.
 *
 *                  Note: To create N worker threads you must still call @b ECSWorkerCreate N times. This limit just affects what the total allowable number of worker threads that may be
 *                  created is.
 *
 *                  ##### Component Maxes
 *                  The maximum number of components per storage type can be set by defining the following:
 *                      - @b ECS_ARCHETYPE_COMPONENT_MAX
 *                      - @b ECS_PACKED_COMPONENT_MAX
 *                      - @b ECS_INDEXED_COMPONENT_MAX
 *                      - @b ECS_LOCAL_COMPONENT_MAX
 *
 *                  This will allow the default maximums to be overriden. Changing these values may require the ECSConfig.h to be regenerated, or for the @b ecs_tool to regenerate the project
 *                  boilerplate.
 *
 *                  ##### ECS_UNSAFE_COMPONENT_DESTRUCTION
 *                  If component destructors don't reference any other component data than the component being destroyed, then @b ECS_UNSAFE_COMPONENT_DESTRUCTION can be defined as 1 to enable
 *                  the unsafe destruction pathway. The benefit of this is it will avoid a copy of the component data.
 *
 *                  ##### Array Chunk Sizes
 *                  The chunk size of the internal arrays can be overriden by defining the following:
 *                      - `ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(index, count)` : @b index is the archetype index, and @b count is the number of components in the archetype
 *                      - `ECS_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the packed component
 *                      - `ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the indexed component
 *                      - `ECS_DUPLICATE_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the duplicate archetype component
 *                      - `ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the duplicate packed component
 *                      - `ECS_DUPLICATE_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the duplicate indexed component
 *                      - `ECS_DUPLICATE_LOCAL_COMPONENT_ARRAY_CHUNK_SIZE(index)` : @b index is the index of the duplicate local component
 *
 *                  By default these are all set to 16.
 */

#ifndef CommonGameKit_ECS_h
#define CommonGameKit_ECS_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSTool.h>
#include <CommonGameKit/ECSConfig.h>

#include <CommonGameKit/ECSComponent.h>

#include <CommonGameKit/ECSEntity.h>

CC_ARRAY_DECLARE(ECSEntity);

#include <CommonGameKit/ECSArchetype.h>

#include <CommonGameKit/ECSContext.h>

#include <CommonGameKit/ECSMutation.h>

/*!
 * @brief The representation of time used by the ECS.
 * @description This can be any linear unit as long as it is consistent.
 * @note The default conversion helpers (@b ECS_TIME_FROM_* and @b ECS_TIME_TO_*) assume nanoseconds.
 */
typedef uint64_t ECSTime;

#define ECS_TIME_FROM_HOURS(x) (ECSTime)((x) * 3600000000000ULL)
#define ECS_TIME_FROM_MINUTES(x) (ECSTime)((x) * 60000000000ULL)
#define ECS_TIME_FROM_SECONDS(x) (ECSTime)((x) * 1000000000ULL)
#define ECS_TIME_FROM_MILLISECONDS(x) (ECSTime)((x) * 1000000ULL)
#define ECS_TIME_FROM_MICROSECONDS(x) (ECSTime)((x) * 1000ULL)
#define ECS_TIME_FROM_NANOSECONDS(x) (ECSTime)(x)

#define ECS_TIME_TO_HOURS(x) ((uint64_t)(x) / 3600000000000ULL)
#define ECS_TIME_TO_MINUTES(x) ((uint64_t)(x) / 60000000000ULL)
#define ECS_TIME_TO_SECONDS(x) ((uint64_t)(x) / 1000000000ULL)
#define ECS_TIME_TO_MILLISECONDS(x) ((uint64_t)(x) / 1000000ULL)
#define ECS_TIME_TO_MICROSECONDS(x) ((uint64_t)(x) / 1000ULL)
#define ECS_TIME_TO_NANOSECONDS(x) (uint64_t)(x)

typedef struct {
    size_t archetype;
    size_t componentIndexes;
    size_t count;
} ECSArchetypePointer;

typedef struct {
    struct {
        const ECSComponentID *ids;
        size_t count;
    } read, write;
    struct {
        const ECSArchetypePointer *pointer;
        size_t count;
    } archetype;
    struct {
        const size_t *offsets;
    } component;
} ECSSystemAccess;

typedef struct {
    size_t index;
    size_t count;
} ECSRange;

/*!
 * @brief A callback for a system update.
 * @description For serial execution this callback will be once or more on the same thread if any of the requested components are in an archetype, or
 *              only once if no archetype components were requested. While for parallel execution this callback may be called multiple tiles
 *              simultaneously across different threads.
 *
 * @warning This function must be threadsafe if it is marked as parallel.
 * @note A release memory barrier is used after calling this function. However note that no acquire barrier is used before.
 * @param Context The context the system will operate on.
 * @param Archetype The archetype to operate on or NULL if there is none.
 * @param ArchetypeComponentIndexes The component indexed for the requested components in the archetype.
 * @param ComponentOffsets The offsets to the other requested components (except for locals).
 * @param Range The sub-range of entities/components to operate on.
 * @param Time The tick time for the system group.
 */
typedef void (*ECSSystemUpdateCallback)(ECSContext *Context, ECSArchetype *Archetype, const size_t *ArchetypeComponentIndexes, const size_t *ComponentOffsets, ECSRange Range, ECSTime Time);

typedef struct {
    ECSSystemUpdateCallback callback;
    size_t offset;
    size_t size;
} ECSSystemUpdate;

#define ECS_SYSTEM_UPDATE(update) (ECSSystemUpdate){ .callback = (update), .offset = 0, .size = 0 }
#define ECS_SYSTEM_UPDATE_PARALLEL(update) ECS_SYSTEM_UPDATE_PARALLEL_ARCHETYPE_CHUNK(update, SIZE_MAX)
#define ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(update, arrayOffset, chunkSize) (ECSSystemUpdate){ .callback = (update), .offset = (arrayOffset), .size = (chunkSize) }
#define ECS_SYSTEM_UPDATE_PARALLEL_ARCHETYPE_CHUNK(update, chunkSize) ECS_SYSTEM_UPDATE_PARALLEL_CHUNK(update, 1, chunkSize)

#define ECS_SYSTEM_UPDATE_GET_UPDATE(update) (update).callback
#define ECS_SYSTEM_UPDATE_GET_PARALLEL(update) (_Bool)(update).offset
#define ECS_SYSTEM_UPDATE_GET_PARALLEL_ARCHETYPE(update) (ECS_SYSTEM_UPDATE_GET_PARALLEL_OFFSET(update) == 1)
#define ECS_SYSTEM_UPDATE_GET_PARALLEL_OFFSET(update) (update).offset
#define ECS_SYSTEM_UPDATE_GET_PARALLEL_CHUNK_SIZE(update) (update).size

typedef struct {
    size_t index;
    size_t count;
} ECSSystemRange;

typedef struct {
    size_t group;
    size_t priority;
} ECSGroupDependency;

typedef struct {
    ECSTime freq;
    _Bool dynamic;
    struct {
        size_t count;
        const ECSGroupDependency *deps;
        struct {
            const ECSSystemRange *range;
            const uint8_t *graphs;
            const ECSSystemUpdate *update;
            const ECSSystemAccess *access;
        } systems;
    } priorities;
} ECSGroup;

typedef struct {
    size_t executing;
    uint8_t *state;
    ECSTime time;
    size_t running;
} ECSExecutionGroup;

/*!
 * @brief Worker thread ID;
 */
typedef uintptr_t ECSWorkerID;

/*!
 * @brief A callback a waiting worker calls.
 * @param ID The unique ID (0..n) of the worker thread that called it. Or -1 when the tick calls it.
 */
typedef void (*ECSWaitingCallback)(ECSWorkerID ID);

/*!
 * @brief The shared memory zone used for some allocations by the ECS.
 */
extern CCMemoryZone ECSSharedZone;

/*!
 * @brief The block size of the @b ECSSharedZone.
 * @description This should be at least as big as the largest component size. By default it is set to 1MB.
 * @note If you wish to change the size you must do so prior to calling @b ECSInit.
 */
extern size_t ECSSharedMemorySize;

/*!
 * @brief Set the callback to be used by waiting worker threads.
 * @warning This must be set prior to any calls to @b ECSWorkerCreate or @b ECSTick.
 */
extern ECSWaitingCallback ECSWaiting;

/*!
 * @brief Set the component IDs.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy.
 */
extern const ECSComponentID *ECSComponentIDs;

/*!
 * @brief Set the archetype component indexes.
 * @warning This must be set prior to any calls to @b ECSWorkerCreate or @b ECSTick.
 */
extern const size_t *ECSArchetypeComponentIndexes;

/*!
 * @brief Set the archetype component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSArchetypeComponentSizes;

/*!
 * @brief Set the packed component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSPackedComponentSizes;

/*!
 * @brief Set the indexed component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSIndexedComponentSizes;

/*!
 * @brief Set the local component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSLocalComponentSizes;

/*!
 * @brief Set the duplicate archetype component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSDuplicateArchetypeComponentSizes;

/*!
 * @brief Set the duplicate packed component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSDuplicatePackedComponentSizes;

/*!
 * @brief Set the duplicate indexed component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSDuplicateIndexedComponentSizes;

/*!
 * @brief Set the duplicate local component sizes.
 * @warning This must be set prior to any calls to add/remove components.
 */
extern const size_t *ECSDuplicateLocalComponentSizes;

/*!
 * @brief Set the archetype component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSArchetypeComponentDestructors;

/*!
 * @brief Set the packed component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSPackedComponentDestructors;

/*!
 * @brief Set the indexed component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSIndexedComponentDestructors;

/*!
 * @brief Set the local component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSLocalComponentDestructors;

/*!
 * @brief Set the duplicate archetype component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSDuplicateArchetypeComponentDestructors;

/*!
 * @brief Set the duplicate packed component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSDuplicatePackedComponentDestructors;

/*!
 * @brief Set the duplicate indexed component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSDuplicateIndexedComponentDestructors;

/*!
 * @brief Set the duplicate local component destructors.
 * @warning This must be set prior to any calls to @b ECSEntityDestroy or component removal.
 */
extern const ECSComponentDestructor *ECSDuplicateLocalComponentDestructors;

/*!
 * @brief Create an ECS worker thread.
 * @description Can create up to @b ECS_WORKER_THREAD_MAX worker threads. The default is 128, if this limit needs to be changed @b ECS_WORKER_THREAD_MAX
 *              should be set to the new limit when compiling.
 *
 * @warning This should only be called from the same thread @b ECSTick is called from.
 * @return Returns TRUE if a worker thread was successfully created, otherwise returns FALSE if it was not.
 */
_Bool ECSWorkerCreate(void);

/*!
 * @brief Initialise the ECS before use.
 */
void ECSInit(void);

/*!
 * @brief Update the ECS.
 * @param Context The context to be used for the update tick.
 * @param Groups The system groups to be used for the update tick.
 * @param GroupCount The number of system groups.
 * @param State The array of execution state (size of @b GroupCount) to be used by the ECS. The @b state field should be at least as big as the maximum number
 *              of systems in the largest priority of any group.
 *
 * @param DeltaTime The delta time of the tick.
 */
void ECSTick(ECSContext *Context, const ECSGroup *Groups, size_t GroupCount, ECSExecutionGroup *State, ECSTime DeltaTime);

/*!
 * @brief Add an archetype component.
 * @note Should typically use @b ECSEntityAddComponent or @b ECSEntityAddComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to add a component to.
 * @param Data The data to initialise the component with. If NULL the component data will be uninitialised.
 * @param ID The component ID of an archetype component.
 */
void ECSArchetypeAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);

/*!
 * @brief Remove an archetype component from an entity.
 * @note Should typically use @b ECSEntityRemoveComponent or @b ECSEntityRemoveComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to remove a component from.
 * @param ID The component ID of the archetype component to be removed.
 */
void ECSArchetypeRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Get the archetype index of an archetype component.
 * @param ID The component ID of the archetype component to get the index of.
 * @return Returns the index of the archetype component.
 */
size_t ECSArchetypeComponentIndex(ECSComponentRefs *Refs, ECSComponentID ID);

/*!
 * @brief Add a packed component.
 * @note Should typically use @b ECSEntityAddComponent or @b ECSEntityAddComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to add a component to.
 * @param Data The data to initialise the component with. If NULL the component data will be uninitialised.
 * @param ID The component ID of a packed component.
 */
void ECSPackedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);

/*!
 * @brief Remove a packed component from an entity.
 * @note Should typically use @b ECSEntityRemoveComponent or @b ECSEntityRemoveComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to remove a component from.
 * @param ID The component ID of the packed component to be removed.
 */
void ECSPackedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Add an indexed component.
 * @note Should typically use @b ECSEntityAddComponent or @b ECSEntityAddComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to add a component to.
 * @param Data The data to initialise the component with. If NULL the component data will be uninitialised.
 * @param ID The component ID of an indexed component.
 */
void ECSIndexedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);

/*!
 * @brief Remove an indexed component from an entity.
 * @note Should typically use @b ECSEntityRemoveComponent or @b ECSEntityRemoveComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to remove a component from.
 * @param ID The component ID of the indexed component to be removed.
 */
void ECSIndexedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Add a local component.
 * @note Should typically use @b ECSEntityAddComponent or @b ECSEntityAddComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to add a component to.
 * @param Data The data to initialise the component with. If NULL the component data will be uninitialised.
 * @param ID The component ID of a local component.
 */
void ECSLocalAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);

/*!
 * @brief Remove a local component from an entity.
 * @note Should typically use @b ECSEntityRemoveComponent or @b ECSEntityRemoveComponents instead.
 * @param Context The context to be used.
 * @param Entity The entity to remove a component from.
 * @param ID The component ID of the local component to be removed.
 */
void ECSLocalRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Get the index of a local component.
 * @param ID The component ID of the local component to get the index for.
 * @return Returns the index of the local component.
 */
static inline size_t ECSLocalComponentIndex(ECSComponentID ID) CC_CONSTANT_FUNCTION;

/*!
 * @brief Get the offset of a local component data.
 * @param ID The component ID of the local component to get the offset for.
 * @return Returns the offset of the local component.
 */
static inline ptrdiff_t ECSLocalComponentOffset(ECSComponentID ID) CC_CONSTANT_FUNCTION;

/*!
 * @brief Create entities.
 * @param Context The context to be used.
 * @param Entities A pointer to where the entities should be stored.
 * @param Count The number of entities to create.
 */
void ECSEntityCreate(ECSContext *Context, ECSEntity *Entities, size_t Count);

/*!
 * @brief Destroy entities.
 * @param Context The context to be used.
 * @param Entities A pointer to the entities to be destroyed.
 * @param Count The number of entities to destroy.
 */
void ECSEntityDestroy(ECSContext *Context, ECSEntity *Entities, size_t Count);

/*!
 * @brief Add a component to an entity.
 * @param Context The context to be used.
 * @param Entity The entity to add a component to.
 * @param Data The data to initialise the component with. If NULL the component data will be uninitialised.
 * @param ID The component ID.
 */
static inline void ECSEntityAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);

/*!
 * @brief Remove a component from an entity.
 * @param Context The context to be used.
 * @param Entity The entity to remove a component from.
 * @param ID The component ID of the component to be removed.
 */
static inline void ECSEntityRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Add multiple components to an entity.
 * @param Context The context to be used.
 * @param Entity The entity to add the components to.
 * @param Components The type component data for the components to be added.
 * @param Count The number of components to add.
 */
void ECSEntityAddComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t Count);

/*!
 * @brief Remove multiple components from an entity.
 * @param Context The context to be used.
 * @param Entity The entity to remove the components from.
 * @param IDs The component IDs of the components to be removed.
 * @param Count The number of components to be removed.
 */
void ECSEntityRemoveComponents(ECSContext *Context, ECSEntity Entity, ECSComponentID *IDs, size_t Count);

/*!
 * @brief Check whether an entity has a component.
 * @note This function can be safely called on destroyed entity references where it will always return FALSE.
 * @param Context The context to be used.
 * @param Entity The entity to check.
 * @param ID The component ID of the component to check the entity for.
 * @return Returns whether the entity has the specified component (TRUE) or not (FALSE).
 */
static inline _Bool ECSEntityHasComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Get the component data from an entity.
 * @note This function can be safely called on destroyed entity references where it will always return NULL.
 * @param Context The context to be used.
 * @param Entity The entity to get the component data for.
 * @param ID The component ID of the component to retrieve.
 * @return Returns a pointer to the requested component data or NULL if there is none.
 */
static inline void *ECSEntityGetComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

/*!
 * @brief Get the component data for all of the components for an entity.
 * @note This function can be safely called on destroyed entity references where it won't retrieve any components.
 * @param Context The context to be used.
 * @param Entity The entity to get the component data for.
 * @param Components A pointer to an array (of size @b Count) where the typed component data should be stored. This may be NULL to not retrieve any data.
 * @param Count A pointer to the number of components to retrieve, it will also be set to the number of components retrieved. Pass @b SIZE_MAX to retrieve
 *              every component (or if @b Components is NULL to retrieve the number of components).
 */
void ECSEntityGetComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t *Count);

/*!
 * @brief Add multiple duplicate components to an entity.
 * @param Context The context to be used.
 * @param Entity The entity to add the duplicate components to.
 * @param Data The array of component data (of size @b Count).
 * @param ID The component ID of the duplicate component.
 * @param Count The number of duplicate components to add.
 */
static inline void ECSEntityAddDuplicateComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID, size_t Count);

/*!
 * @brief Remove multiple duplicate components from an entity.
 * @param Context The context to be used.
 * @param Entity The entity to remove the duplicate components from.
 * @param ID The component ID of the duplicate component.
 * @param Index The index in the duplicate component array of the first component to be removed. Reverse indexation is also possible where -1 = starts from the
 *              last component and removes @b Count components towards 0.
 * @param Count The number of duplicate components to remove.
 */
static inline void ECSEntityRemoveDuplicateComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID, ptrdiff_t Index, size_t Count);

/*!
 * @brief Get the component base index.
 * @description The base index is the flat index range of all the components, rather than the type related index.
 * @param ID The component ID to get the base index for.
 * @return Returns the base index for the component.
 */
static inline size_t ECSComponentBaseIndex(ECSComponentID ID) CC_CONSTANT_FUNCTION;

/*!
 * @brief Store some data in the shared memory zone.
 * @note This should only be called from the same thread that is also executing other ECS functions. And will automatically be deallocated by the ECS if the data
 *       is stored during a callback (such as an @b ECSComponentDestructor callback).
 *
 * @param Data A pointer to the data to be copied. Use @b CCMemoryZoneAllocate(ECSSharedZone, @b Size) instead when you want uninitialised memory.
 * @param Size The size of the data to be copied.
 * @return Returns a pointer to the stored data.
 */
static inline void *ECSSharedZoneStore(void *Data, size_t Size);

/*!
 * @brief The destructor for duplicate components.
 * @param Data The data for the duplicate component.
 * @param ID The ID of the duplicate component.
 */
void ECSDuplicateDestructor(void *Data, ECSComponentID ID);

#ifndef ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(index, count) 16
#endif

#ifndef ECS_PACKED_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#ifndef ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#ifndef ECS_DUPLICATE_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_DUPLICATE_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#ifndef ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#ifndef ECS_DUPLICATE_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_DUPLICATE_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#ifndef ECS_DUPLICATE_LOCAL_COMPONENT_ARRAY_CHUNK_SIZE
#define ECS_DUPLICATE_LOCAL_COMPONENT_ARRAY_CHUNK_SIZE(index) 16
#endif

#pragma mark -

static inline CC_CONSTANT_FUNCTION size_t ECSComponentBaseIndex(ECSComponentID ID)
{
    size_t Index = ID & ~ECSComponentStorageMask;
    
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeLocal:
            Index &= CCBitMaskForValue(ECS_LOCAL_COMPONENT_MAX);
            Index += ECS_INDEXED_COMPONENT_MAX;
        case ECSComponentStorageTypeIndexed:
            Index += ECS_PACKED_COMPONENT_MAX;
        case ECSComponentStorageTypePacked:
            Index += ECS_ARCHETYPE_COMPONENT_MAX;
        case ECSComponentStorageTypeArchetype:
            return Index;
    }
    
    CCAssertLog(0, "Unsupported component type");
    
    return SIZE_MAX;
}

static inline void *ECSSharedZoneStore(void *Data, size_t Size)
{
    CCAssertLog(Data, "Data must not be null");
    
    void *Ptr = CCMemoryZoneAllocate(ECSSharedZone, Size);
    memcpy(Ptr, Data, Size);
    
    return Ptr;
}

static inline CC_CONSTANT_FUNCTION size_t ECSLocalComponentIndex(ECSComponentID ID)
{
    CCAssertLog((ID & ECSComponentStorageTypeMask) == ECSComponentStorageTypeLocal, "ID must be a local storage type");
    
    return ID & CCBitMaskForValue(ECS_LOCAL_COMPONENT_MAX);
}

static inline CC_CONSTANT_FUNCTION ptrdiff_t ECSLocalComponentOffset(ECSComponentID ID)
{
    CCAssertLog((ID & ECSComponentStorageTypeMask) == ECSComponentStorageTypeLocal, "ID must be a local storage type");
    
    return (ID & ~ECSComponentStorageMask) >> CCBitCountSet(CCBitMaskForValue(ECS_LOCAL_COMPONENT_MAX));
}

static inline void ECSEntityAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    switch (ID & (ECSComponentStorageTypeMask | ECSComponentStorageModifierDuplicate))
    {
        case ECSComponentStorageTypeArchetype:
            ECSArchetypeAddComponent(Context, Entity, Data, ID);
            break;
            
        case ECSComponentStorageTypePacked:
            ECSPackedAddComponent(Context, Entity, Data, ID);
            break;
            
        case ECSComponentStorageTypeIndexed:
            ECSIndexedAddComponent(Context, Entity, Data, ID);
            break;
            
        case ECSComponentStorageTypeLocal:
            ECSLocalAddComponent(Context, Entity, Data, ID);
            break;
            
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeArchetype:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypePacked:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeIndexed:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeLocal:
            ECSEntityAddDuplicateComponent(Context, Entity, Data, ID, 1);
            break;
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
}

static inline void ECSEntityRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    switch (ID & (ECSComponentStorageTypeMask | ECSComponentStorageModifierDuplicate))
    {
        case ECSComponentStorageTypeArchetype:
            ECSArchetypeRemoveComponent(Context, Entity, ID);
            break;
            
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

static inline _Bool ECSEntityHasComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    return CCBitsGet(Refs->has, ECSComponentBaseIndex(ID));
}

static inline void *ECSEntityGetComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
            return ECSEntityHasComponent(Context, Entity, ID) ? CCArrayGetElementAtIndex(Refs->archetype.ptr->components[ECSArchetypeComponentIndex(Refs, ID & ~ECSComponentStorageMask)], Refs->archetype.index) : NULL;
            
        case ECSComponentStorageTypePacked:
        {
            const size_t Index = (ID & ~ECSComponentStorageMask);
            return ECSEntityHasComponent(Context, Entity, ID) ? CCArrayGetElementAtIndex(*Context->packed[Index].components, Refs->packed.indexes[Index]) : NULL;
        }
            
        case ECSComponentStorageTypeIndexed:
        {
            const size_t Index = (ID & ~ECSComponentStorageMask);
            return ECSEntityHasComponent(Context, Entity, ID) ? CCArrayGetElementAtIndex(Context->indexed[Index], Entity) : NULL;
        }
            
        case ECSComponentStorageTypeLocal:
        {
            const ptrdiff_t Offset = ECSLocalComponentOffset(ID);
            return ECSEntityHasComponent(Context, Entity, ID) ? &Refs->local[Offset] : NULL;
        }
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
    
    return NULL;
}

static inline void ECSEntityAddDuplicateComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ID & ECSComponentStorageModifierDuplicate)
    {
        CCArray *Component = ECSEntityGetComponent(Context, Entity, ID);
        CCArray Duplicates;
        
        if (!Component)
        {
            Component = &Duplicates;
            
            size_t Index = (ID & ~ECSComponentStorageMask);
            
            switch (ID & ECSComponentStorageTypeMask)
            {
                case ECSComponentStorageTypeArchetype:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateArchetypeComponentSizes[Index], ECS_DUPLICATE_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(Index));
                    ECSArchetypeAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                case ECSComponentStorageTypePacked:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicatePackedComponentSizes[Index], ECS_DUPLICATE_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(Index));
                    ECSPackedAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                case ECSComponentStorageTypeIndexed:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateIndexedComponentSizes[Index], ECS_DUPLICATE_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(Index));
                    ECSIndexedAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                case ECSComponentStorageTypeLocal:
                    Index = ECSLocalComponentIndex(ID);
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateLocalComponentSizes[Index], ECS_DUPLICATE_LOCAL_COMPONENT_ARRAY_CHUNK_SIZE(Index));
                    ECSLocalAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported component type");
                    break;
            }
        }
        
        else Duplicates = *Component;
        
        CCArrayAppendElements(Duplicates, Data, Count);
    }
    
    else CCAssertLog(0, "Component does not allow duplicates");
}

static inline void ECSEntityRemoveDuplicateComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID, ptrdiff_t Index, size_t Count)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (ID & ECSComponentStorageModifierDuplicate)
    {
        ECSComponentDestructor Destructor = NULL;
        
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
        size_t CopiedComponentCount = 0;
        void **CopiedComponents;
#endif
        
        if (ID & ECSComponentStorageModifierDestructor)
        {
            switch (ID & ECSComponentStorageTypeMask)
            {
                case ECSComponentStorageTypeArchetype:
                    Destructor = ECSDuplicateArchetypeComponentDestructors[ID & ~ECSComponentStorageMask];
                    break;
                    
                case ECSComponentStorageTypePacked:
                    Destructor = ECSDuplicatePackedComponentDestructors[ID & ~ECSComponentStorageMask];
                    break;
                    
                case ECSComponentStorageTypeIndexed:
                    Destructor = ECSDuplicateIndexedComponentDestructors[ID & ~ECSComponentStorageMask];
                    break;
                    
                case ECSComponentStorageTypeLocal:
                    Destructor = ECSDuplicateLocalComponentDestructors[ECSLocalComponentIndex(ID)];
                    break;
                    
                default:
                    CCAssertLog(0, "Unsupported component type");
                    break;
                    
            }
            
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
            CCMemoryZoneSave(ECSSharedZone);
#endif
        }
        
        CCArray *Component = ECSEntityGetComponent(Context, Entity, ID);
        
        if (Component)
        {
            CCArray Duplicates = *Component;
            const size_t DuplicatesCount = CCArrayGetCount(Duplicates);
            
            if (Index < 0)
            {
                Index = (DuplicatesCount + Index) - (Count - 1);
                
                if (Index < 0)
                {
                    Count += Index;
                    Index = 0;
                }
            }
            
            if (Index < DuplicatesCount)
            {
                if ((Index + Count) >= DuplicatesCount)
                {
                    if (Index)
                    {
                        const size_t ElementCount = DuplicatesCount - Index;
                        
                        if (ID & ECSComponentStorageModifierDestructor)
                        {
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
                            for (size_t Loop = 0; Loop < ElementCount; Loop++) Destructor(CCArrayGetElementAtIndex(Duplicates, Loop + Index), ID);
#else
                            CopiedComponents = CCMemoryZoneAllocate(ECSSharedZone, sizeof(*CopiedComponents) * ElementCount);
                            const size_t DuplicateComponentSize = CCArrayGetElementSize(Duplicates);
                            for (size_t Loop = 0; Loop < ElementCount; Loop++) CopiedComponents[CopiedComponentCount++] = ECSSharedZoneStore(CCArrayGetElementAtIndex(Duplicates, Loop + Index), DuplicateComponentSize); // TODO: don't loop and copy entire list of components at once
#endif
                        }
                        
                        CCArrayRemoveElementsAtIndex(Duplicates, Index, ElementCount);
                    }
                    
                    else
                    {
                        if (ID & ECSComponentStorageModifierDestructor)
                        {
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
                            for (size_t Loop = 0; Loop < DuplicatesCount; Loop++) Destructor(CCArrayGetElementAtIndex(Duplicates, Loop), ID);
#else
                            CopiedComponents = CCMemoryZoneAllocate(ECSSharedZone, sizeof(*CopiedComponents) * DuplicatesCount);
                            const size_t DuplicateComponentSize = CCArrayGetElementSize(Duplicates);
                            for (size_t Loop = 0; Loop < DuplicatesCount; Loop++) CopiedComponents[CopiedComponentCount++] = ECSSharedZoneStore(CCArrayGetElementAtIndex(Duplicates, Loop), DuplicateComponentSize); // TODO: don't loop and copy entire list of components at once
#endif
                            
                            CCArrayDestroy(Duplicates);
                        }
                        
                        switch (ID & ECSComponentStorageTypeMask)
                        {
                            case ECSComponentStorageTypeArchetype:
                                ECSArchetypeRemoveComponent(Context, Entity, ID);
                                break;
                                
                            case ECSComponentStorageTypePacked:
                                ECSPackedRemoveComponent(Context, Entity, ID);
                                break;
                                
                            case ECSComponentStorageTypeIndexed:
                                ECSIndexedRemoveComponent(Context, Entity, ID);
                                break;
                                
                            case ECSComponentStorageTypeLocal:
                                ECSLocalRemoveComponent(Context, Entity, ID);
                                break;
                                
                            default:
                                CCAssertLog(0, "Unsupported component type");
                                break;
                        }
                    }
                }
                
                else
                {
                    size_t CopyIndex = Index + Count;
                    size_t CopyCount = DuplicatesCount - CopyIndex;
                    
                    if (CopyCount > Count)
                    {
                        CopyIndex += CopyCount - Count;
                        CopyCount = Count;
                    }
                    
                    if (ID & ECSComponentStorageModifierDestructor)
                    {
#if ECS_UNSAFE_COMPONENT_DESTRUCTION
                        for (size_t Loop = 0; Loop < Count; Loop++) Destructor(CCArrayGetElementAtIndex(Duplicates, Loop + Index), ID);
#else
                        CopiedComponents = CCMemoryZoneAllocate(ECSSharedZone, sizeof(*CopiedComponents) * Count);
                        const size_t DuplicateComponentSize = CCArrayGetElementSize(Duplicates);
                        for (size_t Loop = 0; Loop < Count; Loop++) CopiedComponents[CopiedComponentCount++] = ECSSharedZoneStore(CCArrayGetElementAtIndex(Duplicates, Loop + Index), DuplicateComponentSize); // TODO: don't loop and copy entire list of components at once
#endif
                    }
                    
                    CCArrayCopyElementsAtIndex(Duplicates, CopyIndex, Index, CopyCount);
                    CCArrayRemoveElementsAtIndex(Duplicates, DuplicatesCount - Count, Count);
                }
            }
        }
        
#if !ECS_UNSAFE_COMPONENT_DESTRUCTION
        if (ID & ECSComponentStorageModifierDestructor)
        {
            for (size_t Loop = 0; Loop < CopiedComponentCount; Loop++)
            {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
                Destructor(CopiedComponents[Loop], ID);
#pragma clang diagnostic pop
            }
            
            CCMemoryZoneRestore(ECSSharedZone);
        }
#endif
    }
    
    else CCAssertLog(0, "Component does not allow duplicates");
}

#endif
