/*
 *  Copyright (c) 2024, Stefan Johnson
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

#include "ECSReflect.h"
#include "ECS.h"

#define CC_TYPE_ECSSerializeChunk(...) ECSSerializeChunk
#define CC_TYPE_0_ECSSerializeChunk CC_TYPE_ECSSerializeChunk,

#if ECS_ARCHETYPE_COMPONENT_MAX < UINT8_MAX
const CCReflectInteger CC_REFLECT(ECSArchetypeComponentID) = CC_REFLECT_UNSIGNED_FIXED_WIDTH_INTEGER(ECSArchetypeComponentID, CCReflectEndianNative);
#else
const CCReflectInteger CC_REFLECT(ECSArchetypeComponentID) = CC_REFLECT_UNSIGNED_INTEGER(ECSArchetypeComponentID, CCReflectEndianNative);
#endif

const CCReflectInteger CC_REFLECT(ECSEntity) = CC_REFLECT_UNSIGNED_INTEGER(ECSEntity, CCReflectEndianNative);

const CCReflectType *ECSArchetypeComponentTypes;
const CCReflectType *ECSPackedComponentTypes;
const CCReflectType *ECSIndexedComponentTypes;
const CCReflectType *ECSLocalComponentTypes;

const CCReflectType *ECSDuplicateArchetypeComponentTypes;
const CCReflectType *ECSDuplicatePackedComponentTypes;
const CCReflectType *ECSDuplicateIndexedComponentTypes;
const CCReflectType *ECSDuplicateLocalComponentTypes;

typedef CC_ENUM(ECSSerializeChunk, uint8_t) {
    ECSSerializeChunkEnd = 0,
    ECSSerializeChunkEntity,
    ECSSerializeChunkRegisterEntity,
    ECSSerializeChunkArchetypeComponent,
    ECSSerializeChunkPackedComponent,
    ECSSerializeChunkLink,
    
    ECSSerializeChunkTypeMask = 0xf,
    
    ECSSerializeChunkInvertedLink = ECSSerializeChunkLink | (1 << 4)
};

CCReflectTypeMapper ECSLinkPointerMap = ECSLinkPointerMapDefaults;
CCReflectTypeUnmapper ECSLinkPointerUnmap = ECSLinkPointerUnmapDefaults;

static void ECSLinkPointerMapper(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent)
{
    ECSLinkPointerMap(Type, Data, Args, Handler, Zone, Allocator, Intent);
}

static void ECSLinkPointerUnmapper(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator)
{
    ECSLinkPointerUnmap(Type, MappedType, Data, Args, Handler, Zone, Allocator);
}

void ECSLinkPointerMapDefaults(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent)
{
//#define CC_REFLECT_STATELESS_COMPARATORS \
//    CCStringComparatorForCollection, \
//    CCBigIntComparatorForCollection, \
//    CCBigIntFastComparatorForCollection, \
//    CCStringComparatorForDictionary, \
//    CCBigIntComparatorForDictionary, \
//    CCBigIntFastComparatorForDictionary
//
//    if CC_REFLECT_MAP_STATELESS_VALUES_WHEN(CC_REFLECT_VALUE_IS_POINTER, CC_REFLECT_STATELESS_COMPARATORS)
    /*else*/ CCAssertLog(0, "Unknown link pointer");
}

void ECSLinkPointerUnmapDefaults(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator)
{
//    if CC_REFLECT_UNMAP_STATELESS_VALUES(CC_REFLECT_VALUE_TO_POINTER(CCComparator), CC_REFLECT_STATELESS_COMPARATORS)
    
    CCAssertLog(0, "Unknown link pointer");
}

const CCReflectStaticPointer CC_REFLECT(PTYPE(ECSLink, weak, static)) = CC_REFLECT_STATIC_POINTER(&CC_REFLECT(void), CCReflectOwnershipWeak, ECSLinkPointerMapper, ECSLinkPointerUnmapper);

