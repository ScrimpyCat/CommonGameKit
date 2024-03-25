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
#include "ECSMutation.h"
#include "ECS.h"

size_t ECSMutableStateEntitiesMax;
size_t ECSMutableStateReplaceRegistryMax;
size_t ECSMutableStateAddLinkMax;
size_t ECSMutableStateRemoveLinkMax;
size_t ECSMutableStateAddComponentMax;
size_t ECSMutableStateRemoveComponentMax;
size_t ECSMutableStateCustomCallbackMax;
size_t ECSMutableStateSharedDataMax;

ECSProxyEntity ECSMutationStageEntityCreate(ECSContext *Context, size_t Count)
{
    return atomic_fetch_add_explicit(&Context->mutations->entity.create, Count, memory_order_relaxed) | ECS_RELATIVE_ENTITY_FLAG;
}

size_t ECSMutationInspectEntityCreate(ECSContext *Context)
{
    return atomic_load_explicit(&Context->mutations->entity.create, memory_order_relaxed);
}

ECSEntity *ECSMutationStageEntityDestroy(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->entity.remove.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateEntitiesMax, "Exceeds ECSMutableStateEntitiesMax (%zu)", ECSMutableStateEntitiesMax);
    
    return Context->mutations->entity.remove.entities + Offset;
}

ECSEntity *ECSMutationInspectEntityDestroy(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->entity.remove.count, memory_order_relaxed);
    
    return Context->mutations->entity.remove.entities;
}

ECSProxyEntity *ECSMutationStageRegistryRegister(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->registry.add.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateEntitiesMax, "Exceeds ECSMutableStateEntitiesMax (%zu)", ECSMutableStateEntitiesMax);
    
    return Context->mutations->registry.add.entities + Offset;
}

ECSProxyEntity *ECSMutationInspectRegistryRegister(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->registry.add.count, memory_order_relaxed);
    
    return Context->mutations->registry.add.entities;
}

ECSEntity *ECSMutationStageRegistryDeregister(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->registry.remove.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateEntitiesMax, "Exceeds ECSMutableStateEntitiesMax (%zu)", ECSMutableStateEntitiesMax);
    
    return Context->mutations->registry.remove.entities + Offset;
}

ECSEntity *ECSMutationInspectRegistryDeregister(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->registry.remove.count, memory_order_relaxed);
    
    return Context->mutations->registry.remove.entities;
}

ECSMutableReplaceRegistryState *ECSMutationStageRegistryReregister(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->registry.replace.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateReplaceRegistryMax, "Exceeds ECSMutableStateReplaceRegistryMax (%zu)", ECSMutableStateReplaceRegistryMax);
    
    return Context->mutations->registry.replace.state + Offset;
}

ECSMutableReplaceRegistryState *ECSMutationInspectRegistryReregister(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->registry.replace.count, memory_order_relaxed);
    
    return Context->mutations->registry.replace.state;
}

ECSMutableAddLinkState *ECSMutationStageAddLink(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->link.add.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateAddLinkMax, "Exceeds ECSMutableStateAddLinkMax (%zu)", ECSMutableStateAddLinkMax);
    
    return Context->mutations->link.add.state + Offset;
}

ECSMutableAddLinkState *ECSMutationInspectAddLink(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->link.add.count, memory_order_relaxed);
    
    return Context->mutations->link.add.state;
}

ECSMutableRemoveLinkState *ECSMutationStageRemoveLink(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->link.remove.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateRemoveLinkMax, "Exceeds ECSMutableStateRemoveLinkMax (%zu)", ECSMutableStateRemoveLinkMax);
    
    return Context->mutations->link.remove.state + Offset;
}

ECSMutableRemoveLinkState *ECSMutationInspectRemoveLink(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->link.remove.count, memory_order_relaxed);
    
    return Context->mutations->link.remove.state;
}

ECSMutableAddComponentState *ECSMutationStageEntityAddComponents(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->component.add.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateAddComponentMax, "Exceeds ECSMutableStateAddComponentMax (%zu)", ECSMutableStateAddComponentMax);
    
    return Context->mutations->component.add.state + Offset;
}

ECSMutableAddComponentState *ECSMutationInspectEntityAddComponents(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->component.add.count, memory_order_relaxed);
    
    return Context->mutations->component.add.state;
}

