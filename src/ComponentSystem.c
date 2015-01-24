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
        CCComponentList active, added, removed, destroy;
        atomic_flag addedLock, removedLock;
    } components;
} CCComponentSystem;

static CCComponentSystem *Systems[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];
static size_t SystemsCount[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];
static double ElapsedTime[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];

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
            .active = NULL,
            .added = NULL,
            .removed = NULL,
            .destroy = NULL,
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
            if (Systems[ExecutionType][Loop].components.active) CCLinkedListDestroy((CCLinkedList)Systems[ExecutionType][Loop].components.active);
            if (Systems[ExecutionType][Loop].components.added) CCLinkedListDestroy((CCLinkedList)Systems[ExecutionType][Loop].components.added);
            if (Systems[ExecutionType][Loop].components.removed) CCLinkedListDestroy((CCLinkedList)Systems[ExecutionType][Loop].components.removed);
            if (Systems[ExecutionType][Loop].components.destroy) CCLinkedListDestroy((CCLinkedList)Systems[ExecutionType][Loop].components.destroy);
            
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
            CCComponentList *ActiveComponents = &Systems[ExecutionType][Loop].components.active;
            
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
        CCLinkedListNode *Node = CCLinkedListCreateNode(CC_STD_ALLOCATOR, sizeof(CCComponent), &Component);
        
        if (System->addingComponent)
        {
            System->addingComponent(Component);
            
            if (System->lock) System->lock();
            System->components.active = System->components.active ? (CCComponentList)CCLinkedListInsert((CCLinkedList)System->components.active , Node) : (CCComponentList)Node;
            if (System->unlock) System->unlock();
        }
        
        else
        {
            while (!atomic_flag_test_and_set(&System->components.addedLock));
            System->components.added = System->components.added ? (CCComponentList)CCLinkedListInsert((CCLinkedList)System->components.added, Node) : (CCComponentList)Node;
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
            
            CCComponentList List = System->components.active;
            do
            {
                if (List->component == Component) break;
            } while ((List = CCComponentListEnumerateNext(List)));
            
            if (List) CCLinkedListDestroyNode((CCLinkedListNode*)List);
            
            if (System->unlock) System->unlock();
        }
        
        else
        {
            CCLinkedListNode *Node = CCLinkedListCreateNode(CC_STD_ALLOCATOR, sizeof(CCComponent), &Component);
            
            while (!atomic_flag_test_and_set(&System->components.removedLock));
            System->components.removed = System->components.removed ? (CCComponentList)CCLinkedListInsert((CCLinkedList)System->components.removed, Node) : (CCComponentList)Node;
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

CCComponentList CCComponentSystemGetComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    return System ? System->components.active : NULL;
}

CCComponentList CCComponentSystemGetAddedComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    CCComponentList List = NULL;
    if ((System) && (System->components.added))
    {
        List = System->components.added;
        
        while (!atomic_flag_test_and_set(&System->components.addedLock));
        System->components.active = System->components.active ? (CCComponentList)CCLinkedListInsert((CCLinkedList)System->components.active, (CCLinkedList)System->components.added) : System->components.added;
        
        System->components.added = NULL;
        atomic_flag_clear(&System->components.addedLock);
    }
    
    return List;
}

CCComponentList CCComponentSystemGetRemovedComponentsForSystem(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    CCComponentList List = NULL;
    if ((System) && (System->components.removed))
    {
        if (System->components.destroy)
        {
            CCLinkedListDestroy((CCLinkedList)System->components.destroy);
            System->components.destroy = NULL;
        }
        
        List = System->components.removed;
        
        if (System->components.active)
        {
            CCComponentList Active = System->components.active, Removed = System->components.removed;
            do
            {
                if (CCComponentListGetCurrentComponent(Active) == CCComponentListGetCurrentComponent(Removed))
                {
                    Removed = CCComponentListEnumerateNext(Removed);
                    
                    CCComponentList Dead = Active;
                    Active = CCComponentListEnumerateNext(Active);
                    CCLinkedListDestroyNode((CCLinkedList)Dead);
                    continue;
                }
            } while ((Active = CCComponentListEnumerateNext(Active)));
        }
        
        System->components.destroy = System->components.removed;
        System->components.removed = NULL;
    }
    
    return List;
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
