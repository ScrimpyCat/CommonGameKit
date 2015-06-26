//
//  EntityManager.c
//  Blob Game
//
//  Created by Stefan Johnson on 23/02/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "EntityManager.h"
#include "Entity.h"
#include "ComponentBase.h"
#include "ComponentSystem.h"
#include <CommonC/Common.h>
#include <tinycthread.h>
#include <stdatomic.h>


typedef struct {
    CCCollection active, added, removed, destroy;
    atomic_flag addedLock, removedLock;
} CCEntityManager;

static CCEntityManager EntityManager = {
    .addedLock = ATOMIC_FLAG_INIT,
    .removedLock = ATOMIC_FLAG_INIT
};

static void CCEntityDestructor(CCCollection Collection, CCEntity *Entity)
{
    CCEntityDestroy(*Entity);
}

void CCEntityManagerCreate(void)
{
    EntityManager = (CCEntityManager){
        .active = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyEnumerating | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCEntity), NULL),
        .added = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCEntity), NULL),
        .removed = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCEntity), NULL),
        .destroy = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting | CCCollectionHintHeavyDeleting, sizeof(CCEntity), (CCCollectionElementDestructor)CCEntityDestructor),
        .addedLock = ATOMIC_FLAG_INIT,
        .removedLock = ATOMIC_FLAG_INIT
    };
}

void CCEntityManagerDestroy(void)
{
    if (EntityManager.active) CCCollectionDestroy(EntityManager.active);
    if (EntityManager.added) CCCollectionDestroy(EntityManager.added);
    if (EntityManager.removed) CCCollectionDestroy(EntityManager.removed);
    if (EntityManager.destroy) CCCollectionDestroy(EntityManager.destroy);
}

void CCEntityManagerUpdate(void)
{
    //Add to active
    while (!atomic_flag_test_and_set(&EntityManager.addedLock));
    CCCollectionInsertCollection(EntityManager.active, EntityManager.added, NULL); //TODO: make a consumed insert or a retained list?
    
    CCCollectionRemoveAllElements(EntityManager.added);
    
    atomic_flag_clear(&EntityManager.addedLock);
    
    
    //Remove from active
    while (!atomic_flag_test_and_set(&EntityManager.removedLock));
    CCCollection Entries = CCCollectionFindCollection(EntityManager.active, EntityManager.removed, NULL);
    CCCollectionRemoveCollection(EntityManager.active, Entries);
    CCCollectionDestroy(Entries);
    
    CCCollectionInsertCollection(EntityManager.destroy, EntityManager.removed, NULL); //TODO: make a consumed insert or a retained list?
    
    CCCollectionRemoveAllElements(EntityManager.removed);
    atomic_flag_clear(&EntityManager.removedLock);
    
    
    CCCollection DestroyableEntries = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyInserting, sizeof(CCCollectionEntry), NULL);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(EntityManager.destroy, &Enumerator);
    for (CCEntity *Entity = CCCollectionEnumeratorGetCurrent(&Enumerator); Entity; Entity = CCCollectionEnumeratorNext(&Enumerator))
    {
        _Bool DestroyEntity = TRUE;
        CCEnumerator ComponentEnumerator;
        CCCollectionGetEnumerator(CCEntityGetComponents(*Entity), &ComponentEnumerator);
        for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&ComponentEnumerator); Component; Component = CCCollectionEnumeratorNext(&ComponentEnumerator))
        {
            if (CCComponentGetIsManaged(Component))
            {
                CCComponentSystemRemoveComponent(Component);
                DestroyEntity = FALSE;
            }
        }
        
        
        if (DestroyEntity) CCCollectionInsertElement(DestroyableEntries, &(CCCollectionEntry){ CCCollectionEnumeratorGetEntry(&Enumerator) });
    }
    
    CCCollectionRemoveCollection(EntityManager.destroy, DestroyableEntries);
    CCCollectionDestroy(DestroyableEntries);
}

void CCEntityManagerAddEntity(CCEntity Entity)
{
    while (!atomic_flag_test_and_set(&EntityManager.addedLock));
    CCCollectionInsertElement(EntityManager.added, &Entity);
    atomic_flag_clear(&EntityManager.addedLock);
}

void CCEntityManagerRemoveEntity(CCEntity Entity)
{
    while (!atomic_flag_test_and_set(&EntityManager.removedLock));
    CCCollectionInsertElement(EntityManager.removed, &Entity);
    atomic_flag_clear(&EntityManager.removedLock);
}
