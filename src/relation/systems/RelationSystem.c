/*
 *  Copyright (c) 2017, Stefan Johnson
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

#define CC_QUICK_COMPILE
#include "RelationSystem.h"
#include <threads.h>
#include "RelationParentComponent.h"
#include "TypeCallbacks.h"

static _Bool CCRelationSystemTryLock(CCComponentSystemHandle *System);
static void CCRelationSystemLock(CCComponentSystemHandle *System);
static void CCRelationSystemUnlock(CCComponentSystemHandle *System);
static _Bool CCRelationSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id);
static void CCRelationSystemUpdate(CCComponentSystemHandle *System, double DeltaTime, CCCollection(CCComponent) Components);

static mtx_t Lock;
static CCDictionary(CCEntity, CCCollection(CCComponent)) Parents;
void CCRelationSystemRegister(void)
{
    int err;
    if ((err = mtx_init(&Lock, mtx_plain | mtx_recursive)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create relation system lock (%d)", err);
        exit(EXIT_FAILURE); //TODO: How should we handle this?
    }
    
    Parents = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding, sizeof(CCEntity), sizeof(CCCollection), NULL);
    
    CCComponentSystemRegister(CC_RELATION_SYSTEM_ID, CCComponentSystemExecutionTypeUpdate, (CCComponentSystemUpdateCallback)CCRelationSystemUpdate, NULL, CCRelationSystemHandlesComponent, NULL, NULL, CCRelationSystemTryLock, CCRelationSystemLock, CCRelationSystemUnlock);
}

void CCRelationSystemDeregister(void)
{
    mtx_destroy(&Lock);
    
    CCComponentSystemDeregister(CC_RELATION_SYSTEM_ID, CCComponentSystemExecutionTypeUpdate);
}

static _Bool CCRelationSystemTryLock(CCComponentSystemHandle *System)
{
    int err = mtx_trylock(&Lock);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock Relation system (%d)", err);
    }
    
    return err == thrd_success;
}

static void CCRelationSystemLock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_lock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock Relation system (%d)", err);
    }
}

static void CCRelationSystemUnlock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_unlock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock Relation system (%d)", err);
    }
}

static _Bool CCRelationSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id)
{
    return (id & CC_COMPONENT_SYSTEM_FLAG_MASK) == CC_RELATION_COMPONENT_FLAG;
}

static void CCRelationSystemUpdate(CCComponentSystemHandle *System, double DeltaTime, CCCollection(CCComponent) Components)
{
    CCCollection(CCComponent) Removed = CCComponentSystemGetRemovedComponentsForSystem(CC_RELATION_SYSTEM_ID);
    
    CC_COLLECTION_FOREACH(CCComponent, Relation, Removed)
    {
        if (CCComponentGetID(Relation) == CC_RELATION_PARENT_COMPONENT_ID)
        {
            CCEntity Parent = CCRelationParentComponentGetParent(Relation);
            CCDictionaryEntry Entry = CCDictionaryFindKey(Parents, &Parent);
            
            if (Entry)
            {
                CCCollection(CCComponent) Children = *(CCCollection*)CCDictionaryGetEntry(Parents, Entry);
                CCCollectionRemoveElement(Children, CCCollectionFindElement(Children, &Relation, NULL));
                
                if (!CCCollectionGetCount(Children)) CCDictionaryRemoveEntry(Parents, Entry);
            }
        }
    }
    
    CCCollectionDestroy(Removed);
    
    CCCollection(CCComponent) Added = CCComponentSystemGetAddedComponentsForSystem(CC_RELATION_SYSTEM_ID);
    
    CC_COLLECTION_FOREACH(CCComponent, Relation, Added)
    {
        if (CCComponentGetID(Relation) == CC_RELATION_PARENT_COMPONENT_ID)
        {
            CCEntity Parent = CCRelationParentComponentGetParent(Relation);
            CCDictionaryEntry Entry = CCDictionaryEntryForKey(Parents, &Parent);
            
            CCCollection(CCComponent) Children;
            if (!CCDictionaryEntryIsInitialized(Parents, Entry))
            {
                Children = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyFinding, sizeof(CCComponent), NULL);
                CCDictionarySetEntry(Parents, Entry, &Children);
            }
            
            else Children = *(CCCollection*)CCDictionaryGetEntry(Parents, Entry);
            
            CCCollectionInsertElement(Children, &Relation);
        }
    }
    
    CCCollectionDestroy(Added);
}

CCCollection(CCEntity) CCRelationSystemGetChildren(CCEntity Entity)
{
    CCCollection(CCEntity) Entities = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating, sizeof(CCEntity), CCEntityDestructorForCollection);
    
    CCRelationSystemLock(NULL);
    
    CCCollection(CCComponent) *Children = CCDictionaryGetValue(Parents, &Entity);
    if (Children)
    {
        CC_COLLECTION_FOREACH(CCComponent, Relation, *Children)
        {
            CCCollectionInsertElement(Entities, &(CCEntity){ CCRetain(CCComponentGetEntity(Relation)) });
        }
    }
    
    CCRelationSystemUnlock(NULL);
    
    return Entities;
}