static const CCReflectInteger CC_REFLECT(ECSSerializeChunk) = CC_REFLECT_UNSIGNED_FIXED_WIDTH_INTEGER(ECSSerializeChunk, CCReflectEndianNative);

void ECSSerialize(ECSContext *Context, void *State, ECSSerializeHandler Handler, CCMemoryZone Zone)
{
    CCAssertLog(Context, "Context must not be null");
    
    Handler(&CC_REFLECT(ECSRegistryID), &Context->registry.id, State);
    
    CCDictionary ArchetypesSerialized = CCDictionaryCreate(CC_ZONE_ALLOCATOR(Zone), CCDictionaryHintSizeMedium, sizeof(void*), 0, NULL);
    
    CCBits(uint8_t, ECSHasBitCount) ComponentsMask;
    CCBits(uint8_t, ECSHasBitCount) ComponentsSerialized;
    
    CC_BITS_INIT_SET(ComponentsMask);
    CC_BITS_INIT_SET(ComponentsSerialized);
    
    const size_t PackedBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypePacked);
    const size_t IndexedBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypeIndexed);
    const size_t LocalBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypeLocal);
    
    size_t ComponentIndexes[CCMax(ECS_LOCAL_COMPONENT_MAX, CCMax(ECS_PACKED_COMPONENT_MAX, ECS_INDEXED_COMPONENT_MAX))];
    
    const size_t Count = CCArrayGetCount(Context->manager.map);
    Handler(&CC_REFLECT(size_t), &Count, State);
    
    const size_t AvailableCount = CCArrayGetCount(Context->manager.available);
    Handler(&CC_REFLECT(size_t), &AvailableCount, State);
    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(size_t), AvailableCount), CCArrayGetData(Context->manager.available), State);
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        const ECSEntity Entity = Count - Loop - 1;
        
        ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
        
        if (!CCBitsGet(Refs->has, ECSHasBitEntityDestroyed))
        {
            Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkEntity }, State);
            
            Handler(&CC_REFLECT(ECSEntity), &Entity, State);
            
            Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(uint8_t), sizeof(Refs->has)), Refs->has, State);
            
            const size_t PackedCount = CCBitsMask(Refs->has, ComponentsMask, PackedBaseIndex, ECS_PACKED_COMPONENT_MAX, ComponentIndexes);
            for (size_t Loop2 = 0; Loop2 < PackedCount; Loop2++)
            {
                ComponentIndexes[Loop2] = Refs->packed.indexes[ComponentIndexes[Loop2] - PackedBaseIndex];
            }
            
            Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(size_t), PackedCount), ComponentIndexes, State);
            
            const size_t LocalComponentCount = CCBitsMask(Refs->has, ComponentsSerialized, LocalBaseIndex, ECS_LOCAL_COMPONENT_MAX, ComponentIndexes);
            
            for (size_t Loop2 = 0; Loop2 < LocalComponentCount; Loop2++)
            {
                const size_t Index = ComponentIndexes[Loop2] - LocalBaseIndex;
                
                if (!ECSLocalComponentTypes[Index]) continue;
                
                void *Local = Refs->local + ECSLocalComponentOffset(ECSComponentIDs[ComponentIndexes[Loop2]]);
                Handler(ECSLocalComponentTypes[Index], Local, State);
            }
            
            const size_t IndexedComponentCount = CCBitsMask(Refs->has, ComponentsSerialized, IndexedBaseIndex, ECS_INDEXED_COMPONENT_MAX, ComponentIndexes);
            
            for (size_t Loop2 = 0; Loop2 < IndexedComponentCount; Loop2++)
            {
                const size_t Index = ComponentIndexes[Loop2] - IndexedBaseIndex;
                
                CCReflectType ComponentType = ECSIndexedComponentTypes[Index];
                if (!ComponentType) continue;
                
                ECSIndexedComponent Indexed = Context->indexed[Index];
                
                Handler(ComponentType, CCArrayGetElementAtIndex(Indexed, Entity), State);
            }
            
            const size_t ArchComponentCount = Refs->archetype.component.count;
            Handler(&CC_REFLECT(size_t), &ArchComponentCount, State);
            
            const ECSArchetype *Archetype = Refs->archetype.ptr;
            
            if (Archetype)
            {
                Handler(&CC_REFLECT(size_t), &Refs->archetype.index, State);
                
                const ECSArchetypeComponentID *ComponentIndexes = Refs->archetype.component.ids;
                
                Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSArchetypeComponentID), ArchComponentCount), ComponentIndexes, State);
                
                CCDictionaryEntry Entry = CCDictionaryEntryForKey(ArchetypesSerialized, &Archetype);
                
                if (!CCDictionaryEntryIsInitialized(ArchetypesSerialized, Entry))
                {
                    Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkArchetypeComponent }, State);
                    
                    Handler(&CC_REFLECT(size_t), &ArchComponentCount, State);
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSArchetypeComponentID), ArchComponentCount), ComponentIndexes, State);
                    
                    const size_t ArchetypeCount = CCArrayGetCount(Archetype->entities);
                    
                    Handler(&CC_REFLECT(size_t), &ArchetypeCount, State);
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), ArchetypeCount), CCArrayGetData(Archetype->entities), State);
                    
                    for (size_t Loop2 = 0; Loop2 < ArchComponentCount; Loop2++)
                    {
                        const size_t Index = ComponentIndexes[Loop2];
                        
                        CCAssertLog(ECSArchetypeComponentTypes[Index], "Archetype component must be reflected");
                        
                        Handler(&CC_REFLECT_ARRAY(ECSArchetypeComponentTypes[Index], ArchetypeCount), CCArrayGetData(Archetype->components[Loop2]), State);
                    }
                }
            }
            
            for (size_t Loop2 = 0, ComponentCount = CCBitsMask(Refs->has, ComponentsSerialized, PackedBaseIndex, ECS_PACKED_COMPONENT_MAX, ComponentIndexes); Loop2 < ComponentCount; Loop2++)
            {
                CCBitsClear(ComponentsSerialized, ComponentIndexes[Loop2]);
                
                const size_t Index = ComponentIndexes[Loop2] - PackedBaseIndex;
                
                if (!ECSPackedComponentTypes[Index]) continue;
                
                Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkPackedComponent }, State);
                Handler(&CC_REFLECT(size_t), &Index, State);
                
                ECSPackedComponent *Packed = &Context->packed[Index];
                const size_t PackedCount = CCArrayGetCount(Packed->entities);
                
                Handler(&CC_REFLECT(size_t), &PackedCount, State);
                
                Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), PackedCount), CCArrayGetData(Packed->entities), State);
                Handler(&CC_REFLECT_ARRAY(ECSPackedComponentTypes[Index], PackedCount), CCArrayGetData(*Packed->components), State);
            }
            
            ECSRegistryID RegistryID = ECSRegistryGetID(Context, Entity);
            
            if (RegistryID)
            {
                Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkRegisterEntity }, State);
                
                Handler(&CC_REFLECT(ECSRegistryID), &RegistryID, State);
            }
            
            CCEnumerable Enumerable;
            ECSLinkEnumerable(Context, Entity, &Enumerable);
            
            for (const ECSLink **Link = CCEnumerableGetCurrent(&Enumerable); Link; Link = CCEnumerableNext(&Enumerable))
            {
                const ECSLink *LinkPtr = *Link;
                
                if (ECS_LINK_IS_INVERTED(*Link))
                {
                    Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkInvertedLink }, State);
                    
                    LinkPtr = ECS_LINK_INVERT(LinkPtr);
                }
                
                else
                {
                    Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkLink }, State);
                }
                
                Handler(&CC_REFLECT(PTYPE(ECSLink, weak, static)), &LinkPtr, State);
                
                size_t EntityCount;
                const ECSEntity *Entities = ECSLinkGet(Context, Entity, *Link, &EntityCount);
                
                Handler(&CC_REFLECT(size_t), &EntityCount, State);
                Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), EntityCount), Entities, State);
            }
        }
    }
    
    Handler(&CC_REFLECT(ECSSerializeChunk), &(ECSSerializeChunk){ ECSSerializeChunkEnd }, State);
    
    CCDictionaryDestroy(ArchetypesSerialized);
}

