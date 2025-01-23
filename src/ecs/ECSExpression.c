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
    
    CCExpressionStateSetPrivate(Result, CCExpressionCreateCustomType(CC_STD_ALLOCATOR, ECSExpressionValueTypeComponent, ID, NULL, NULL));
    
    return Result;
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
                    CCExpression Private = CCExpressionStateGetPrivate(Result);
                    
                    if ((Private) && (CCExpressionGetType(Private) == ECSExpressionValueTypeComponent))
                    {
                        if (CCCollectionGetCount(CCExpressionGetList(Result)) == 1)
                        {
                            CCExpression ArgExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0);
                            
                            if ((CCExpressionGetType(ArgExpr) != CCExpressionValueTypeList) || (CCCollectionGetCount(CCExpressionGetList(ArgExpr)) != 2) || (CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(ArgExpr), 0)) != CCExpressionValueTypeAtom))
                            {
                                Result = ArgExpr;
                            }
                        }
                        
                        ECSComponentID *ID = CCExpressionGetData(Private);
                        CCReflectType Type = NULL;
                        size_t Size = 0;
                        
                        if (*ID & ECSComponentStorageModifierDuplicate)
                        {
                            Type = ECSDuplicateComponentType(*ID);
                            Size = ECSDuplicateComponentSize(*ID);
                        }
                        
                        else
                        {
                            Type = ECSComponentType(*ID);
                            Size = ECSComponentSize(*ID);
                        }
                        
                        
                        void *Data = ECSMutationSetSharedData(Context, Size);
                        
                        memset(Data, 0, Size);
                        
                        // TODO: support custom initialisation?
                        
                        CCMemoryZone Zone = CCMemoryZoneLocal();
                        CCMemoryZoneSave(Zone);
                        CCExpressionReflectionDeserialize(Result, Type, Data, Zone, CC_STD_ALLOCATOR);
                        CCMemoryZoneRestore(Zone);
                        
                        
                        ECSMutableAddComponentState *ComponentState = ECSMutationStageEntityAddComponents(Context, 1);
                        
                        ComponentState->entity = Entity;
                        ComponentState->count = 1;
                        ComponentState->components = ECSMutationSetSharedData(Context, sizeof(ECSTypedComponent));
                        ComponentState->components[0] = (ECSTypedComponent){
                            .id = *ID,
                            .data = Data
                        };
                    }
                    
                    else
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
                            CCBigIntFastSetString(&ID, CCExpressionGetString(IDExpr));
                            
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
                        
                        return EntityExpr;
                    }
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

CCExpression ECSExpressionEntityUnregister(CCExpression Expression)
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
                    ECSEntity *Unregister = ECSMutationStageRegistryDeregister(Context, 1);
                    
                    *Unregister = Entity;
                    
                    return EntityExpr;
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-unregister", "entity:entity");
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
