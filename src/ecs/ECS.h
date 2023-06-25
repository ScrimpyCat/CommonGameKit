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

typedef void (*ECSSystemUpdateCallback)(ECSContext *Context, ECSArchetype *Archetype, const size_t *ArchetypeComponentIndexes, const size_t *ComponentOffsets, void *Changes, ECSTime Time);

#if ECS_SYSTEM_UPDATE_TAGGED_POINTER
typedef ECSSystemUpdateCallback ECSSystemUpdate;

#define ECS_SYSTEM_UPDATE(update) update
#define ECS_SYSTEM_UPDATE_PARALLEL(update) ((ECSSystemUpdate)((uintptr_t)(update) + 1))

#define ECS_SYSTEM_UPDATE_GET_UPDATE(update) ((ECSSystemUpdateCallback)((uintptr_t)(update) & ~1))
#define ECS_SYSTEM_UPDATE_GET_PARALLEL(update) ((_Bool)((uintptr_t)(update) & 1))
#else
typedef struct {
    ECSSystemUpdateCallback callback;
    _Bool parallel;
} ECSSystemUpdate;

#define ECS_SYSTEM_UPDATE(update) (ECSSystemUpdate){ (update), FALSE }
#define ECS_SYSTEM_UPDATE_PARALLEL(update) (ECSSystemUpdate){ (update), TRUE }

#define ECS_SYSTEM_UPDATE_GET_UPDATE(update) update.callback
#define ECS_SYSTEM_UPDATE_GET_PARALLEL(update) update.parallel
#endif

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
 * @brief Set the callback to be used by waiting worker threads.
 * @warning This must be set prior to any calls to @b ECSWorkerCreate or @b ECSTick.
 */
extern ECSWaitingCallback ECSWaiting;

/*!
 * @brief Set the archetype component indexes.
 * @warning This must be set prior to any calls to @b ECSWorkerCreate or @b ECSTick.
 */
extern const size_t *ECSArchetypeComponentIndexes;

/*!
 * @brief Set the archetype component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSArchetypeComponentSizes;

/*!
 * @brief Set the packed component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSPackedComponentSizes;

/*!
 * @brief Set the indexed component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSIndexedComponentSizes;

/*!
 * @brief Set the duplicate archetype component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSDuplicateArchetypeComponentSizes;

/*!
 * @brief Set the duplicate packed component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSDuplicatePackedComponentSizes;

/*!
 * @brief Set the duplicate indexed component sizes.
 * @warning This must be set prior to any calls to @b ECSEntityCreate.
 */
extern const size_t *ECSDuplicateIndexedComponentSizes;

/*!
 * @brief Create an ECS worker thread.
 * @description Can create up to @b ECS_WORKER_THREAD_MAX worker threads. The default is 128, if this limit needs to be changed @b ECS_WORKER_THREAD_MAX
 *              should be set to the new limit when compiling.
 *
 * @warning This should only be called from the same thread @b ECSTick is called from.
 * @return Returns TRUE if a worker thread was successfully created, otherwise returns FALSE if it was not.
 */
_Bool ECSWorkerCreate(void);

void ECSInit(void);

void ECSTick(ECSContext *Context, const ECSGroup *Groups, size_t GroupCount, ECSExecutionGroup *State, ECSTime DeltaTime);

void ECSArchetypeAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);
void ECSArchetypeRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);
size_t ECSArchetypeComponentIndex(ECSComponentRefs *Refs, ECSComponentID ID);

void ECSPackedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);
void ECSPackedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

void ECSIndexedAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);
void ECSIndexedRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);

void ECSEntityCreate(ECSContext *Context, ECSEntity *Entities, size_t Count);
static inline void ECSEntityAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID);
static inline void ECSEntityRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);
void ECSEntityAddComponents(ECSContext *Context, ECSEntity Entity, ECSTypedComponent *Components, size_t Count);
void ECSEntityRemoveComponents(ECSContext *Context, ECSEntity Entity, ECSComponentID *IDs, size_t Count);
static inline _Bool ECSEntityHasComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);
static inline void *ECSEntityGetComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID);
static inline void ECSEntityAddDuplicateComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID, size_t Count);
static inline void ECSEntityRemoveDuplicateComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID, size_t Index, size_t Count);

