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

#include "ECSExpression.h"
#include "ExpressionReflection.h"
#include "ExpressionMap.h"
#include "ECS.h"
#include "ECSReflect.h"
#include "ECSNameComponent.h"
#include "ECSComponentIDs.h"
#include "ECSParentChildRelationship.h"

const CCString *ECSExpressionComponentExpressionNames = NULL;

const CCDictionary *ECSExpressionComponentExpressionMaps = NULL;

CCDictionary ECSExpressionEntityRelationships = CC_STATIC_DICTIONARY(CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(ECSLink*), CCStringDestructorForDictionary, NULL, CCStringHasherForDictionary, CCStringComparatorForDictionary);

CCDictionary ECSExpressionECSContexts = CC_STATIC_DICTIONARY(CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(ECSContext*), CCStringDestructorForDictionary, NULL, CCStringHasherForDictionary, CCStringComparatorForDictionary);

CC_DICTIONARY_DECLARE(CCString, ECSComponentID);

static CCDictionary(CCString, ECSComponentID) ECSExpressionComponentIDLookup;

const CCString ECSExpressionEntityChildrenRelationshipOption = CC_STRING("children:");

void ECSExpressionRegisterComponentEvaluators(void)
{
    ECSExpressionComponentIDLookup = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(ECSComponentID), &(CCDictionaryCallbacks){
        .keyDestructor = CCStringDestructorForDictionary,
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary
    });
    
    for (ECSComponentID Loop = 0; Loop < ECS_COMPONENT_MAX; Loop++)
    {
        if (ECSExpressionComponentExpressionNames[Loop])
        {
            CCExpressionEvaluatorRegister(ECSExpressionComponentExpressionNames[Loop], ECSExpressionComponent);
            
            CCDictionarySetValue(ECSExpressionComponentIDLookup, &(CCString){ CCStringCopy(ECSExpressionComponentExpressionNames[Loop]) }, &ECSComponentIDs[Loop]);
            CCDictionarySetValue(ECSExpressionComponentIDLookup, &(CCString){ CCStringCreateByInsertingString(ECSExpressionComponentExpressionNames[Loop], 0, CC_STRING(":")) }, &ECSComponentIDs[Loop]);
        }
    }
    
    CCDictionarySetValue(ECSExpressionEntityRelationships, &ECSExpressionEntityChildrenRelationshipOption, &(const ECSLink*){ &ECSParentChildRelationship });
}

enum {
    ECSExpressionValueTypeContext = 'ecs\0',
    ECSExpressionValueTypeComponent = 'comp',
    ECSExpressionValueTypeEntity = 'enty'
};

typedef struct {
    
    ECSComponentID id;
    uint8_t data[];
} ECSExpressionComponentValue;

CCExpression ECSExpressionComponent(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
    CCString ComponentName = CCExpressionGetAtom(*Expr);
    
    if (CCStringEqual(ComponentName, CC_STRING("component")))
    {
        Expr = CCCollectionEnumeratorNext(&Enumerator);
        CCExpression Result;
        
        if ((!Expr) || (CCExpressionGetType(Result = CCExpressionEvaluate(*Expr)) == CCExpressionValueTypeAtom))
        {
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("component", "name:atom [args:expression]");
            
            return Expression;
        }
        
        ComponentName = CCExpressionGetAtom(Result);
    }
    
    ECSComponentID *ID = CCDictionaryGetValue(ECSExpressionComponentIDLookup, &ComponentName);
    
    if (!ID)
    {
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find the component %S", ComponentName);
        
        return Expression;
    }
    
    CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
    
    while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
    {
        CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(*Expr)) });
    }
    
    const size_t ComponentIndex = ECSComponentBaseIndex(*ID);
    CCDictionary Map = ECSExpressionComponentExpressionMaps[ComponentIndex];
    
    if (Map)
    {
        CCExpression Mapped = CCExpressionMap(Result, Map);
        CCExpressionDestroy(Result);
        Result = Mapped;
    }
    
    CCExpressionStateSetPrivate(Result, CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeComponent, (void*)(uintptr_t)*ID, NULL, NULL));
    
    return Result;
}

