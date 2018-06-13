/*
 *  Copyright (c) 2015, Stefan Johnson
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
/*
 TODO: Cleanup component system so it's easier to work with.
    * Expose CCComponentSystem instead of CCComponentSystemID
 
    * Let multiple systems handle the same components? (not sure about this, as it
      will may make threading trickier)
 
    * Easily find which systems manage which components, access all systems, systems
      belonging to a particular thread, etc.
 */
#include "ComponentSystem.h"
#include <CommonC/Common.h>
#include <stdatomic.h>
#include "GLSetup.h"
#include "ComponentBase.h"
#include "Callbacks.h"
#include "Message.h"


typedef struct {
    CCComponentSystemHandle handle;
    
    CCComponentSystemUpdateCallback update;
    
    CCConcurrentQueue mailbox;
    CCComponentSystemMessageHandlerCallback messageHandler;
    
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

static CCOrderedCollection Systems[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];
static double ElapsedTime[CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask];

static void CCComponentElementDestructor(CCCollection Collection, CCComponent *Component)
{
    CCComponentSetIsManaged(*Component, FALSE);
}

static void CCComponentSystemDestructor(CCCollection Collection, CCComponentSystem *System)
{
    if (System->components.active) CCCollectionDestroy(System->components.active);
    if (System->components.added) CCCollectionDestroy(System->components.added);
    if (System->components.removed) CCCollectionDestroy(System->components.removed);
    if (System->components.destroy) CCCollectionDestroy(System->components.destroy);
    
    CCConcurrentQueueDestroy(System->mailbox);
}

void CCComponentSystemRegister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType, CCComponentSystemUpdateCallback Update, CCComponentSystemMessageHandlerCallback MessageHandler, CCComponentSystemHandlesComponentCallback HandlesComponent, CCComponentSystemAddingComponentCallback AddingComponent, CCComponentSystemRemovingComponentCallback RemovingComponent, CCComponentSystemTryLockCallback SystemTryLock, CCComponentSystemLockCallback SystemLock, CCComponentSystemUnlockCallback SystemUnlock)
{
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    if (!Systems[ExecutionType]) Systems[ExecutionType] = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall | CCCollectionHintHeavyFinding | CCCollectionHintHeavyEnumerating, sizeof(CCComponentSystem), (CCCollectionElementDestructor)CCComponentSystemDestructor);
    
    CCOrderedCollectionAppendElement(Systems[ExecutionType], &(CCComponentSystem){
        .handle = { .id = id, .executionType = ExecutionType },
        .update = Update,
        .mailbox = CCConcurrentQueueCreate(CC_STD_ALLOCATOR, CCConcurrentGarbageCollectorCreate(CC_STD_ALLOCATOR, CCEpochGarbageCollector)),
        .messageHandler = MessageHandler,
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
    });
}

static CCComparisonResult CCComponentSystemIDComparator(CCComponentSystem *System, CCComponentSystemID *id)
{
    return System->handle.id == *id ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

void CCComponentSystemDeregister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType)
{
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    CCCollectionRemoveElement(Systems[ExecutionType], CCCollectionFindElement(Systems[ExecutionType], &id, (CCComparator)CCComponentSystemIDComparator));
}

static void CCComponentSystemFlushMailbox(CCComponentSystem *System)
{
    //TODO: Should we set a hard limit?
    for (CCConcurrentQueueNode *Node; (Node = CCConcurrentQueuePop(System->mailbox)); CCConcurrentQueueDestroyNode(Node))
    {
        CCMessage *Message = *(CCMessage**)CCConcurrentQueueGetNodeData(Node);
        Message->router->deliver(Message, System->handle.id);
    }
}

void CCComponentSystemRun(CCComponentSystemExecutionType ExecutionType)
{
    _Bool TimedUpdate = ExecutionType & CCComponentSystemExecutionOptionTimedUpdate;
    ExecutionType &= CCComponentSystemExecutionTypeMask;
    
    if (!Systems[ExecutionType]) return;
    
    double Delta = 0.0;
    if (TimedUpdate)
    {
        const double CurrentTime = CCTimestamp();
        if (ElapsedTime[ExecutionType] != 0.0) Delta = CurrentTime - ElapsedTime[ExecutionType];
        ElapsedTime[ExecutionType] = CurrentTime;
    }
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Systems[ExecutionType], &Enumerator);
    for (CCComponentSystem *System = CCCollectionEnumeratorGetCurrent(&Enumerator); System; System = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCCollection ActiveComponents = System->components.active;
        
        if (System->lock) System->lock(&System->handle);
        CCComponentSystemFlushMailbox(System);
        
        if (TimedUpdate) ((CCComponentSystemTimedUpdateCallback)System->update)(&System->handle, Delta, ActiveComponents);
        else System->update(&System->handle, NULL, ActiveComponents);
        if (System->unlock) System->unlock(&System->handle);
    }
}