void ECSDeserialize(ECSContext *Context, void *State, ECSDeserializeHandler Handler)
{
    CCAssertLog(Context, "Context must not be null");
    
    CCBits(uint8_t, ECSHasBitCount) ComponentsMask;
    CCBits(uint8_t, ECSHasBitCount) HasFilter;
    
    CC_BITS_INIT_SET(ComponentsMask);
    CC_BITS_INIT_SET(HasFilter);
    
    for (size_t Loop = 0; Loop < ECS_PACKED_COMPONENT_MAX; Loop++)
    {
        if (!ECSPackedComponentTypes[Loop]) CCBitsClear(HasFilter, Loop + ECS_ARCHETYPE_COMPONENT_MAX);
    }
    
    for (size_t Loop = 0; Loop < ECS_INDEXED_COMPONENT_MAX; Loop++)
    {
        if (!ECSIndexedComponentTypes[Loop]) CCBitsClear(HasFilter, Loop + (ECS_ARCHETYPE_COMPONENT_MAX + ECS_PACKED_COMPONENT_MAX));
    }
    
    for (size_t Loop = 0; Loop < ECS_LOCAL_COMPONENT_MAX; Loop++)
    {
        if (!ECSLocalComponentTypes[Loop]) CCBitsClear(HasFilter, Loop + (ECS_ARCHETYPE_COMPONENT_MAX + ECS_PACKED_COMPONENT_MAX + ECS_INDEXED_COMPONENT_MAX));
    }
    
    
    ECSRegistryID RegistryID = NULL;
    Handler(&CC_REFLECT(ECSRegistryID), &RegistryID, State);
    
    CCAssertLog(RegistryID, "RegistryID was not initialised");
    
    ECSRegistryInit(Context, RegistryID);
    
    
    size_t EntityCount = SIZE_MAX;
    Handler(&CC_REFLECT(size_t), &EntityCount, State);
    
    CCAssertLog(EntityCount != SIZE_MAX, "EntityCount was not initialised");
    
    CCArrayRemoveAllElements(Context->manager.map);
    
    if (EntityCount) CCArrayAppendElements(Context->manager.map, NULL, EntityCount);
    
    size_t AvailableCount = SIZE_MAX;
    Handler(&CC_REFLECT(size_t), &AvailableCount, State);
    
    CCAssertLog(AvailableCount != SIZE_MAX, "AvailableCount was not initialised");
    
    CCArrayRemoveAllElements(Context->manager.available);
    
    if (AvailableCount)
    {
        CCArrayAppendElements(Context->manager.available, NULL, AvailableCount);
        
        Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(size_t), AvailableCount), CCArrayGetData(Context->manager.available), State);
    }
    
    const size_t PackedBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypePacked);
    const size_t IndexedBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypeIndexed);
    const size_t LocalBaseIndex = ECSComponentBaseIndex(ECSComponentStorageTypeLocal);
    
    size_t ComponentIndexes[CCMax(ECS_LOCAL_COMPONENT_MAX, CCMax(ECS_PACKED_COMPONENT_MAX, ECS_INDEXED_COMPONENT_MAX))];
    
    ECSSerializeChunk Chunk = ECSSerializeChunkEnd;
    Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
    
    size_t Entity = SIZE_MAX;
    
    while (Chunk != ECSSerializeChunkEnd)
    {
        switch (Chunk & ECSSerializeChunkTypeMask)
        {
            case ECSSerializeChunkEntity:
                do
                {
                    Entity = SIZE_MAX;
                    Handler(&CC_REFLECT(ECSEntity), &Entity, State);
                    
                    CCAssertLog(Entity != SIZE_MAX, "Entity was not initialised");
                    
                    ECSComponentRefs *Refs = CCArrayGetElementAtIndex(Context->manager.map, Entity);
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(uint8_t), sizeof(Refs->has)), Refs->has, State);
                    
                    const size_t PackedCount = CCBitsMask(Refs->has, ComponentsMask, PackedBaseIndex, ECS_PACKED_COMPONENT_MAX, ComponentIndexes);
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(size_t), PackedCount), Refs->packed.indexes, State);
                    
                    for (size_t Loop = 0; Loop < PackedCount; Loop++)
                    {
                        const size_t Index = PackedCount - Loop - 1;
                        
                        Refs->packed.indexes[ComponentIndexes[Index] - PackedBaseIndex] = Refs->packed.indexes[Index];
                    }
                    
                    CCBitsAnd(Refs->has, HasFilter, 0, ECSHasBitCount);
                    
                    const size_t LocalComponentCount = CCBitsMask(Refs->has, ComponentsMask, LocalBaseIndex, ECS_LOCAL_COMPONENT_MAX, ComponentIndexes);
                    
                    for (size_t Loop = 0; Loop < LocalComponentCount; Loop++)
                    {
                        const size_t Index = ComponentIndexes[Loop] - LocalBaseIndex;
                        
                        void *Local = Refs->local + ECSLocalComponentOffset(ECSComponentIDs[Index + LocalBaseIndex]);
                        Handler(ECSLocalComponentTypes[Index], Local, State);
                    }
                    
                    const size_t IndexedComponentCount = CCBitsMask(Refs->has, ComponentsMask, IndexedBaseIndex, ECS_INDEXED_COMPONENT_MAX, ComponentIndexes);
                    
                    for (size_t Loop = 0; Loop < IndexedComponentCount; Loop++)
                    {
                        const size_t Index = ComponentIndexes[Loop] - IndexedBaseIndex;
                        
                        ECSIndexedComponent Indexed = Context->indexed[Index];
                        
                        if (!Indexed)
                        {
                            Indexed = CCArrayCreate(CC_STD_ALLOCATOR, ECSIndexedComponentSizes[Index], ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(Index));
                            
                            Context->indexed[Index] = Indexed;
                            
                            CCArrayAppendElements(Indexed, NULL, CC_ALIGN(Entity, ECS_INDEXED_COMPONENT_ARRAY_CHUNK_SIZE(Index)));
                        }
                        
                        CCAssertLog(Entity < CCArrayGetCount(Indexed), "Entity chunks should be highest to lowest");
                        
                        Handler(ECSIndexedComponentTypes[Index], CCArrayGetElementAtIndex(Indexed, Entity), State);
                    }
                    
                    size_t ArchComponentCount = SIZE_MAX;
                    Handler(&CC_REFLECT(size_t), &ArchComponentCount, State);
                    
                    CCAssertLog(ArchComponentCount != SIZE_MAX, "ArchComponentCount was not initialised");
                    
                    Refs->archetype.component.count = ArchComponentCount;
                    
                    if (ArchComponentCount)
                    {
                        size_t ArchIndex = SIZE_MAX;
                        Handler(&CC_REFLECT(size_t), &ArchIndex, State);
                        
                        CCAssertLog(ArchIndex != SIZE_MAX, "ArchIndex was not initialised");
                        
                        Refs->archetype.index = ArchIndex;
                        
                        ECSArchetypeComponentID *ComponentIndexes = Refs->archetype.component.ids;

                        Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSArchetypeComponentID), ArchComponentCount), ComponentIndexes, State);
                        
                        const size_t ArchID = ECSArchtypeIndex(ComponentIndexes, ArchComponentCount);
                        Refs->archetype.ptr = ECSArchetypeLookup(Context, ArchID, ArchComponentCount);
                        