ECSMutableRemoveComponentState *ECSMutationStageEntityRemoveComponents(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->component.remove.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateRemoveComponentMax, "Exceeds ECSMutableStateRemoveComponentMax (%zu)", ECSMutableStateRemoveComponentMax);
    
    return Context->mutations->component.remove.state + Offset;
}

ECSMutableRemoveComponentState *ECSMutationInspectEntityRemoveComponents(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->component.remove.count, memory_order_relaxed);
    
    return Context->mutations->component.remove.state;
}

ECSMutableCustomCallbackState *ECSMutationStageCustomCallback(ECSContext *Context, size_t Count)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->custom.count, Count, memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateCustomCallbackMax, "Exceeds ECSMutableStateCustomCallbackMax (%zu)", ECSMutableStateCustomCallbackMax);
    
    return Context->mutations->custom.state + Offset;
}

ECSMutableCustomCallbackState *ECSMutationInspectCustomCallback(ECSContext *Context, size_t *Count)
{
    CCAssertLog(Count, "Count must not be null");
    
    *Count = atomic_load_explicit(&Context->mutations->custom.count, memory_order_relaxed);
    
    return Context->mutations->custom.state;
}

void *ECSMutationSetSharedData(ECSContext *Context, size_t Size)
{
    const size_t Offset = atomic_fetch_add_explicit(&Context->mutations->shared.size, CC_ALIGN(Size, 16), memory_order_relaxed);
    
    CCAssertLog(Offset <= ECSMutableStateSharedDataMax, "Exceeds ECSMutableStateSharedDataMax (%zu)", ECSMutableStateSharedDataMax);
    
    return Context->mutations->shared.data + Offset;
}

void *ECSMutationGetSharedData(ECSContext *Context)
{
    return Context->mutations->shared.data;
}