static _Bool ECSExpressionAddComponentData(ECSContext *Context, ECSEntity Entity, CCExpression Expression, _Bool IgnoreDuplicate, ECSComponentID ID)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
        
        if ((CCExpressionGetType(ArgExpr) != CCExpressionValueTypeList) || (CCCollectionGetCount(CCExpressionGetList(ArgExpr)) != 2) || (CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0)) != CCExpressionValueTypeAtom))
        {
            Expression = ArgExpr;
        }
    }
    
    CCReflectType Type = NULL;
    size_t Size = 0;
    
    if ((ID & ECSComponentStorageModifierDuplicate) && (!IgnoreDuplicate))
    {
        Type = ECSDuplicateComponentType(ID);
        Size = ECSDuplicateComponentSize(ID);
    }
    
    else
    {
        Type = ECSComponentType(ID);
        Size = ECSComponentSize(ID);
        
        ID &= ~ECSComponentStorageModifierDuplicate;
    }
    
    
    void *Data = ECSMutationSetSharedData(Context, Size);
    
    memset(Data, 0, Size);
    
    // TODO: support custom initialisation?
    
    CCMemoryZone Zone = CCMemoryZoneLocal();
    CCMemoryZoneSave(Zone);
    CCExpressionReflectionDeserialize(Expression, Type, Data, Zone, CC_STD_ALLOCATOR);
    CCMemoryZoneRestore(Zone);
    
    
    ECSMutableAddComponentState *ComponentState = ECSMutationStageEntityAddComponents(Context, 1);
    
    ComponentState->entity = Entity;
    ComponentState->count = 1;
    ComponentState->components = ECSMutationSetSharedData(Context, sizeof(ECSTypedComponent));
    ComponentState->components[0] = (ECSTypedComponent){
        .id = ID,
        .data = Data
    };
    
    return TRUE;
}

static _Bool ECSExpressionAddComponent(ECSContext *Context, ECSEntity Entity, CCExpression Expression, _Bool IgnoreDuplicate)
{
    CCExpression Private = CCExpressionStateGetPrivate(Expression);
    
    if ((Private) && (CCExpressionGetType(Private) == ECSExpressionValueTypeComponent))
    {
        return ECSExpressionAddComponentData(Context, Entity, Expression, IgnoreDuplicate, (ECSComponentID)(uintptr_t)CCExpressionGetData(Private));
    }
    
    else if (CCExpressionGetType(Expression) == CCExpressionValueTypeList)
    {
        if (CCCollectionGetCount(CCExpressionGetList(Expression)))
        {
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
            
            CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
            ECSComponentID ID;
            
            Private = CCExpressionStateGetPrivate(*Expr);
            
            if ((Private) && (CCExpressionGetType(Private) == ECSExpressionValueTypeComponent))
            {
                ID = (ECSComponentID)(uintptr_t)CCExpressionGetData(Private);
                
                if (!(ID & ECSComponentStorageModifierDuplicate)) return FALSE;
            }
            
            else return FALSE;
            
            while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
            {
                Private = CCExpressionStateGetPrivate(*Expr);
                
                if ((Private) && (CCExpressionGetType(Private) == ECSExpressionValueTypeComponent))
                {
                    const ECSComponentID ItemID = (ECSComponentID)(uintptr_t)CCExpressionGetData(Private);
                    
                    if (ItemID != ID) return FALSE;
                }
                
                else return FALSE;
            }
            
            return ECSExpressionAddComponentData(Context, Entity, Expression, IgnoreDuplicate, ID & ~ECSComponentStorageModifierDuplicate);
        }
    }
    
    return FALSE;
}

