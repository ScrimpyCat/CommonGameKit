//
//  ComponentSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 7/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "ComponentSystem.h"
#include <CommonC/Common.h>
#include <stdatomic.h>
#include "GLSetup.h"
#include "ComponentBase.h"


typedef struct {
    CCComponentSystemID id;
    CCComponentSystemExecutionType executionType;
    
    CCComponentSystemUpdateCallback update;
    
    CCComponentSystemHandlesComponentCallback handlesComponent;
    CCComponentSystemAddingComponentCallback addingComponent;
    CCComponentSystemRemovingComponentCallback removingComponent;
    
    CCComponentSystemTryLockCallback tryLock;
    CCComponentSystemLockCallback lock;
    CCComponentSystemUnlockCallback unlock;
    
    struct {
        CCCollection active, added, removed, destroy;
        atomic_flag addedLock, removedLock;
    } components;
} CCComponentSystem;

static CCComponentSystem *Systems[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];
static size_t SystemsCount[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];
static double ElapsedTime[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];

static void CCComponentDestructor(CCCollection Collection, CCComponent *Component)
{
    CCComponentSetIsManaged(*Component, FALSE);
}

void CCComponentSystemRegister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType, CCComponentSystemUpdateCallback Update, CCComponentSystemHandlesComponentCallback HandlesComponent, CCComponentSystemAddingComponentCallback AddingComponent, CCComponentSystemRemovingComponentCallback RemovingComponent, CCComponentSystemTryLockCallback SystemTryLock, CCComponentSystemLockCallback SystemLock, CCComponentSystemUnlockCallback SystemUnlock)
{
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    CC_SAFE_Realloc(Systems[ExecutionType], sizeof(CCComponentSystem) * ++SystemsCount[ExecutionType],
                    CC_LOG_ERROR("Failed to add new system (%" PRIu32 ") due to realloc failing. Allocation size: %zu", id, sizeof(CCComponentSystem) * SystemsCount[ExecutionType]);
                    SystemsCount[ExecutionType]--;
                    return;
                    );
    
    const size_t Index = SystemsCount[ExecutionType] - 1;
    Systems[ExecutionType][Index] = (CCComponentSystem){
        .id = id,
        .executionType = ExecutionType,
        .update = Update,
        .handlesComponent = HandlesComponent,
        .addingComponent = AddingComponent,
        .removingComponent = RemovingComponent,
        .tryLock = SystemTryLock,
        .lock = SystemLock,
        .unlock = SystemUnlock,
        .components = {
            .active = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyEnumerating | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL),
            .added = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL),
            .removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL),
            .destroy = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL),
            .addedLock = ATOMIC_FLAG_INIT,
            .removedLock = ATOMIC_FLAG_INIT
        }
    };
}

void CCComponentSystemDeregister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType)
{
    //Note: Really only needed for unit tests, so doesn't matter if not correct
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    const size_t Count = SystemsCount[ExecutionType];
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        if (Systems[ExecutionType][Loop].id == id)
        {
            if (Systems[ExecutionType][Loop].components.active) CCCollectionDestroy(Systems[ExecutionType][Loop].components.active);
            if (Systems[ExecutionType][Loop].components.added) CCCollectionDestroy(Systems[ExecutionType][Loop].components.added);
            if (Systems[ExecutionType][Loop].components.removed) CCCollectionDestroy(Systems[ExecutionType][Loop].components.removed);
            if (Systems[ExecutionType][Loop].components.destroy) CCCollectionDestroy(Systems[ExecutionType][Loop].components.destroy);
            
            memset(&Systems[ExecutionType][Loop], 0, sizeof(CCComponentSystem));
            break;
        }
    }
}

void CCComponentSystemRun(CCComponentSystemExecutionType ExecutionType)
{
    _Bool TimedUpdate = ExecutionType & CCComponentSystemExecutionOptionTimedUpdate;
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    
    double Delta = 0.0;
    if (TimedUpdate)
    {
        const double CurrentTime = glfwGetTime();
        if (ElapsedTime[ExecutionType] != 0.0) Delta = CurrentTime - ElapsedTime[ExecutionType];
        ElapsedTime[ExecutionType] = CurrentTime;
    }
    
    const size_t Count = SystemsCount[ExecutionType];
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        if (Systems[ExecutionType][Loop].update)
        {
            CCCollection ActiveComponents = Systems[ExecutionType][Loop].components.active;
            
            if (Systems[ExecutionType][Loop].lock) Systems[ExecutionType][Loop].lock();
            if (TimedUpdate) ((CCComponentSystemTimedUpdateCallback)Systems[ExecutionType][Loop].update)(Delta, ActiveComponents);
            else Systems[ExecutionType][Loop].update(NULL, ActiveComponents);
            if (Systems[ExecutionType][Loop].unlock) Systems[ExecutionType][Loop].unlock();
        }
    }
}