#pragma mark -

static inline void ECSEntityAddComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID)
{
    switch (ID & (ECSComponentStorageMask ^ ECSComponentStorageModifierTag))
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
            
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeArchetype:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypePacked:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeIndexed:
            ECSEntityAddDuplicateComponent(Context, Entity, Data, ID, 1);
            break;
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
}

static inline void ECSEntityRemoveComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    switch (ID & (ECSComponentStorageMask ^ ECSComponentStorageModifierTag))
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
            
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeArchetype:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypePacked:
        case ECSComponentStorageModifierDuplicate | ECSComponentStorageTypeIndexed:
            ECSEntityRemoveDuplicateComponent(Context, Entity, ID, 0, 1);
            break;
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
}

static inline _Bool ECSEntityHasComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
        {
            _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
            return CCBitsGet(Refs->archetype.has, ID);
        }
            
        case ECSComponentStorageTypePacked:
            return CCBitsGet(Refs->packed.has, (ID & ~ECSComponentStorageMask));
            
        case ECSComponentStorageTypeIndexed:
            return CCBitsGet(Refs->indexed.has, (ID & ~ECSComponentStorageMask));
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
    
    return FALSE;
}

static inline void *ECSEntityGetComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID)
{
    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
    
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
        {
            _Static_assert(ECSComponentStorageTypeArchetype == 0, "Expects archetype storage type to be 0");
            return ECSEntityHasComponent(Context, Entity, ID) ? CCArrayGetElementAtIndex(Refs->archetype.ptr->components[ECSArchetypeComponentIndex(Refs, ID)], Refs->archetype.index) : NULL;
        }
            
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
            
        default:
            CCAssertLog(0, "Unsupported component type");
            break;
    }
    
    return NULL;
}

static inline void ECSEntityAddDuplicateComponent(ECSContext *Context, ECSEntity Entity, void *Data, ECSComponentID ID, size_t Count)
{
    if (ID & ECSComponentStorageModifierDuplicate)
    {
        CCArray *Component = ECSEntityGetComponent(Context, Entity, ID);
        CCArray Duplicates;
        
        if (!Component)
        {
            Component = &Duplicates;
            
            const size_t Index = (ID & ~ECSComponentStorageMask);
            
            switch (ID & ECSComponentStorageTypeMask)
            {
                case ECSComponentStorageTypeArchetype:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateArchetypeComponentSizes[Index], 16); // TODO: replace 16 with configurable amount
                    ECSArchetypeAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                case ECSComponentStorageTypePacked:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicatePackedComponentSizes[Index], 16); // TODO: replace 16 with configurable amount
                    ECSPackedAddComponent(Context, Entity, &Duplicates, ID);
                    break;
                    
                case ECSComponentStorageTypeIndexed:
                    Duplicates = CCArrayCreate(CC_STD_ALLOCATOR, ECSDuplicateIndexedComponentSizes[Index], 16); // TODO: replace 16 with configurable amount
                    ECSIndexedAddComponent(Context, Entity, &Duplicates, ID);
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

static inline void ECSEntityRemoveDuplicateComponent(ECSContext *Context, ECSEntity Entity, ECSComponentID ID, size_t Index, size_t Count)
{
    if (ID & ECSComponentStorageModifierDuplicate)
    {
        CCArray *Component = ECSEntityGetComponent(Context, Entity, ID);
        
        if (Component)
        {
            CCArray Duplicates = *Component;
            const size_t DuplicatesCount = CCArrayGetCount(Duplicates);
            
            if (Index < DuplicatesCount)
            {
                if ((Index + Count) >= DuplicatesCount)
                {
                    if (Index) CCArrayRemoveElementsAtIndex(Duplicates, Index, DuplicatesCount - Index);
                    else
                    {
                        CCArrayDestroy(Duplicates);
                        
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
                    
                    CCArrayCopyElementsAtIndex(Duplicates, CopyIndex, Index, CopyCount);
                    CCArrayRemoveElementsAtIndex(Duplicates, Index + CopyCount, Count - CopyCount);
                }
            }
        }
    }
    
    else CCAssertLog(0, "Component does not allow duplicates");
}

#endif