CCExpression ECSExpressionEntity(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            ECSProxyEntity Entity = ECSMutationStageEntityCreate(Context, 1);
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
            
            CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator);
            CCExpression NameExpr;
            
            if ((Expr) && (CCExpressionGetType(NameExpr = CCExpressionEvaluate(*Expr)) == CCExpressionValueTypeString))
            {
                ECSNameComponent *Name = ECSMutationSetSharedData(Context, sizeof(ECSNameComponent));
                
                *Name = CCStringCopy(CCExpressionGetString(NameExpr));
                
                
                ECSMutableAddComponentState *ComponentState = ECSMutationStageEntityAddComponents(Context, 1);
                
                ComponentState->entity = Entity;
                ComponentState->count = 1;
                ComponentState->components = ECSMutationSetSharedData(Context, sizeof(ECSTypedComponent));
                ComponentState->components[0] = (ECSTypedComponent){
                    .id = ECS_NAME_COMPONENT,
                    .data = Name
                };
                
                Expr = CCCollectionEnumeratorNext(&Enumerator);
            }
            
            for ( ; Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCExpression Result = CCExpressionEvaluate(*Expr);
                
                if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
                {
                    if (!ECSExpressionAddComponent(Context, Entity, Result, FALSE))
                    {
                        CCEnumerator RelationshipEnumerator;
                        CCCollectionGetEnumerator(CCExpressionGetList(Result), &RelationshipEnumerator);
                        
                        CCExpression *RelationshipExpr = CCCollectionEnumeratorGetCurrent(&RelationshipEnumerator);
                        
                        if ((RelationshipExpr) && (CCExpressionGetType(*RelationshipExpr) == CCExpressionValueTypeAtom))
                        {
                            CCString Relationship = CCExpressionGetAtom(*RelationshipExpr);
                            const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                            
                            if (Link)
                            {
                                for (CCExpression *ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator); ToEntityExpr; ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator))
                                {
                                    if (CCExpressionGetType(*ToEntityExpr) == ECSExpressionValueTypeEntity)
                                    {
                                        ECSEntity ToEntity = (ECSEntity)CCExpressionGetData(*ToEntityExpr);
                                        ECSMutableAddLinkState *AddLinkState = ECSMutationStageAddLinks(Context, 1);
                                        
                                        AddLinkState->left.entity = Entity;
                                        AddLinkState->left.data = NULL;
                                        AddLinkState->link = *Link;
                                        AddLinkState->right.entity = ToEntity;
                                        AddLinkState->right.data = NULL;
                                    }
                                    
                                    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S: entity)", Relationship, Relationship);
                                }
                            }
                        }
                    }
                }
            }
            
            return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeEntity, (void*)Entity, NULL, NULL);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionECS(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator);
    CCExpression NameExpr;
    
    if ((Expr) && (CCExpressionGetType(NameExpr = CCExpressionEvaluate(*Expr)) == CCExpressionValueTypeString))
    {
        CCString Name = CCExpressionGetString(NameExpr);
        
        ECSContext **Context = CCDictionaryGetValue(ECSExpressionECSContexts, &Name);
        
        if (Context)
        {
            CCExpression Result = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeContext, *Context, NULL, NULL);
            
            CCExpressionCreateState(Expression, CC_STRING("@ecs"), Result, TRUE, NULL, FALSE);
            
            for (CCExpression *ArgExpr = CCCollectionEnumeratorNext(&Enumerator); ArgExpr; ArgExpr = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCExpressionEvaluate(*ArgExpr);
            }
            
            return Result;
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Unknown ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("ecs", "name:string [args:expression]");
    
    return Expression;
}