#if ECS_ARCHETYPE_COMPONENT_ID_SIMD_LOOKUP && CC_HARDWARE_VECTOR_SUPPORT_SSSE3
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
                        memset(Refs->archetype.component.ids + ArchComponentCount, INT8_MAX, sizeof(Refs->archetype.component.ids) - (ArchComponentCount * sizeof(*Refs->archetype.component.ids)));
#else
                        memset(Refs->archetype.component.ids + ArchComponentCount, UINT8_MAX, sizeof(Refs->archetype.component.ids) - (ArchComponentCount * sizeof(*Refs->archetype.component.ids)));
#endif
#endif
                    }
                    
                    else
                    {
                        Refs->archetype.ptr = NULL;
                        Refs->archetype.index = 0;
                        Refs->archetype.component.count = 0;
                        
#if ECS_ARCHETYPE_COMPONENT_ID_SIMD_LOOKUP && CC_HARDWARE_VECTOR_SUPPORT_SSSE3
#if ECS_ARCHETYPE_COMPONENT_ID_COUNT(ECS_ARCHETYPE_MAX) < INT8_MAX
                        memset(Refs->archetype.component.ids, INT8_MAX, sizeof(Refs->archetype.component.ids));
#else
                        memset(Refs->archetype.component.ids, UINT8_MAX, sizeof(Refs->archetype.component.ids));
#endif
#endif
                    }
                    
                    Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
                } while (Chunk == ECSSerializeChunkEntity);
                break;
                
            case ECSSerializeChunkRegisterEntity:
            {
                CCAssertLog(Entity != SIZE_MAX, "Entity was not initialised");
                
                ECSRegistryID RegistryID = NULL;
                Handler(&CC_REFLECT(ECSRegistryID), &RegistryID, State);
                
                CCAssertLog(RegistryID, "RegistryID was not initialised");
                
                ECSRegistryReregister(Context, Entity, RegistryID, TRUE);
                
                Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
                
                break;
            }
                
            case ECSSerializeChunkArchetypeComponent:
                do
                {
                    size_t ComponentCount = SIZE_MAX;
                    Handler(&CC_REFLECT(size_t), &ComponentCount, State);
                    
                    CCAssertLog(ComponentCount != SIZE_MAX, "ComponentCount was not initialised");
                    CCAssertLog(ComponentCount < ECS_ARCHETYPE_MAX, "ComponentCount exceeds archetype max");
                    
                    ECSArchetypeComponentID ComponentIndexes[ECS_ARCHETYPE_MAX];
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSArchetypeComponentID), ComponentCount), ComponentIndexes, State);
                    
                    const size_t ArchID = ECSArchtypeIndex(ComponentIndexes, ComponentCount);
                    ECSArchetype *Archetype = ECSArchetypeLookup(Context, ArchID, ComponentCount);
                    
                    size_t Count = SIZE_MAX;
                    Handler(&CC_REFLECT(size_t), &Count, State);
                    
                    CCAssertLog(Count != SIZE_MAX, "Count was not initialised");
                    
                    const size_t ChunkSize = ECS_ARCHETYPE_COMPONENT_ARRAY_CHUNK_SIZE(ArchID, ComponentCount);
                    
                    CCArray Entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);
                    
                    Archetype->entities = Entities;
                    
                    CCArrayAppendElements(Entities, NULL, Count);
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), Count), CCArrayGetData(Entities), State);
                    
                    for (size_t Loop = 0; Loop < ComponentCount; Loop++)
                    {
                        const size_t Index = ComponentIndexes[Loop];
                        
                        CCAssertLog(ECSArchetypeComponentTypes[Index], "Archetype component must be reflected");
                        
                        CCArray Components = CCArrayCreate(CC_STD_ALLOCATOR, ECSArchetypeComponentSizes[ComponentIndexes[Loop]], ChunkSize);
                        
                        Archetype->components[Loop] = Components;
                        
                        CCArrayAppendElements(Components, NULL, Count);
                        
                        Handler(&CC_REFLECT_ARRAY(ECSArchetypeComponentTypes[Index], Count), CCArrayGetData(Components), State);
                    }
                    
                    Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
                } while (Chunk == ECSSerializeChunkArchetypeComponent);
                break;
                
            case ECSSerializeChunkPackedComponent:
                do
                {
                    size_t Index = SIZE_MAX, Count = SIZE_MAX;
                    Handler(&CC_REFLECT(size_t), &Index, State);
                    Handler(&CC_REFLECT(size_t), &Count, State);
                    
                    CCAssertLog(Index != SIZE_MAX, "Index was not initialised");
                    CCAssertLog(Count != SIZE_MAX, "Count was not initialised");
                    
                    const size_t ChunkSize = ECS_PACKED_COMPONENT_ARRAY_CHUNK_SIZE(Index);
                    
                    CCArray Entities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), ChunkSize);
                    CCArray Components = CCArrayCreate(CC_STD_ALLOCATOR, ECSPackedComponentSizes[Index], ChunkSize);
                    
                    ECSPackedComponent *Packed = &Context->packed[Index];
                    Packed->entities = Entities;
                    *Packed->components = Components;
                    
                    CCArrayAppendElements(Entities, NULL, Count);
                    CCArrayAppendElements(Components, NULL, Count);
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), Count), CCArrayGetData(Entities), State);
                    Handler(&CC_REFLECT_ARRAY(ECSPackedComponentTypes[Index], Count), CCArrayGetData(Components), State);
                    
                    Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
                } while (Chunk == ECSSerializeChunkPackedComponent);
                break;
                
            case ECSSerializeChunkLink:
            {
                CCAssertLog(Entity != SIZE_MAX, "Entity was not initialised");
                
                const size_t Count = CCArrayGetCount(Context->links.associations);
                
                if (Entity >= Count)
                {
                    const size_t NewElementCount = (Entity - Count) + 1;
                    
                    CCArrayAppendElements(Context->links.associations, NULL, NewElementCount);
                    
                    memset(CCArrayGetData(Context->links.associations) + (Count * sizeof(CCDictionary)), 0, NewElementCount * sizeof(CCDictionary));
                }
                
                do
                {
                    const ECSLink *Link = NULL;
                    Handler(&CC_REFLECT(PTYPE(ECSLink, weak, static)), &Link, State);
                    
                    CCAssertLog(Link, "Link was not initialised");
                    
                    const void *Key = Link;
                    ECSLinkType Side = ECSLinkTypeWithRight;
                    
                    if (Chunk == ECSSerializeChunkInvertedLink)
                    {
                        Key = ECS_LINK_INVERT(Key);
                        Side = ECSLinkTypeWithLeft;
                    }
                    
                    const ECSLinkType OppositeSide = Link->type >> Side;
                    
                    const _Bool HasMany = (OppositeSide & ECSLinkTypeGroupMask) == ECSLinkTypeGroupMany;
                    
                    CCDictionary *Assoc = CCArrayGetElementAtIndex(Context->links.associations, Entity);
                    
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
                    
                    size_t EntityCount = SIZE_MAX;
                    Handler(&CC_REFLECT(size_t), &EntityCount, State);
                    
                    CCAssertLog(EntityCount != SIZE_MAX, "EntityCount was not initialised");
                    
                    void *Entities;
                    
                    if (HasMany)
                    {
                        CCArray(ECSEntity) LinkedEntities;
                        
                        if (!CCDictionaryEntryIsInitialized(*Assoc, LinkEntry))
                        {
                            LinkedEntities = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(ECSEntity), 16);
                            CCDictionarySetEntry(*Assoc, LinkEntry, &LinkedEntities);
                        }
                        
                        else LinkedEntities = *(CCArray*)CCDictionaryGetEntry(*Assoc, LinkEntry);
                        
                        CCArrayRemoveAllElements(LinkedEntities);
                        
                        CCArrayAppendElements(LinkedEntities, NULL, EntityCount);
                        
                        Entities = CCArrayGetData(LinkedEntities);
                    }
                    
                    else
                    {
                        CCAssertLog(EntityCount == 1, "EntityCount must be 1");
                        
                        Entities = CCDictionaryGetEntry(*Assoc, LinkEntry);
                    }
                    
                    Handler(&CC_REFLECT_ARRAY(&CC_REFLECT(ECSEntity), EntityCount), Entities, State);
                    
                    Handler(&CC_REFLECT(ECSSerializeChunk), &Chunk, State);
                } while ((Chunk & ECSSerializeChunkTypeMask) == ECSSerializeChunkLink);
                
                break;
            }
        }
    }
}
