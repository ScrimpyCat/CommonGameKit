/*
 *  Copyright (c) 2018, Stefan Johnson
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

#include "ScriptableInterfaceSystem.h"
#include <threads.h>
#include <CommonC/Common.h>
#include "ScriptableInterfaceDynamicFieldComponent.h"
#include "ComponentExpressions.h"

static _Bool CCScriptableInterfaceSystemTryLock(CCComponentSystemHandle *System);
static void CCScriptableInterfaceSystemLock(CCComponentSystemHandle *System);
static void CCScriptableInterfaceSystemUnlock(CCComponentSystemHandle *System);
static _Bool CCScriptableInterfaceSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id);
static void CCScriptableInterfaceSystemUpdate(CCComponentSystemHandle *System, void *Context, CCCollection Components);
static void CCScriptableInterfaceSystemRead(CCComponentSystemHandle *System, void *Context, CCCollection Components);

static mtx_t Lock[CCComponentSystemExecutionMax];
static CCConcurrentIndexMap ReadableComponentReferences[CCComponentSystemExecutionMax];
static CCDictionary ReadableComponentIndexes[CCComponentSystemExecutionMax];
static CCQueue AvailableIndexes[CCComponentSystemExecutionMax];
static CCConcurrentGarbageCollector GC;
void CCScriptableInterfaceSystemRegister(void)
{
    for (int Loop = 0; Loop < CCComponentSystemExecutionMax; Loop++)
    {
        int err;
        if ((err = mtx_init(&Lock[Loop], mtx_plain | mtx_recursive)) != thrd_success)
        {
            CC_LOG_ERROR("Failed to create scriptable interface system lock (%d)", err);
            exit(EXIT_FAILURE); //TODO: How should we handle this?
        }
        
        ReadableComponentReferences[Loop] = CCConcurrentIndexMapCreate(CC_STD_ALLOCATOR, sizeof(CCComponent), 32, CCConcurrentGarbageCollectorCreate(CC_STD_ALLOCATOR, CCEpochGarbageCollector));
        ReadableComponentIndexes[Loop] = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeMedium, sizeof(CCComponent), sizeof(size_t), NULL);
        AvailableIndexes[Loop] = CCQueueCreate(CC_STD_ALLOCATOR);
        
        CCComponentSystemRegister(CC_SCRIPTABLE_INTERFACE_SYSTEM_ID + Loop, Loop, (CCComponentSystemUpdateCallback)(Loop == (CCComponentSystemExecutionTypeUpdate & CCComponentSystemExecutionTypeMask) ? CCScriptableInterfaceSystemUpdate : CCScriptableInterfaceSystemRead), NULL, CCScriptableInterfaceSystemHandlesComponent, NULL, NULL, CCScriptableInterfaceSystemTryLock, CCScriptableInterfaceSystemLock, CCScriptableInterfaceSystemUnlock);
    }
    
    GC = CCConcurrentGarbageCollectorCreate(CC_STD_ALLOCATOR, CCEpochGarbageCollector);
}

void CCScriptableInterfaceSystemDeregister(void)
{
    CCConcurrentGarbageCollectorDestroy(GC);
    
    for (int Loop = 0; Loop < CCComponentSystemExecutionMax; Loop++)
    {
        mtx_destroy(&Lock[Loop]);
        
        CCComponent Scriptable;
        for (size_t Loop2 = 0; CCConcurrentIndexMapGetElementAtIndex(ReadableComponentReferences[Loop], Loop2, &Scriptable); Loop2++)
        {
            CCComponentDestroy(Scriptable);
        }
        
        CCConcurrentIndexMapDestroy(ReadableComponentReferences[Loop]);
        CCDictionaryDestroy(ReadableComponentIndexes[Loop]);
        CCQueueDestroy(AvailableIndexes[Loop]);
        
        CCComponentSystemDeregister(CC_SCRIPTABLE_INTERFACE_SYSTEM_ID + Loop, Loop);
    }
}

static _Bool CCScriptableInterfaceSystemTryLock(CCComponentSystemHandle *System)
{
    int err = mtx_trylock(&Lock[System->executionType & CCComponentSystemExecutionTypeMask]);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
    
    return err == thrd_success;
}

static void CCScriptableInterfaceSystemLock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_lock(&Lock[System->executionType & CCComponentSystemExecutionTypeMask])) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
}

static void CCScriptableInterfaceSystemUnlock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_unlock(&Lock[System->executionType & CCComponentSystemExecutionTypeMask])) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock animation system (%d)", err);
    }
}

static _Bool CCScriptableInterfaceSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id)
{
    return ((id & CC_COMPONENT_SYSTEM_FLAG_MASK) == CC_SCRIPTABLE_INTERFACE_COMPONENT_FLAG) && ((System->executionType & CCComponentSystemExecutionTypeMask) == (CCComponentSystemExecutionTypeUpdate & CCComponentSystemExecutionTypeMask));
}

static void CCScriptableInterfaceSystemUpdate(CCComponentSystemHandle *System, void *Context, CCCollection Components)
{
    CCConcurrentGarbageCollectorBegin(GC);
    
    CCCollection Removed = CCComponentSystemGetRemovedComponentsForSystem(System->id);
    CC_COLLECTION_FOREACH(CCComponent, Scriptable, Removed)
    {
        if (CCComponentGetID(Scriptable) == CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID)
        {
            CCComponent Target = CCScriptableInterfaceDynamicFieldComponentGetTarget(Scriptable);
            if (Target)
            {
                CCComponentSystemHandle *System = CCComponentSystemHandlesComponentID(CCComponentGetID(Target));
                if (System)
                {
                    const size_t Instance = System->executionType & CCComponentSystemExecutionTypeMask;
                    CCDictionaryEntry Entry = CCDictionaryFindKey(ReadableComponentIndexes[Instance], &Scriptable);
                    if (Entry)
                    {
                        size_t *Index = CCDictionaryGetEntry(ReadableComponentIndexes[Instance], Entry);
                        if (Index)
                        {
                            CCConcurrentIndexMapReplaceElementAtIndex(ReadableComponentReferences[Instance], *Index, &(CCComponent){ NULL }, NULL);
                            
                            CCQueuePush(AvailableIndexes[Instance], CCQueueCreateNode(CC_DEBUG_ALLOCATOR, sizeof(size_t), &Index));
                            
                            CCConcurrentGarbageCollectorManage(GC, Scriptable, CCComponentDestroy);
                        }
                        
                        CCDictionaryRemoveEntry(ReadableComponentIndexes[Instance], Entry);
                    }
                }
            }
        }
    }
    CCCollectionDestroy(Removed);
    
    CCConcurrentGarbageCollectorEnd(GC);
    
    CCCollection Added = CCComponentSystemGetAddedComponentsForSystem(System->id);
    CC_COLLECTION_FOREACH(CCComponent, Scriptable, Added)
    {
        if (CCComponentGetID(Scriptable) == CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID)
        {
            CCComponent Target = CCScriptableInterfaceDynamicFieldComponentGetTarget(Scriptable);
            if (Target)
            {
                CCComponentSystemHandle *System = CCComponentSystemHandlesComponentID(CCComponentGetID(Target));
                if (System)
                {
                    const size_t Instance = System->executionType & CCComponentSystemExecutionTypeMask;
                    
                    CCRetain(Scriptable);
                    
                    size_t Index;
                    CCQueueNode *Node = CCQueuePop(AvailableIndexes[Instance]);
                    if (Node)
                    {
                        Index = *(size_t*)CCQueueGetNodeData(Node);
                        CCQueueDestroyNode(Node);
                        
                        CCConcurrentIndexMapReplaceElementAtIndex(ReadableComponentReferences[Instance], Index, &Scriptable, NULL);
                    }
                    
                    else Index = CCConcurrentIndexMapAppendElement(ReadableComponentReferences[Instance], &Scriptable);
                    
                    CCDictionarySetValue(ReadableComponentIndexes[Instance], &Scriptable, &Index);
                }
            }
        }
    }
    CCCollectionDestroy(Added);
    
    CC_COLLECTION_FOREACH(CCComponent, Scriptable, Components)
    {
        if (CCComponentGetID(Scriptable) == CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID)
        {
            CCExpression ReferenceState = CCScriptableInterfaceDynamicFieldComponentGetReferenceState(Scriptable);
            if (ReferenceState)
            {
                CCScriptableInterfaceDynamicFieldComponentSetState(Scriptable, ReferenceState);
            }
        }
    }
}

static void CCScriptableInterfaceSystemRead(CCComponentSystemHandle *System, void *Context, CCCollection Components)
{
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(System->id));
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(System->id));
    
    const size_t Instance = System->executionType & CCComponentSystemExecutionTypeMask;
    
    CCConcurrentGarbageCollectorBegin(GC);
    
    CCComponent Scriptable;
    for (size_t Loop = 0; CCConcurrentIndexMapGetElementAtIndex(ReadableComponentReferences[Instance], Loop, &Scriptable); Loop++)
    {
        if (Scriptable)
        {
            CCComponent Target = CCScriptableInterfaceDynamicFieldComponentGetTarget(Scriptable);
            CCExpression Field = CCScriptableInterfaceDynamicFieldComponentGetField(Scriptable);
            CCExpression State = CCScriptableInterfaceDynamicFieldComponentGetState(Scriptable);
            
            if ((Target) && (Field) && (State))
            {
                if (CCComponentGetIsManaged(Target))
                {
                    const CCComponentExpressionDescriptor *Descriptor = CCComponentExpressionDescriptorForID(CCComponentGetID(Target));
                    if (Descriptor)
                    {
                        CCExpressionStateSetSuper(Field, State);
                        
                        Descriptor->deserialize(Target, CCExpressionEvaluate(Field), FALSE);
                    }
                }
                
                else if (CCRefCount(Target) == 1) CCComponentSystemRemoveComponent(Scriptable);
            }
        }
    }
    
    CCConcurrentGarbageCollectorEnd(GC);
}
