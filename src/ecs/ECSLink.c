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

#define CC_QUICK_COMPILE
#include "ECSLink.h"
#include "ECSContext.h"
#include "ECS.h"

void ECSLinkMapInit(ECSContext *Context)
{
    CCAssertLog(Context, "Context must not be null");
    
    if (!Context->links.associations) Context->links.associations = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCDictionary), Context->manager.map->chunkSize);
}

static uintmax_t ECSLinkHasher(const void **Key)
{
    return (uintptr_t)*Key;
}

static CCComparisonResult ECSLinkComparator(const void **Left, const void **Right)
{
    return *Left == *Right ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static _Bool ECSLinkFindEntity(const ECSEntity *Entities, size_t Count, ECSEntity Entity, size_t *Index)
{
    if (!Count)
    {
        *Index = 0;
        
        return FALSE;
    }
    
    size_t M = 0;
    
    for (ptrdiff_t L = 0, R = Count - 1; L <= R; )
    {
        M = (L + R) / 2;
        
        const ECSEntity EntityM = Entities[M];
        
        if (EntityM < Entity) L = M + 1;
        else if (EntityM > Entity) R = M - 1;
        else break;
    }
    
    *Index = M + (Entity > Entities[M]);
    
    return Entities[M] == Entity;
}

void ECSLinkAdd(ECSContext *Context, ECSEntity EntityA, void *DataA, const ECSLink *Link, ECSEntity EntityB, void *DataB)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(ECSEntityIsAlive(Context, EntityA), "EntityA must be alive");
    CCAssertLog(ECSEntityIsAlive(Context, EntityB), "EntityB must be alive");
    CCAssertLog(Link, "Link must not be null");
    
    if (ECS_LINK_IS_INVERTED(Link))
    {
        Link = ECS_LINK_INVERT(Link);
        const ECSEntity Temp = EntityA;
        EntityA = EntityB;
        EntityB = Temp;
    }
    
    const size_t Count = CCArrayGetCount(Context->links.associations);
    const size_t MaxEntity = CCMax(EntityA, EntityB);
    
    if (MaxEntity >= Count)
    {
        const size_t NewElementCount = (MaxEntity - Count) + 1;
        
        CCArrayAppendElements(Context->links.associations, NULL, NewElementCount);
        
        memset(CCArrayGetData(Context->links.associations) + (Count * sizeof(CCDictionary)), 0, NewElementCount * sizeof(CCDictionary));
    }
    
    
    struct {
        ECSEntity entity;
        void *data;
        ECSLinkType side;
        ECSEntity prev;
    } Pair[2] = {
        { EntityA, DataA, ECSLinkTypeWithLeft, ECS_ENTITY_NULL },
        { EntityB, DataB, ECSLinkTypeWithRight, ECS_ENTITY_NULL }
    };
    
    const void *Key = Link;
    
    for (size_t Loop = 0; Loop < 2; Loop++, Key = ECS_LINK_INVERT(Key))
    {
        const ECSLinkType OppositeSide = Link->type >> Pair[(Loop + 1) & 1].side;
        
        const _Bool HasMany = (OppositeSide & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
        
        CCDictionary *Assoc = CCArrayGetElementAtIndex(Context->links.associations, Pair[Loop].entity);
        
        if (!*Assoc)
        {
            size_t Size;
            CCDictionaryElementDestructor Destructor;
            
            if (HasMany)
            {
                Size = sizeof(CCArray);
                Destructor = CCArrayDestructorForDictionary;
            }
            
            else
            {
                Size = sizeof(ECSEntity);
                Destructor = NULL;
            }
            
            *Assoc = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintHeavyInserting | CCDictionaryHintHeavyDeleting, sizeof(void*), Size, &(CCDictionaryCallbacks){
                .getHash = (CCDictionaryKeyHasher)ECSLinkHasher,
                .compareKeys = (CCComparator)ECSLinkComparator,
                .valueDestructor = Destructor
            });
        }
        
        CCDictionaryEntry LinkEntry = CCDictionaryEntryForKey(*Assoc, &Key);
        const ECSEntity LinkedEntity = Pair[(Loop + 1) & 1].entity;
        
        if (HasMany)
        {
            CCArray(ECSEntity) LinkedEntities;
            
            if (!CCDictionaryEntryIsInitialized(*Assoc, LinkEntry))
            {
                LinkedEntities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);
                CCDictionarySetEntry(*Assoc, LinkEntry, &LinkedEntities);
            }
            
            else LinkedEntities = *(CCArray*)CCDictionaryGetEntry(*Assoc, LinkEntry);
            
            const size_t LinkedCount = CCArrayGetCount(LinkedEntities);
            
            size_t Index = 0;
            if (!ECSLinkFindEntity(CCArrayGetData(LinkedEntities), LinkedCount, LinkedEntity, &Index))
            {
                if (Index == LinkedCount) CCArrayAppendElement(LinkedEntities, &LinkedEntity);
                else CCArrayInsertElementAtIndex(LinkedEntities, Index, &LinkedEntity);
            }
            
            else return;
        }
        
        else
        {
            if (CCDictionaryEntryIsInitialized(*Assoc, LinkEntry))
            {
                const ECSEntity PrevEntity = *(ECSEntity*)CCDictionaryGetEntry(*Assoc, LinkEntry);
                
                if (PrevEntity == Pair[(Loop + 1) & 1].entity) return;
                
                Pair[Loop].prev = PrevEntity;
                
                const _Bool OppositeHasMany = ((Link->type >> Pair[Loop].side) & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
                
                CCDictionary OppositeAssoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, PrevEntity);
                CCDictionaryEntry OppositeLinkEntry = CCDictionaryFindKey(OppositeAssoc, &(void*){ ECS_LINK_INVERT(Key) });
                
                if (OppositeHasMany)
                {
                    CCArray OppositeLinkedEntities = *(CCArray*)CCDictionaryGetEntry(OppositeAssoc, OppositeLinkEntry);
                    const size_t OppositeLinkedCount = CCArrayGetCount(OppositeLinkedEntities);
                    
                    if (OppositeLinkedCount > 1)
                    {
                        size_t Index;
                        ECSLinkFindEntity(CCArrayGetData(OppositeLinkedEntities), OppositeLinkedCount, Pair[Loop].entity, &Index);
                        
                        CCArrayRemoveElementAtIndex(OppositeLinkedEntities, Index);
                    }
                    
                    else CCDictionaryRemoveEntry(OppositeAssoc, OppositeLinkEntry);
                }
                
                else CCDictionaryRemoveEntry(OppositeAssoc, OppositeLinkEntry);
            }
            
            CCDictionarySetEntry(*Assoc, LinkEntry, &LinkedEntity);
        }
    }
    
    for (size_t Loop = 0; Loop < 2; Loop++)
    {
        const ECSLinkType Side = Link->type >> Pair[Loop].side;
        
        const ECSEntity PrevEntity = Pair[Loop].prev;
        
        if (PrevEntity != ECS_ENTITY_NULL)
        {
            switch (Side & ECSLinkTypeAssociateMask)
            {
                case ECSLinkTypeAssociateCallback:
                    Link->associate[Loop].callback.remove(Context, Pair[Loop].entity);
                    break;
                    
                default:
                    break;
            }
        }
        
        switch (Side & ECSLinkTypeAssociateMask)
        {
            case ECSLinkTypeAssociateCallback:
                Link->associate[Loop].callback.add(Context, Pair[Loop].entity, Pair[Loop].data);
                break;
                
            case ECSLinkTypeAssociateComponent:
                ECSEntityAddComponent(Context, Pair[Loop].entity, (Pair[Loop].data ? Pair[Loop].data : Link->associate[Loop].component.data), Link->associate[Loop].component.id);
                break;
                
            default:
                break;
        }
        
        if (PrevEntity != ECS_ENTITY_NULL)
        {
            const ECSLinkType OppositeSide = Link->type >> Pair[(Loop + 1) & 1].side;
            
            switch (OppositeSide & ECSLinkTypeAssociateMask)
            {
                case ECSLinkTypeAssociateCallback:
                    Link->associate[(Loop + 1) & 1].callback.remove(Context, PrevEntity);
                    break;
                    
                case ECSLinkTypeAssociateComponent:
                    ECSEntityRemoveComponent(Context, PrevEntity, Link->associate[(Loop + 1) & 1].component.id);
                    break;
                    
                default:
                    break;
            }
            
            if ((OppositeSide & ECSLinkTypeDeletionMask) == ECSLinkTypeDeletionCascading)
            {
                ECSEntityDestroy(Context, &PrevEntity, 1);
            }
        }
    }
}