CCExpression ECSExpressionEntityRegister(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            const size_t Count = CCCollectionGetCount(CCExpressionGetList(Expression));
            
            if ((Count >= 2) && (Count <= 4))
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity)
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    
                    if (Count >= 3)
                    {
                        CCExpression IDExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                        
                        if (CCExpressionGetType(IDExpr) == CCExpressionValueTypeString)
                        {
                            _Bool Acquire = FALSE;
                            
                            if (Count == 4)
                            {
                                CCExpression AcquireExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
                                
                                if (CCExpressionGetType(AcquireExpr) != CCExpressionValueTypeInteger) goto Error;
                                
                                Acquire = CCExpressionGetInteger(AcquireExpr);
                            }
                            
                            CCBigIntFast ID = CCBigIntFastCreate(CC_STD_ALLOCATOR);
                            CCBigIntFastSet(&ID, CCExpressionGetString(IDExpr));
                            
                            ECSMutableReplaceRegistryState *ReplacingRegistryState = ECSMutationStageRegistryReregister(Context, 1);
                            
                            ReplacingRegistryState->entity = Entity;
                            ReplacingRegistryState->id = ID;
                            ReplacingRegistryState->acquire = Acquire;
                        }
                    }
                    
                    else
                    {
                        ECSProxyEntity *Register = ECSMutationStageRegistryRegister(Context, 1);
                        
                        *Register = Entity;
                    }
                    
                    return CCExpressionRetain(EntityExpr);
                }
            }
            
        Error:;
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-register", "entity:entity [id:string] [acquire-id:boolean]");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityDeregister(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity)
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    ECSEntity *Deregister = ECSMutationStageRegistryDeregister(Context, 1);
                    
                    *Deregister = Entity;
                    
                    return CCExpressionRetain(EntityExpr);
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-deregister", "entity:entity");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityLookup(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
            {
                CCExpression IDExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(IDExpr) == CCExpressionValueTypeString)
                {
                    CCBigIntFast ID = CCBigIntFastCreate(CC_STD_ALLOCATOR);
                    CCBigIntFastSetString(&ID, CCExpressionGetString(IDExpr));
                    
                    ECSEntity Entity = ECSRegistryLookup(Context, ID);
                    CCBigIntFastDestroy(ID);
                    
                    return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeEntity, (void*)Entity, NULL, NULL);
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-lookup", "id:string");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityLink(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression OptionExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(OptionExpr) == CCExpressionValueTypeList))
                {
                    CCEnumerator RelationshipEnumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(OptionExpr), &RelationshipEnumerator);
                    
                    CCExpression *RelationshipExpr = CCCollectionEnumeratorGetCurrent(&RelationshipEnumerator);
                    
                    if ((RelationshipExpr) && (CCExpressionGetType(*RelationshipExpr) == CCExpressionValueTypeAtom))
                    {
                        CCString Relationship = CCExpressionGetAtom(*RelationshipExpr);
                        const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                        
                        if (Link)
                        {
                            ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                            
                            for (CCExpression *ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator); ToEntityExpr; ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator))
                            {
                                if (CCExpressionGetType(*ToEntityExpr) == ECSExpressionValueTypeEntity)
                                {
                                    ECSEntity ToEntity = (ECSEntity)CCExpressionGetData(*ToEntityExpr);
                                    ECSMutableAddLinkState *AddLinkState = ECSMutationStageAddLinks(Context, 1);
                                    
                                    AddLinkState->left.entity = Entity;
                                    AddLinkState->left.data = NULL;
                                    AddLinkState->link = *Link;
                                    AddLinkState->right.entity = ToEntity;
                                    AddLinkState->right.data = NULL;
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S: entity)", Relationship, Relationship);
                            }
                            
                            return CCExpressionRetain(EntityExpr);
                        }
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-link", "entity:entity (relationship:option ...:entity)");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityUnlink(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            const size_t Count = CCCollectionGetCount(CCExpressionGetList(Expression));
            
            if ((Count >= 2) && (Count <= 3))
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity)
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    
                    if (Count == 3)
                    {
                        CCExpression OptionExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                        
                        if (CCExpressionGetType(OptionExpr) == CCExpressionValueTypeList)
                        {
                            CCEnumerator RelationshipEnumerator;
                            CCCollectionGetEnumerator(CCExpressionGetList(OptionExpr), &RelationshipEnumerator);
                            
                            CCExpression *RelationshipExpr = CCCollectionEnumeratorGetCurrent(&RelationshipEnumerator);
                            
                            if ((RelationshipExpr) && (CCExpressionGetType(*RelationshipExpr) == CCExpressionValueTypeAtom))
                            {
                                if (CCCollectionGetCount(CCExpressionGetList(OptionExpr)) == 1)
                                {
                                    OptionExpr = *RelationshipExpr;
                                    
                                    goto RemoveAllForLink;
                                }
                                
                                CCString Relationship = CCExpressionGetAtom(*RelationshipExpr);
                                const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                                
                                if (Link)
                                {
                                    for (CCExpression *ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator); ToEntityExpr; ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator))
                                    {
                                        if (CCExpressionGetType(*ToEntityExpr) == ECSExpressionValueTypeEntity)
                                        {
                                            ECSEntity ToEntity = (ECSEntity)CCExpressionGetData(*ToEntityExpr);
                                            ECSMutableRemoveLinkState *RemoveLinkState = ECSMutationStageRemoveLinks(Context, 1);
                                            
                                            RemoveLinkState->link = *Link;
                                            RemoveLinkState->left.entity = Entity;
                                            RemoveLinkState->right.entity = ToEntity;
                                        }
                                        
                                        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S: entity)", Relationship, Relationship);
                                    }
                                    
                                    return CCExpressionRetain(EntityExpr);
                                }
                            }
                        }
                        
                        else if (CCExpressionGetType(OptionExpr) == CCExpressionValueTypeAtom)
                        {
                        RemoveAllForLink:;
                            CCString Relationship = CCExpressionGetAtom(OptionExpr);
                            const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                            
                            if (Link)
                            {
                                ECSMutableRemoveLinkState *RemoveLinkState = ECSMutationStageRemoveLinks(Context, 1);
                                
                                RemoveLinkState->link = *Link;
                                RemoveLinkState->left.entity = Entity;
                                RemoveLinkState->right.entity = ECS_ENTITY_NULL;
                                
                                return CCExpressionRetain(EntityExpr);
                            }
                        }
                    }
                    
                    else
                    {
                        ECSMutableRemoveLinkState *RemoveLinkState = ECSMutationStageRemoveLinks(Context, 1);
                        
                        RemoveLinkState->link = NULL;
                        RemoveLinkState->left.entity = Entity;
                        RemoveLinkState->right.entity = ECS_ENTITY_NULL;
                        
                        return CCExpressionRetain(EntityExpr);
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-unlink", "entity:entity [(relationship:option [...:entity])]");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityLinked(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression OptionExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(OptionExpr) == CCExpressionValueTypeList))
                {
                    CCEnumerator RelationshipEnumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(OptionExpr), &RelationshipEnumerator);
                    
                    CCExpression *RelationshipExpr = CCCollectionEnumeratorGetCurrent(&RelationshipEnumerator);
                    
                    if ((RelationshipExpr) && (CCExpressionGetType(*RelationshipExpr) == CCExpressionValueTypeAtom))
                    {
                        CCString Relationship = CCExpressionGetAtom(*RelationshipExpr);
                        const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                        
                        if (Link)
                        {
                            CCExpression *ToEntityExpr = CCCollectionEnumeratorNext(&RelationshipEnumerator);
                            
                            if ((ToEntityExpr) && (CCExpressionGetType(*ToEntityExpr) == ECSExpressionValueTypeEntity))
                            {
                                ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                                ECSEntity ToEntity = (ECSEntity)CCExpressionGetData(*ToEntityExpr);
                                
                                return CCExpressionCreateInteger(CC_STD_ALLOCATOR, ECSLinked(Context, Entity, *Link, ToEntity));
                            }
                        }
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-linked?", "entity:entity (relationship:option with-entity:entity)");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityLinkGet(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression RelationshipExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(RelationshipExpr) == CCExpressionValueTypeAtom))
                {
                    CCString Relationship = CCExpressionGetAtom(RelationshipExpr);
                    const ECSLink **Link = CCDictionaryGetValue(ECSExpressionEntityRelationships, &(CCString){ Relationship });
                    
                    if (Link)
                    {
                        size_t Count;
                        ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                        const ECSEntity *Entities = ECSLinkGet(Context, Entity, *Link, &Count);
                        
                        CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
                        
                        for (size_t Loop = 0; Loop < Count; Loop++)
                        {
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &(CCExpression){ CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeEntity, (void*)Entities[Loop], NULL, NULL) });
                        }
                        
                        return Result;
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-link-get", "entity:entity relationship:option");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityAdd(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression ComponentExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(ComponentExpr) == CCExpressionValueTypeList))
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    
                    if (ECSExpressionAddComponent(Context, Entity, ComponentExpr, FALSE))
                    {
                        return CCExpressionRetain(EntityExpr);
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-add", "entity:entity component:component");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityRemove(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression ComponentTypeExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(ComponentTypeExpr) == CCExpressionValueTypeAtom))
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    CCString ComponentName = CCExpressionGetAtom(ComponentTypeExpr);
                    ECSComponentID *ID = CCDictionaryGetValue(ECSExpressionComponentIDLookup, &ComponentName);
                    
                    if (ID)
                    {
                        ECSComponentID *IDs = ECSMutationSetSharedData(Context, sizeof(ECSComponentID));
                        ECSMutableRemoveComponentState *RemoveComponentState = ECSMutationStageEntityRemoveComponents(Context, 1);
                        
                        RemoveComponentState->entity = Entity;
                        RemoveComponentState->count = 1;
                        RemoveComponentState->ids = IDs;
                        
                        *IDs = *ID;
                        
                        return CCExpressionRetain(EntityExpr);
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-remove", "entity:entity component-name:atom");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityHas(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression ComponentTypeExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(ComponentTypeExpr) == CCExpressionValueTypeAtom))
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    CCString ComponentName = CCExpressionGetAtom(ComponentTypeExpr);
                    ECSComponentID *ID = CCDictionaryGetValue(ECSExpressionComponentIDLookup, &ComponentName);
                    
                    if (ID)
                    {
                        return CCExpressionCreateInteger(CC_STD_ALLOCATOR, ECSEntityHasComponent(Context, Entity, *ID));
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-has?", "entity:entity component-name:atom");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityGet(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                CCExpression ComponentTypeExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                
                if ((CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity) && (CCExpressionGetType(ComponentTypeExpr) == CCExpressionValueTypeAtom))
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    CCString ComponentName = CCExpressionGetAtom(ComponentTypeExpr);
                    ECSComponentID *ID = CCDictionaryGetValue(ECSExpressionComponentIDLookup, &ComponentName);
                    
                    if (ID)
                    {
                        const void *Data = ECSEntityGetComponent(Context, Entity, *ID);
                        const CCReflectType Type = ECSComponentType(*ID);
                        
                        CCMemoryZone Zone = CCMemoryZoneLocal();
                        CCMemoryZoneSave(Zone);
                        CCExpression Result = CCExpressionReflectionSerialize(Type, Data, Zone);
                        CCMemoryZoneRestore(Zone);
                        
                        if (*ID & ECSComponentStorageModifierDuplicate)
                        {
                            const ECSComponentID ItemID = *ID & ~ECSComponentStorageModifierDuplicate;
                            
                            CCEnumerator Enumerator;
                            CCCollectionGetEnumerator(CCExpressionGetList(Result), &Enumerator);
                            
                            for (CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
                            {
                                CCExpressionStateSetPrivate(*Expr, CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeComponent, (void*)(uintptr_t)ItemID, NULL, NULL));
                            }
                        }
                        
                        CCExpressionStateSetPrivate(Result, CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeComponent, (void*)(uintptr_t)*ID, NULL, NULL));
                        
                        return Result;
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-get", "entity:entity component-name:atom");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntitySet(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity)
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    CCExpression ComponentExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                    
                    if (ECSExpressionAddComponent(Context, Entity, ComponentExpr, TRUE))
                    {
                        return CCExpressionRetain(EntityExpr);
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-set", "entity:entity component:component");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}

CCExpression ECSExpressionEntityDestroy(CCExpression Expression)
{
    CCExpression ContextExpr = CCExpressionGetState(Expression, CC_STRING("@ecs"));
    
    if ((ContextExpr) && (CCExpressionGetType(ContextExpr) == ECSExpressionValueTypeContext))
    {
        ECSContext *Context = CCExpressionGetData(ContextExpr);
        
        if (Context)
        {
            if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
            {
                CCExpression EntityExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
                
                if (CCExpressionGetType(EntityExpr) == ECSExpressionValueTypeEntity)
                {
                    ECSEntity Entity = (ECSEntity)CCExpressionGetData(EntityExpr);
                    
                    ECSEntity *Destroy = ECSMutationStageEntityDestroy(Context, 1);
                    
                    *Destroy = Entity;
                    
                    return CCExpressionRetain(EntityExpr);
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-destroy", "entity:entity");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid ecs context");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Missing ecs context");
    
    return Expression;
}
