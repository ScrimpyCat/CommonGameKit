/*
 *  Copyright (c) 2015, Stefan Johnson
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
#include "StateExpressions.h"

CCExpression CCStateExpressionCreateState(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if ((ArgCount >= 1) && (ArgCount <= 3))
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            CCExpression Value = ArgCount >= 2 ? CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2)) : NULL;
            CCExpression Invalidator = NULL;
            
            if (ArgCount == 3)
            {
                CCExpression InvalidatorOption = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
                
                if (CCExpressionGetType(InvalidatorOption) == CCExpressionValueTypeList)
                {
                    CCExpression Option = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(InvalidatorOption), 0);
                    
                    if ((CCExpressionGetType(Option) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("invalidate:"))))
                    {
                        Invalidator = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(InvalidatorOption), 1);
                    }
                    
                    else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("invalidate", "invalidator:expr");
                }
                
                else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("invalidate", "invalidator:expr");
            }
            
            CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Name), Value, TRUE, Invalidator, TRUE);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("state!", "name:string [value] [invalidator]");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("state!", "name:string [value] [invalidator]");
    
    return NULL;
}

CCExpression CCStateExpressionCreateNamespace(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeAtom)
        {
            CCExpressionCreateState(Expression->state.super, CC_STRING("@namespace"), Name, TRUE, NULL, FALSE);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("namespace!", "name:atom");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("namespace!", "name:atom");
    
    return NULL;
}

CCExpression CCStateExpressionCreateEnum(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    int32_t Index = 0;
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); Index++)
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeString)
        {
            CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Result), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Index), FALSE, NULL, FALSE);
        }
        
        else if ((CCExpressionGetType(Result) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Result)) == 2))
        {
            CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), NewIndex = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1));
            
            if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(NewIndex) == CCExpressionValueTypeInteger))
            {
                Index = CCExpressionGetInteger(NewIndex);
                CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Name), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Index), FALSE, NULL, FALSE);
            }
            
            else
            {
                CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("enum!", "[name:string] [(name:string index:integer)]");
                break;
            }
        }
        
        else
        {
            CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("enum!", "[name:string] [(name:string index:integer)]");
            break;
        }
    }
    
    return NULL;
}

CCExpression CCStateExpressionSuper(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
        
        CCExpression Super = Expression->state.super;
        while ((Super) && (!Super->state.values)) Super = Super->state.super; //first state
        if (Super)
        {
            for (Super = Super->state.super; (Super) && (!Super->state.values); Super = Super->state.super); //second state
            
            if (Super)
            {
                Expr->state.super = Super;
                
                return CCExpressionRetain(CCExpressionEvaluate(Expr));
            }
        }
    }
    
    else if (ArgCount == 2)
    {
        CCExpression Namespace = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
        
        if (CCExpressionGetType(Namespace) == CCExpressionValueTypeAtom)
        {
            CCExpression Super = Expression->state.super;
            if (Super)
            {
                for ( ; Super; Super = Super->state.super)
                {
                    CCExpression Name = CCExpressionGetStateStrict(Super, CC_STRING("@namespace"));
                    if ((Name) && (CCExpressionGetType(Name) == CCExpressionValueTypeAtom) && (CCStringEqual(CCExpressionGetAtom(Name), CCExpressionGetAtom(Namespace))))
                    {
                        CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
                        Expr->state.super = Super;
                        
                        return CCExpressionRetain(CCExpressionEvaluate(Expr));
                    }
                }
            }
            
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find namespace (%S)", CCExpressionGetAtom(Namespace));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("super", "expression:expr [namespace:atom]");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("super", "expression:expr [namespace:atom]");
    
    return Expression;
}

CCExpression CCStateExpressionStrictSuper(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
        Expr->state.super = Expression->state.super->state.super;
        
        return CCExpressionRetain(CCExpressionEvaluate(Expr));
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("strict-super", "expression:expr");
    
    return Expression;
}