void ECSLinkRemove(ECSContext *Context, ECSEntity EntityA, const ECSLink *Link, ECSEntity EntityB)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Link, "Link must not be null");
    
    if (ECS_LINK_IS_INVERTED(Link))
    {
        Link = ECS_LINK_INVERT(Link);
        const ECSEntity Temp = EntityA;
        EntityA = EntityB;
        EntityB = Temp;
    }
    
    const size_t Count = CCArrayGetCount(Context->links.associations);
    const size_t MaxEntity = CCMax(EntityA, EntityB);
    
    if (MaxEntity < Count)
    {
        struct {
            ECSEntity entity;
            ECSLinkType side;
        } Pair[2] = {
            { EntityA, ECSLinkTypeWithLeft },
            { EntityB, ECSLinkTypeWithRight},
        };
        
        const void *Key = Link;
        
        for (size_t Loop = 0; Loop < 2; Loop++, Key = ECS_LINK_INVERT(Key))
        {
            const ECSLinkType OppositeSide = Link->type >> Pair[(Loop + 1) & 1].side;
            
            const _Bool HasMany = (OppositeSide & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
            
            CCDictionary Assoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, Pair[Loop].entity);
            
            if (!Assoc) return;
            
            CCDictionaryEntry LinkEntry = CCDictionaryFindKey(Assoc, &Key);
            
            if (!LinkEntry) return;
            
            void *Linked = CCDictionaryGetEntry(Assoc, LinkEntry);
            const ECSEntity LinkedEntity = Pair[(Loop + 1) & 1].entity;
            
            if (HasMany)
            {
                CCArray(ECSEntity) LinkedEntities = *(CCArray*)Linked;
                const size_t LinkedCount = CCArrayGetCount(LinkedEntities);
                
                size_t Index = 0;
                if (!ECSLinkFindEntity(CCArrayGetData(LinkedEntities), LinkedCount, LinkedEntity, &Index)) return;
                
                CCArrayRemoveElementAtIndex(LinkedEntities, Index);
                
                if (LinkedCount == 1) CCDictionaryRemoveEntry(Assoc, LinkEntry);
            }
            
            else
            {
                if (*(ECSEntity*)Linked != LinkedEntity) return;
                
                CCDictionaryRemoveEntry(Assoc, LinkEntry);
            }
        }
        
        
        for (size_t Loop = 0; Loop < 2; Loop++, Key = ECS_LINK_INVERT(Key))
        {
            const ECSLinkType Side = Link->type >> Pair[Loop].side;
            
            switch (Side & ECSLinkTypeAssociateMask)
            {
                case ECSLinkTypeAssociateCallback:
                    Link->associate[Loop].callback.remove(Context, Pair[Loop].entity);
                    break;
                    
                case ECSLinkTypeAssociateComponent:
                    ECSEntityRemoveComponent(Context, Pair[Loop].entity, Link->associate[Loop].component.id);
                    break;
                    
                default:
                    break;
            }
            
            if ((Side & ECSLinkTypeDeletionMask) == ECSLinkTypeDeletionCascading)
            {
                ECSEntityDestroy(Context, &Pair[Loop].entity, 1);
            }
        }
    }
}

