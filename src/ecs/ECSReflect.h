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

#ifndef CommonGameKit_ECSReflect_h
#define CommonGameKit_ECSReflect_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSContext.h>
#include <CommonGameKit/ECS.h>

extern const CCReflectType *ECSArchetypeComponentTypes;
extern const CCReflectType *ECSPackedComponentTypes;
extern const CCReflectType *ECSIndexedComponentTypes;
extern const CCReflectType *ECSLocalComponentTypes;

extern const CCReflectType *ECSDuplicateArchetypeComponentTypes;
extern const CCReflectType *ECSDuplicatePackedComponentTypes;
extern const CCReflectType *ECSDuplicateIndexedComponentTypes;
extern const CCReflectType *ECSDuplicateLocalComponentTypes;

#define CC_TYPE_ECSEntity(...) ECSEntity
#define CC_TYPE_0_ECSEntity CC_TYPE_ECSEntity,

#define CC_TYPE_ECSArchetypeComponentID(...) ECSArchetypeComponentID
#define CC_TYPE_0_ECSArchetypeComponentID CC_TYPE_ECSArchetypeComponentID,

#define CC_TYPE_ECSRegistryID(...) CCBigIntFast
#define CC_TYPE_0_ECSRegistryID CC_TYPE_ECSRegistryID,

#define CC_TYPE_ECSLink(...) ECSLink
#define CC_TYPE_0_ECSLink CC_TYPE_ECSLink,

extern const CCReflectInteger CC_REFLECT(ECSEntity);

extern const CCReflectInteger CC_REFLECT(ECSArchetypeComponentID);

extern const CCReflectStaticPointer CC_REFLECT(PTYPE(ECSLink, weak, static));

extern CCReflectTypeMapper ECSLinkPointerMap;
extern CCReflectTypeUnmapper ECSLinkPointerUnmap;

void ECSLinkPointerMapDefaults(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent);
void ECSLinkPointerUnmapDefaults(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator);

/*!
 * @brief A serialization handler.
 * @param Type The type of @b Data.
 * @param Data The data to be serialized.
 * @param State Custom state to be used by the handler.
 */
typedef void (*ECSSerializeHandler)(CCReflectType Type, const void *Data, void *State);

/*!
 * @brief A deserialization handler.
 * @param Type The type of @b Data.
 * @param Data Where the deserialized data should be written to.
 * @param State Custom state to be used by the handler.
 */
typedef void (*ECSDeserializeHandler)(CCReflectType Type, void *Data, void *State);

/*!
 * @brief Serialize an ECS context.
 * @param Context The context to serialize.
 * @param State The state for the @b Handler.
 * @param Handler The serialization handler.
 * @param Zone The memory zone to use for internal allocations.
 */
void ECSSerialize(ECSContext *Context, void *State, ECSSerializeHandler Handler, CCMemoryZone Zone);

/*!
 * @brief Deserialize an ECS context.
 * @param Context The context to write to.
 * @param State The state for the @b Handler.
 * @param Handler The deserialization handler.
 */
void ECSDeserialize(ECSContext *Context, void *State, ECSDeserializeHandler Handler);

/*!
 * @brief Get the component type.
 * @param ID The component ID to get the type for.
 * @return Returns the type of the component.
 */
static inline CCReflectType ECSComponentType(ECSComponentID ID);

/*!
 * @brief Get the duplicate component type.
 * @param ID The component ID to get the duplicate type for.
 * @return Returns the duplicate type of the component.
 */
static inline CCReflectType ECSDuplicateComponentType(ECSComponentID ID);

#pragma mark -

static inline CCReflectType ECSComponentType(ECSComponentID ID)
{
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
            return ECSArchetypeComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypePacked:
            return ECSPackedComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypeIndexed:
            return ECSIndexedComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypeLocal:
            return ECSLocalComponentTypes[ECSLocalComponentIndex(ID)];
    }
    
    CCAssertLog(0, "Unsupported component type");
    
    return NULL;
}

static inline CCReflectType ECSDuplicateComponentType(ECSComponentID ID)
{
    switch (ID & ECSComponentStorageTypeMask)
    {
        case ECSComponentStorageTypeArchetype:
            return ECSDuplicateArchetypeComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypePacked:
            return ECSDuplicatePackedComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypeIndexed:
            return ECSDuplicateIndexedComponentTypes[ID & ~ECSComponentStorageMask];
            
        case ECSComponentStorageTypeLocal:
            return ECSDuplicateLocalComponentTypes[ECSLocalComponentIndex(ID)];
    }
    
    CCAssertLog(0, "Unsupported component type");
    
    return NULL;
}

#endif
