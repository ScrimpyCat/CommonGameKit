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
#include "EntityExpressions.h"
#include "ComponentExpressions.h"
#include "Entity.h"
#include "EntityManager.h"
#include "RelationParentComponent.h"
#include "ComponentSystem.h"

static void CCEntityExpressionEntityAddChildren(CCEntity Entity, CCEnumerator *Enumerator)
{
    for (CCExpression *Expr = CCCollectionEnumeratorGetCurrent(Enumerator); Expr; Expr = CCCollectionEnumeratorNext(Enumerator))
    {
        if (CCExpressionGetType(*Expr) == CCEntityExpressionValueTypeEntity)
        {
            CCComponent ParentComponent = CCComponentCreate(CC_RELATION_PARENT_COMPONENT_ID);
            CCRelationParentComponentSetParent(ParentComponent, Entity);
            CCEntityAttachComponent(CCExpressionGetData(*Expr), ParentComponent);
            CCComponentSystemAddComponent(ParentComponent);
        }
        
        else if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
        {
            CCEnumerator InnerEnumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(*Expr), &InnerEnumerator);
            
            CCEntityExpressionEntityAddChildren(Entity, &InnerEnumerator);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("children", "entity");
    }
}

static void CCEntityExpressionEntityAddNestedComponents(CCEntity Entity, CCEnumerator *Enumerator)
{
    for (CCExpression *Expr = CCCollectionEnumeratorGetCurrent(Enumerator); Expr; Expr = CCCollectionEnumeratorNext(Enumerator))
    {
        if (CCExpressionGetType(*Expr) == CCComponentExpressionValueTypeComponent)
        {
            CCExpressionChangeOwnership(*Expr, NULL, NULL);
            CCEntityAttachComponent(Entity, CCExpressionGetData(*Expr));
        }
        
        else if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
        {
            CCEnumerator InnerEnumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(*Expr), &InnerEnumerator);
            
            CCEntityExpressionEntityAddNestedComponents(Entity, &InnerEnumerator);
        }
    }
}

CCExpression CCEntityExpressionEntity(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 2)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            CCString ID = 0;
            
            CCEntity Entity = CCEntityCreate(CCExpressionGetString(Name), CC_STD_ALLOCATOR);
            for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCExpression Component = CCExpressionEvaluate(*Expr);
                if (CCExpressionGetType(Component) == CCComponentExpressionValueTypeComponent)
                {
                    CCExpressionChangeOwnership(Component, NULL, NULL);
                    CCEntityAttachComponent(Entity, CCExpressionGetData(Component));
                }
                
                else if ((CCExpressionGetType(Component) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Component))))
                {
                    CCEnumerator OptionEnumerator;
                    CCCollectionGetEnumerator(CCExpressionGetList(Component), &OptionEnumerator);
                    
                    CCExpression Option = *(CCExpression*)CCCollectionEnumeratorGetCurrent(&OptionEnumerator);
                    if (CCExpressionGetType(Option) == CCExpressionValueTypeAtom)
                    {
                        if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("children:")))
                        {
                            CCCollectionEnumeratorNext(&OptionEnumerator);
                            CCEntityExpressionEntityAddChildren(Entity, &OptionEnumerator);
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("id:")))
                        {
                            if (CCCollectionGetCount(CCExpressionGetList(Component)) == 2)
                            {
                                CCExpression IDExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Component), 1);
                                
                                if (CCExpressionGetType(IDExpr) == CCExpressionValueTypeString)
                                {
                                    ID = CCExpressionGetString(IDExpr);
                                }
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("id", "string");
                        }
                    }
                    
                    else
                    {
                        CCEntityExpressionEntityAddNestedComponents(Entity, &OptionEnumerator);
                    }
                }
            }
            
            if (ID) CCEntityManagerAddEntityWithID(Entity, ID);
            else CCEntityManagerAddEntity(Entity);
            
            return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCEntityExpressionValueTypeEntity, Entity, NULL, NULL);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity", "name:string [...:component]");
    
    return Expression;
}

CCExpression CCEntityExpressionEntityLookup(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression IDExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
        if (CCExpressionGetType(IDExpr) == CCExpressionValueTypeString)
        {
            CCString ID = CCExpressionGetString(IDExpr);
            CCEntity Entity = CCEntityManagerGetEntity(ID);
            
            if (!Entity)
            {
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("No entity exists for id (%S)", ID);
                
                return CCExpressionCreateNull(CC_STD_ALLOCATOR);
            }
            
            return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCEntityExpressionValueTypeEntity, Entity, NULL, NULL);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("entity-lookup", "id:string");
    
    return Expression;
}
