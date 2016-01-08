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

#include "StateExpressions.h"

CCExpression CCStateExpressionCreateState(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if ((ArgCount == 1) || (ArgCount == 2))
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString) CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Name), ArgCount == 2 ? CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2)) : NULL, TRUE);
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("state!", "name:string [value]");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("state!", "name:string [value]");
    
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
            CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Result), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Index), FALSE);
        }
        
        else if ((CCExpressionGetType(Result) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Result)) == 2))
        {
            CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), NewIndex = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1));
            
            if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(NewIndex) == CCExpressionValueTypeInteger))
            {
                Index = CCExpressionGetInteger(NewIndex);
                CCExpressionCreateState(Expression->state.super, CCExpressionGetString(Name), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Index), FALSE);
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
                
                return CCExpressionCopy(CCExpressionEvaluate(Expr));
            }
        }
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("super", "expression:expr");
    
    return Expression;
}

CCExpression CCStateExpressionStrictSuper(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
        Expr->state.super = Expression->state.super->state.super;
        
        return CCExpressionCopy(CCExpressionEvaluate(Expr));
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("strict-super", "expression:expr");
    
    return Expression;
}