static CCComponentSystem *CCComponentSystemHandlesComponentFind(CCComponent Component)
{
    for (size_t Loop = 0; Loop < (CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask); Loop++)
    {
        const size_t Count = SystemsCount[Loop];
        for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
        {
            CCComponentSystem *System = &Systems[Loop][Loop2];
            if ((System->handlesComponent) && (System->handlesComponent(CCComponentGetID(Component)))) return System;
        }
    }
    
    return NULL;
}

void CCComponentSystemAddComponent(CCComponent Component)
{
    CCComponentSystem *System = CCComponentSystemHandlesComponentFind(Component);
    if (System)
    {
        CCComponentSetIsManaged(Component, TRUE);
        if (System->addingComponent)
        {
            System->addingComponent(Component);
            
            if (System->lock) System->lock();
            CCCollectionInsertElement(System->components.active, &Component);
            if (System->unlock) System->unlock();
        }
        
        else
        {
            while (!atomic_flag_test_and_set(&System->components.addedLock));
            CCCollectionInsertElement(System->components.added, &Component);
            atomic_flag_clear(&System->components.addedLock);
        }
    }
}

void CCComponentSystemRemoveComponent(CCComponent Component)
{
    CCComponentSystem *System = CCComponentSystemHandlesComponentFind(Component);
    if (System)
    {
        if (System->removingComponent)
        {
            System->removingComponent(Component);
            
            if (System->lock) System->lock();
            CCCollectionRemoveElement(System->components.active, CCCollectionFindElement(System->components.active, &Component, NULL));
            if (System->unlock) System->unlock();
            CCComponentSetIsManaged(Component, FALSE);
        }
        
        else
        {
            while (!atomic_flag_test_and_set(&System->components.removedLock));
            CCCollectionInsertElement(System->components.removed, &Component);
            atomic_flag_clear(&System->components.removedLock);
        }
    }
}

static CCComponentSystem *CCComponentSystemFind(CCComponentSystemID id)
{
    for (size_t Loop = 0; Loop < (CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask); Loop++)
    {
        const size_t Count = SystemsCount[Loop];
        for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
        {
            if (Systems[Loop][Loop2].id == id) return &Systems[Loop][Loop2];
        }
    }
    
    return NULL;
}

CCCollection CCComponentSystemGetComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    return System->components.active;
}

CCCollection CCComponentSystemGetAddedComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    CCCollection Added = NULL;
    if (System)
    {
        while (!atomic_flag_test_and_set(&System->components.addedLock));
        CCCollectionInsertCollection(System->components.active, System->components.added, NULL); //TODO: make a consumed insert or a retained list?
        
        Added = System->components.added;
        System->components.added = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL);
        
        atomic_flag_clear(&System->components.addedLock);
    }
    
    return Added;
}

CCCollection CCComponentSystemGetRemovedComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    CCCollection Removed = NULL;
    if (System)
    {
        CCCollectionDestroy(System->components.destroy);
        
        while (!atomic_flag_test_and_set(&System->components.removedLock));
        
        System->components.destroy = System->components.removed;
        System->components.removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL);
        
        atomic_flag_clear(&System->components.removedLock);
        
        
        CCCollection Entries = CCCollectionFindCollection(System->components.active, System->components.destroy, NULL);
        CCCollectionRemoveCollection(System->components.active, Entries);
        CCCollectionDestroy(Entries);
        
        Removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), (CCCollectionElementDestructor)CCComponentDestructor);
        CCCollectionInsertCollection(Removed, System->components.destroy, NULL);
    }
    
    return Removed;
}

_Bool CCComponentSystemTryLock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    _Bool Locked = TRUE;
    if ((System) && (System->tryLock)) Locked = System->tryLock();
    
    return Locked;
}

void CCComponentSystemLock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    if ((System) && (System->lock)) System->lock();
}

void CCComponentSystemUnlock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    if ((System) && (System->unlock)) System->unlock();
}