static void ECSLinkRemoveLinkForOppositeEntity(ECSContext *Context, ECSEntity LinkedEntity, _Bool HasMany, const void *Key, ECSEntity Entity)
{
    CCDictionary OppositeAssoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, LinkedEntity);
    
    if (HasMany)
    {
        CCDictionaryEntry LinkEntry = CCDictionaryFindKey(OppositeAssoc, &Key);
        void *OppositedLinked = CCDictionaryGetEntry(OppositeAssoc, LinkEntry);
        
        CCArray(ECSEntity) OppositeLinkedEntities = *(CCArray*)OppositedLinked;
        const size_t OppositeLinkedCount = CCArrayGetCount(OppositeLinkedEntities);
        
        size_t Index = 0;
        if (!ECSLinkFindEntity(CCArrayGetData(OppositeLinkedEntities), OppositeLinkedCount, Entity, &Index)) return;
        
        CCArrayRemoveElementAtIndex(OppositeLinkedEntities, Index);
        
        if (OppositeLinkedCount == 1) CCDictionaryRemoveEntry(OppositeAssoc, LinkEntry);
    }
    
    else
    {
        CCDictionaryRemoveValue(OppositeAssoc, &Key);
    }
}

void ECSLinkRemoveLinkForEntity(ECSContext *Context, ECSEntity Entity, const ECSLink *Link)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Link, "Link must not be null");
    
    const size_t Count = CCArrayGetCount(Context->links.associations);
    const size_t MaxEntity = Entity;
    
    if (MaxEntity < Count)
    {
        const void *Key = Link, *OppositeKey = ECS_LINK_INVERT(Link);
        
        ECSLinkType Side, OppositeSide;
        size_t SideIndex, OppositeSideIndex;
        
        if (ECS_LINK_IS_INVERTED(Link))
        {
            Link = ECS_LINK_INVERT(Link);
            
            Side = ECSLinkTypeWithRight;
            OppositeSide = ECSLinkTypeWithLeft;
            SideIndex = 1;
            OppositeSideIndex = 0;
        }
        
        else
        {
            Side = ECSLinkTypeWithLeft;
            OppositeSide = ECSLinkTypeWithRight;
            SideIndex = 0;
            OppositeSideIndex = 1;
        }
        
        Side = Link->type >> Side;
        OppositeSide = Link->type >> OppositeSide;
        
        const _Bool HasMany = (OppositeSide & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
        const _Bool OppositeHasMany = (Side & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
        
        CCDictionary Assoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, Entity);
        
        if (!Assoc) return;
        
        CCDictionaryEntry LinkEntry = CCDictionaryFindKey(Assoc, &Key);
        
        if (!LinkEntry) return;
        
        void *Linked = CCDictionaryGetEntry(Assoc, LinkEntry);
        
        if (HasMany)
        {
            CCArray(ECSEntity) LinkedEntities = CCRetain(*(CCArray*)Linked);
            const size_t LinkedCount = CCArrayGetCount(LinkedEntities);
            
            CCDictionaryRemoveEntry(Assoc, LinkEntry);
            
            if (LinkedCount)
            {
                switch (OppositeSide & ECSLinkTypeAssociateMask)
                {
                    case ECSLinkTypeAssociateCallback:
                        for (size_t Loop = 0; Loop < LinkedCount; Loop++)
                        {
                            const ECSEntity LinkedEntity = *(ECSEntity*)CCArrayGetElementAtIndex(LinkedEntities, Loop);
                            
                            ECSLinkRemoveLinkForOppositeEntity(Context, LinkedEntity, OppositeHasMany, OppositeKey, Entity);
                            
                            Link->associate[OppositeSideIndex].callback.remove(Context, LinkedEntity);
                        }
                        break;
                        
                    case ECSLinkTypeAssociateComponent:
                        for (size_t Loop = 0; Loop < LinkedCount; Loop++)
                        {
                            const ECSEntity LinkedEntity = *(ECSEntity*)CCArrayGetElementAtIndex(LinkedEntities, Loop);
                            
                            ECSLinkRemoveLinkForOppositeEntity(Context, LinkedEntity, OppositeHasMany, OppositeKey, Entity);
                            
                            ECSEntityRemoveComponent(Context, LinkedEntity, Link->associate[OppositeSideIndex].component.id);
                        }
                        break;
                        
                    default:
                        for (size_t Loop = 0; Loop < LinkedCount; Loop++)
                        {
                            const ECSEntity LinkedEntity = *(ECSEntity*)CCArrayGetElementAtIndex(LinkedEntities, Loop);
                            
                            ECSLinkRemoveLinkForOppositeEntity(Context, LinkedEntity, OppositeHasMany, OppositeKey, Entity);
                        }
                        break;
                }
                
                if ((OppositeSide & ECSLinkTypeDeletionMask) == ECSLinkTypeDeletionCascading)
                {
                    ECSEntityDestroy(Context, CCArrayGetData(LinkedEntities), LinkedCount);
                }
            }
            
            CCArrayDestroy(LinkedEntities);
        }
        
        else
        {
            const ECSEntity LinkedEntity = *(ECSEntity*)Linked;
            
            CCDictionaryRemoveEntry(Assoc, LinkEntry);
            
            ECSLinkRemoveLinkForOppositeEntity(Context, LinkedEntity, OppositeHasMany, OppositeKey, Entity);
            
            switch (OppositeSide & ECSLinkTypeAssociateMask)
            {
                case ECSLinkTypeAssociateCallback:
                    Link->associate[OppositeSideIndex].callback.remove(Context, LinkedEntity);
                    break;
                    
                case ECSLinkTypeAssociateComponent:
                    ECSEntityRemoveComponent(Context, LinkedEntity, Link->associate[OppositeSideIndex].component.id);
                    break;
                    
                default:
                    break;
            }
            
            if ((OppositeSide & ECSLinkTypeDeletionMask) == ECSLinkTypeDeletionCascading)
            {
                ECSEntityDestroy(Context, &LinkedEntity, 1);
            }
        }
        
        
        switch (Side & ECSLinkTypeAssociateMask)
        {
            case ECSLinkTypeAssociateCallback:
                Link->associate[SideIndex].callback.remove(Context, Entity);
                break;
                
            case ECSLinkTypeAssociateComponent:
                ECSEntityRemoveComponent(Context, Entity, Link->associate[SideIndex].component.id);
                break;
                
            default:
                break;
        }
        
        if ((Side & ECSLinkTypeDeletionMask) == ECSLinkTypeDeletionCascading)
        {
            ECSEntityDestroy(Context, &Entity, 1);
        }
    }
}