static CCComparisonResult CCComponentSystemComponentComparator(CCComponentSystem *System, CCComponent *Component)
{
    return (System->handlesComponent) && (System->handlesComponent(&System->handle, CCComponentGetID(*Component))) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCComponentSystem *CCComponentSystemHandlesComponentFind(CCComponent Component)
{
    for (size_t Loop = 0; Loop < (CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask); Loop++)
    {
        if (Systems[Loop])
        {
            CCComponentSystem *System = CCCollectionGetElement(Systems[Loop], CCCollectionFindElement(Systems[Loop], &Component, (CCComparator)CCComponentSystemComponentComparator));
            if (System) return System;
        }
    }
    
    return NULL;
}

static CCComparisonResult CCComponentSystemComponentIDComparator(CCComponentSystem *System, CCComponentID *ComponentID)
{
    return (System->handlesComponent) && (System->handlesComponent(&System->handle, *ComponentID)) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

CCComponentSystemID CCComponentSystemHandlesComponentID(CCComponentID ComponentID)
{
    for (size_t Loop = 0; Loop < (CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask); Loop++)
    {
        if (Systems[Loop])
        {
            CCComponentSystem *System = CCCollectionGetElement(Systems[Loop], CCCollectionFindElement(Systems[Loop], &ComponentID, (CCComparator)CCComponentSystemComponentIDComparator));
            if (System) return System->handle.id;
        }
    }
    
    return 0;
}

void CCComponentSystemAddComponent(CCComponent Component)
{
    CCRetain(Component);
    
    CCComponentSystem *System = CCComponentSystemHandlesComponentFind(Component);
    if (System)
    {
        CCComponentSetIsManaged(Component, TRUE);
        if (System->addingComponent)
        {
            System->addingComponent(&System->handle, Component);
            
            if (System->lock) System->lock(&System->handle);
            CCCollectionInsertElement(System->components.active, &Component);
            if (System->unlock) System->unlock(&System->handle);
        }
        
        else
        {
            while (!atomic_flag_test_and_set(&System->components.addedLock)) CC_SPIN_WAIT();
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
            System->removingComponent(&System->handle, Component);
            
            if (System->lock) System->lock(&System->handle);
            CCCollectionRemoveElement(System->components.active, CCCollectionFindElement(System->components.active, &Component, NULL));
            if (System->unlock) System->unlock(&System->handle);
            CCComponentSetIsManaged(Component, FALSE);
        }
        
        else
        {
            while (!atomic_flag_test_and_set(&System->components.removedLock)) CC_SPIN_WAIT();
            CCCollectionInsertElement(System->components.removed, &Component);
            atomic_flag_clear(&System->components.removedLock);
        }
    }
    
    CCComponentDestroy(Component);
}

static CCComponentSystem *CCComponentSystemFind(CCComponentSystemID id)
{
    for (size_t Loop = 0; Loop < (CCComponentSystemExecutionMax & CCComponentSystemExecutionTypeMask); Loop++)
    {
        if (Systems[Loop])
        {
            CCComponentSystem *System = CCCollectionGetElement(Systems[Loop], CCCollectionFindElement(Systems[Loop], &id, (CCComparator)CCComponentSystemIDComparator));
            if (System) return System;
        }
    }
    
    return NULL;
}

void CCComponentSystemHandleMessage(CCComponentSystemID id, CCMessage *Message)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    if (System)
    {
        if (System->messageHandler) System->messageHandler(&System->handle, Message);
    }
}

CCConcurrentQueue CCComponentSystemGetMailbox(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    return System->mailbox;
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
        while (!atomic_flag_test_and_set(&System->components.addedLock)) CC_SPIN_WAIT();
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
        
        while (!atomic_flag_test_and_set(&System->components.removedLock)) CC_SPIN_WAIT();
        
        System->components.destroy = System->components.removed;
        System->components.removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), NULL);
        
        atomic_flag_clear(&System->components.removedLock);
        
        
        CCCollection Entries = CCCollectionFindCollection(System->components.active, System->components.destroy, NULL);
        CCCollectionRemoveCollection(System->components.active, Entries);
        CCCollectionDestroy(Entries);
        
        Removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCComponent), (CCCollectionElementDestructor)CCComponentElementDestructor);
        CCCollectionInsertCollection(Removed, System->components.destroy, NULL);
    }
    
    return Removed;
}

_Bool CCComponentSystemTryLock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    
    _Bool Locked = TRUE;
    if ((System) && (System->tryLock)) Locked = System->tryLock(&System->handle);
    
    return Locked;
}

void CCComponentSystemLock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    if ((System) && (System->lock)) System->lock(&System->handle);
}

void CCComponentSystemUnlock(CCComponentSystemID id)
{
    CCComponentSystem *System = CCComponentSystemFind(id);
    if ((System) && (System->unlock)) System->unlock(&System->handle);
}