void ECSMutationApply(ECSContext *Context)
{
    atomic_thread_fence(memory_order_acquire);
    
    CCMemoryZoneSave(ECSSharedZone);
    
    size_t NewEntityCount = ECSMutationInspectEntityCreate(Context);
    ECSEntity *NewEntities = NULL;
    if (NewEntityCount)
    {
        NewEntities = CCMemoryZoneAllocate(ECSSharedZone, sizeof(ECSEntity) * NewEntityCount);
        ECSEntityCreate(Context, NewEntities, NewEntityCount);
    }
    
    size_t ReregisterEntityCount;
    ECSMutableReplaceRegistryState *ReregisterState = ECSMutationInspectRegistryReregister(Context, &ReregisterEntityCount);
    for (size_t Loop = 0; Loop < ReregisterEntityCount; Loop++)
    {
        ECSRegistryReregister(Context, ECSProxyEntityResolve(ReregisterState[Loop].entity, NewEntities, NewEntityCount), ReregisterState[Loop].id, ReregisterState[Loop].acquire);
    }
    
    size_t RegisterEntityCount;
    ECSProxyEntity *RegisterEntities = ECSMutationInspectRegistryRegister(Context, &RegisterEntityCount);
    for (size_t Loop = 0; Loop < RegisterEntityCount; Loop++)
    {
        // TODO: Add batched variant
        ECSRegistryRegister(Context, ECSProxyEntityResolve(RegisterEntities[Loop], NewEntities, NewEntityCount));
    }
    
    size_t AddLinkCount;
    ECSMutableAddLinkState *AddLinkState = ECSMutationInspectAddLink(Context, &AddLinkCount);
    for (size_t Loop = 0; Loop < AddLinkCount; Loop++)
    {
        ECSLinkAdd(Context, ECSProxyEntityResolve(AddLinkState[Loop].left.entity, NewEntities, NewEntityCount), AddLinkState[Loop].left.data, AddLinkState[Loop].link, ECSProxyEntityResolve(AddLinkState[Loop].right.entity, NewEntities, NewEntityCount), AddLinkState[Loop].right.data);
    }
    
    size_t RemoveComponentCount;
    ECSMutableRemoveComponentState *RemoveComponentState = ECSMutationInspectEntityRemoveComponents(Context, &RemoveComponentCount);
    for (size_t Loop = 0; Loop < RemoveComponentCount; Loop++)
    {
        ECSEntityRemoveComponents(Context, ECSProxyEntityResolve(RemoveComponentState[Loop].entity, NewEntities, NewEntityCount), RemoveComponentState[Loop].ids, RemoveComponentState[Loop].count);
    }
    
    size_t AddComponentCount;
    ECSMutableAddComponentState *AddComponentState = ECSMutationInspectEntityAddComponents(Context, &AddComponentCount);
    for (size_t Loop = 0; Loop < AddComponentCount; Loop++)
    {
        ECSEntityAddComponents(Context, ECSProxyEntityResolve(AddComponentState[Loop].entity, NewEntities, NewEntityCount), AddComponentState[Loop].components, AddComponentState[Loop].count);
    }
    
    size_t CallbackCount;
    ECSMutableCustomCallbackState *CallbackState = ECSMutationInspectCustomCallback(Context, &CallbackCount);
    for (size_t Loop = 0; Loop < CallbackCount; Loop++)
    {
        CallbackState[Loop].callback(Context, CallbackState[Loop].data, NewEntities, NewEntityCount);
    }
    
    size_t RemoveLinkCount;
    ECSMutableRemoveLinkState *RemoveLinkState = ECSMutationInspectRemoveLink(Context, &RemoveLinkCount);
    for (size_t Loop = 0; Loop < RemoveLinkCount; Loop++)
    {
        const ECSLink *Link = RemoveLinkState[Loop].link;
        
        if (Link)
        {
            ECSEntity Entity = RemoveLinkState[Loop].left.entity;
            
            if (RemoveLinkState[Loop].right.entity)
            {
                if (Entity)
                {
                    ECSLinkRemove(Context, ECSProxyEntityResolve(Entity, NewEntities, NewEntityCount), Link, ECSProxyEntityResolve(RemoveLinkState[Loop].right.entity, NewEntities, NewEntityCount));
                }
                
                else
                {
                    Entity = RemoveLinkState[Loop].right.entity;
                    
                    Link = ECS_LINK_INVERT(Link);
                    
                    goto RemoveLinkForEntity;
                }
            }
            
            else if (Entity)
            {
            RemoveLinkForEntity:
                ECSLinkRemoveLinkForEntity(Context, ECSProxyEntityResolve(Entity, NewEntities, NewEntityCount), Link);
            }
            
            else
            {
                ECSLinkRemoveLink(Context, Link);
            }
        }
        
        else
        {
            ECSEntity Entity = RemoveLinkState[Loop].left.entity;
            
            if (RemoveLinkState[Loop].right.entity)
            {
                if (Entity)
                {
                    ECSLinkRemoveAllLinksBetweenEntities(Context, ECSProxyEntityResolve(Entity, NewEntities, NewEntityCount), ECSProxyEntityResolve(RemoveLinkState[Loop].right.entity, NewEntities, NewEntityCount));
                }
                
                else
                {
                    Entity = RemoveLinkState[Loop].right.entity;
                    goto RemoveAllLinksForEntity;
                }
            }
            
            else if (Entity)
            {
            RemoveAllLinksForEntity:
                ECSLinkRemoveAllLinksForEntity(Context, ECSProxyEntityResolve(Entity, NewEntities, NewEntityCount));
            }
        }
    }
    
    size_t DeregisterEntityCount;
    ECSProxyEntity *DeregisterEntities = ECSMutationInspectRegistryDeregister(Context, &DeregisterEntityCount);
    for (size_t Loop = 0; Loop < DeregisterEntityCount; Loop++)
    {
        // TODO: Add batched variant
        ECSRegistryDeregister(Context, ECSProxyEntityResolve(DeregisterEntities[Loop], NewEntities, NewEntityCount));
    }
    
    size_t DestroyEntityCount;
    ECSEntity *DestroyEntities = ECSMutationInspectEntityDestroy(Context, &DestroyEntityCount);
    if (DestroyEntityCount) ECSEntityDestroy(Context, DestroyEntities, DestroyEntityCount);
    
    CCMemoryZoneRestore(ECSSharedZone);
    
    atomic_store_explicit(&Context->mutations->entity.create, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->entity.remove.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->registry.add.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->registry.remove.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->registry.replace.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->link.add.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->link.remove.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->component.add.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->component.remove.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->custom.count, 0, memory_order_relaxed);
    atomic_store_explicit(&Context->mutations->shared.size, 0, memory_order_relaxed);
}
