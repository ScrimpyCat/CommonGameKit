/*
 *  Copyright (c) 2023, Stefan Johnson
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

#include "ECSRegistry.h"
#include "ECSContext.h"

void ECSRegistryInit(ECSContext *Context, ECSRegistryID ID)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog((!Context->registry.id) || (CCBigIntFastCompareLessThanEqual(Context->registry.id, (CCBigIntFast)ID)), "ID must be greater or equal to the current registry ID");
    
    if (!Context->registry.id) Context->registry.id = CCBigIntFastCopy(ID);
    else CCBigIntFastSet(&Context->registry.id, ID);
    
    if (!Context->registry.registeredEntites) Context->registry.registeredEntites = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintHeavyInserting | CCDictionaryHintHeavyDeleting , sizeof(ECSRegistryID), sizeof(ECSEntity), &(CCDictionaryCallbacks){
        .keyDestructor = CCBigIntFastDestructorForDictionary,
        .getHash = CCBigIntFastLowHasherForDictionary,
        .compareKeys = CCBigIntFastComparatorForDictionary
    });
    
    if (!Context->registry.uniqueEntityIDs) Context->registry.uniqueEntityIDs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSRegistryID), Context->manager.map->chunkSize);
}

ECSRegistryID ECSRegistryRegister(ECSContext *Context, ECSEntity Entity)
{
    CCAssertLog(Context, "Context must not be null");
    
    const size_t Count = CCArrayGetCount(Context->registry.uniqueEntityIDs);
    
    if (Entity < Count)
    {
        ECSRegistryID CurrentID = *(ECSRegistryID*)CCArrayGetElementAtIndex(Context->registry.uniqueEntityIDs, Entity);
        
        if (CurrentID) return CurrentID;
    }
    
    else
    {
        const size_t NewElementCount = (Entity - Count) + 1;
        
        CCArrayAppendElements(Context->registry.uniqueEntityIDs, NULL, NewElementCount);
        
        memset(CCArrayGetData(Context->registry.uniqueEntityIDs) + (Count * sizeof(ECSRegistryID)), 0, NewElementCount * sizeof(ECSRegistryID));
    }
    
    
    ECSRegistryID NewID = CCBigIntFastCopy(Context->registry.id);
    
    CCBigIntFastAdd(&Context->registry.id, 1);
    
    CCDictionarySetValue(Context->registry.registeredEntites, &NewID, &Entity);
    CCArrayReplaceElementAtIndex(Context->registry.uniqueEntityIDs, Entity, &NewID);
    
    return NewID;
}

void ECSRegistryDeregister(ECSContext *Context, ECSEntity Entity)
{
    CCAssertLog(Context, "Context must not be null");
    
    const size_t Count = CCArrayGetCount(Context->registry.uniqueEntityIDs);
    
    if (Entity < Count)
    {
        ECSRegistryID ID = *(ECSRegistryID*)CCArrayGetElementAtIndex(Context->registry.uniqueEntityIDs, Entity);
        
        if (ID)
        {
            CCDictionaryRemoveValue(Context->registry.registeredEntites, &ID);
            CCArrayReplaceElementAtIndex(Context->registry.uniqueEntityIDs, Entity, &(ECSRegistryID){ NULL });
        }
    }
}

void ECSRegistryReregister(ECSContext *Context, ECSEntity Entity, ECSRegistryID ID, _Bool AcquireID)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (CCBigIntFastCompareLessThan(Context->registry.id, ID))
    {
        CCBigIntFastSet(&Context->registry.id, ID);
        CCBigIntFastAdd(&Context->registry.id, 1);
    }
    
    CCDictionaryEntry Entry = CCDictionaryEntryForKey(Context->registry.registeredEntites, &ID);
    
    if (CCDictionaryEntryIsInitialized(Context->registry.registeredEntites, Entry))
    {
        ECSEntity RegisteredEntity = *(ECSEntity*)CCDictionaryGetEntry(Context->registry.registeredEntites, Entry);
        
        if (RegisteredEntity == Entity) return;
        
        CCAssertLog(AcquireID, "Cannot reregister a registered ID unless AcquireID is set to TRUE");
        
        CCArrayReplaceElementAtIndex(Context->registry.uniqueEntityIDs, RegisteredEntity, &(ECSRegistryID){ NULL });
    }
    
    const size_t Count = CCArrayGetCount(Context->registry.uniqueEntityIDs);
    
    if (Entity < Count)
    {
        ECSRegistryID CurrentID = *(ECSRegistryID*)CCArrayGetElementAtIndex(Context->registry.uniqueEntityIDs, Entity);
        
        if (CurrentID)
        {
            CCDictionaryRemoveValue(Context->registry.registeredEntites, &CurrentID);
        }
    }
    
    else
    {
        const size_t NewElementCount = (Entity - Count) + 1;
        
        CCArrayAppendElements(Context->registry.uniqueEntityIDs, NULL, NewElementCount);
        
        memset(CCArrayGetData(Context->registry.uniqueEntityIDs) + (Count * sizeof(ECSRegistryID)), 0, NewElementCount * sizeof(ECSRegistryID));
    }
    
    CCDictionarySetEntry(Context->registry.registeredEntites, Entry, &Entity);
    CCArrayReplaceElementAtIndex(Context->registry.uniqueEntityIDs, Entity, &ID);
}

ECSEntity ECSRegistryLookup(ECSContext *Context, ECSRegistryID ID)
{
    CCAssertLog(Context, "Context must not be null");
    
    ECSEntity *Entity = CCDictionaryGetValue(Context->registry.registeredEntites, &ID);
    
    return Entity ? *Entity : ECS_ENTITY_NULL;
}

ECSRegistryID ECSRegistryGetID(ECSContext *Context, ECSEntity Entity)
{
    CCAssertLog(Context, "Context must not be null");
    
    const size_t Count = CCArrayGetCount(Context->registry.uniqueEntityIDs);
    
    if (Entity < Count)
    {
        return *(ECSRegistryID*)CCArrayGetElementAtIndex(Context->registry.uniqueEntityIDs, Entity);
    }
    
    return NULL;
}