void ECSLinkRemoveAllLinksForEntity(ECSContext *Context, ECSEntity Entity)
{
    CCAssertLog(Context, "Context must not be null");
    
    CCEnumerable Enumerable;
    ECSLinkEnumerable(Context, Entity, &Enumerable);
    
    CCMemoryZoneSave(ECSSharedZone);
    
    CCMemoryZoneBlock *Block = CCMemoryZoneGetCurrentBlock(ECSSharedZone);
    ptrdiff_t Offset = CCMemoryZoneBlockGetCurrentOffset(Block);
    
    size_t Count = 0;
    for (void **Link = CCEnumerableGetCurrent(&Enumerable); Link; Link = CCEnumerableNext(&Enumerable))
    {
        ECSSharedZoneStore(Link, sizeof(void*));
        Count++;
    }
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        void **Link = CCMemoryZoneBlockGetPointer(&Block, &Offset, NULL);
        ECSLinkRemoveLinkForEntity(Context, Entity, *Link);
        Offset += sizeof(void*);
    }
    
    CCMemoryZoneRestore(ECSSharedZone);
}

void ECSLinkRemoveAllLinksBetweenEntities(ECSContext *Context, ECSEntity EntityA, ECSEntity EntityB)
{
    CCAssertLog(Context, "Context must not be null");
    
    CCEnumerable Enumerable;
    ECSLinkEnumerable(Context, EntityA, &Enumerable);
    
    CCMemoryZoneSave(ECSSharedZone);
    
    CCMemoryZoneBlock *Block = CCMemoryZoneGetCurrentBlock(ECSSharedZone);
    ptrdiff_t Offset = CCMemoryZoneBlockGetCurrentOffset(Block);
    
    size_t Count = 0;
    for (void **Link = CCEnumerableGetCurrent(&Enumerable); Link; Link = CCEnumerableNext(&Enumerable))
    {
        ECSSharedZoneStore(Link, sizeof(void*));
        Count++;
    }
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        void **Link = CCMemoryZoneBlockGetPointer(&Block, &Offset, NULL);
        ECSLinkRemove(Context, EntityA, *Link, EntityB);
        Offset += sizeof(void*);
    }
    
    CCMemoryZoneRestore(ECSSharedZone);
}

void ECSLinkRemoveLink(ECSContext *Context, const ECSLink *Link)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Link, "Link must not be null");
    
    for (size_t Loop = 0, Count = CCArrayGetCount(Context->links.associations); Loop < Count; Loop++)
    {
        ECSLinkRemoveLinkForEntity(Context, Loop, Link);
    }
}

#pragma - Query

_Bool ECSLinked(ECSContext *Context, ECSEntity EntityA, const ECSLink *Link, ECSEntity EntityB)
{
    size_t Count;
    const ECSEntity *Entities = ECSLinkGet(Context, EntityA, Link, &Count);
    
    return Entities ? ECSLinkFindEntity(Entities, Count, EntityB, &Count) : FALSE;
}

const ECSEntity *ECSLinkGet(ECSContext *Context, ECSEntity Entity, const ECSLink *Link, size_t *Count)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Link, "Link must not be null");
    CCAssertLog(Count, "Count must not be null");
    
    const size_t EntityCount = CCArrayGetCount(Context->links.associations);
    const size_t MaxEntity = Entity;
    
    if (MaxEntity < EntityCount)
    {
        const void *Key = Link;
        
        ECSLinkType OppositeSide;
        
        if (ECS_LINK_IS_INVERTED(Link))
        {
            Link = ECS_LINK_INVERT(Link);
            
            OppositeSide = ECSLinkTypeWithLeft;
        }
        
        else
        {
            OppositeSide = ECSLinkTypeWithRight;
        }
        
        OppositeSide = Link->type >> OppositeSide;
        
        const _Bool HasMany = (OppositeSide & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
        
        CCDictionary Assoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, Entity);
        
        if (!Assoc) goto Empty;
        
        CCDictionaryEntry LinkEntry = CCDictionaryFindKey(Assoc, &Key);
        
        if (!LinkEntry) goto Empty;
        
        void *Linked = CCDictionaryGetEntry(Assoc, LinkEntry);
        
        if (HasMany)
        {
            CCArray(ECSEntity) LinkedEntities = *(CCArray*)Linked;
            *Count = CCArrayGetCount(LinkedEntities);
            
            return CCArrayGetData(LinkedEntities);
        }
        
        else
        {
            *Count = 1;
            
            return Linked;
        }
    }
    
Empty:
    *Count = 0;
    
    return NULL;
}

void ECSLinkEnumerable(ECSContext *Context, ECSEntity Entity, CCEnumerable *Enumerable)
{
    CCAssertLog(Context, "Context must not be null");
    CCAssertLog(Enumerable, "Enumerable must not be null");
    
    const size_t Count = CCArrayGetCount(Context->links.associations);
    const size_t MaxEntity = Entity;
    
    if (MaxEntity < Count)
    {
        CCDictionary Assoc = *(CCDictionary*)CCArrayGetElementAtIndex(Context->links.associations, Entity);
        
        if (Assoc)
        {
            CCDictionaryGetKeyEnumerable(Assoc, Enumerable);
            return;
        }
    }
    
    *Enumerable = CCEnumerableCreate(NULL, 0, 0);
}
