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

#include "ControlFlowExpressions.h"

CCExpression CCControlFlowExpressionBegin(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression Result = Expression;
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        Result = CCExpressionEvaluate(*Expr);
    }
    
    return Result ? CCExpressionRetain(Result) : Expression;
}

CCExpression CCControlFlowExpressionBranch(CCExpression Expression)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if ((ArgCount >= 2) && (ArgCount <= 3))
    {
        CCExpression Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        
        _Bool Predicate = FALSE;
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger) Predicate = CCExpressionGetInteger(Result);
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("if", "predicate:integer true:expr [false:expr]");
        
        if ((Predicate) || (ArgCount == 3)) Expr = CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), Predicate ? 2 : 3)));
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("if", "predicate:integer true:expr [false:expr]");
    
    return Expr;
}

CCExpression CCControlFlowExpressionLoop(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if (ArgCount == 3)
    {
        CCExpression Var = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Var) == CCExpressionValueTypeString)
        {
            CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
            CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3);
            
            if (CCExpressionGetType(List) == CCExpressionValueTypeList)
            {
                if (!CCExpressionGetStateStrict(Expression, CCExpressionGetString(Var)))
                {
                    CCExpressionCreateState(Expression, CCExpressionGetString(Var), CCExpressionCreateNull(CC_STD_ALLOCATOR), FALSE, NULL, FALSE);
                }
                
                CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CC_COLLECTION_FOREACH(CCExpression, Item, CCExpressionGetList(List))
                {
                    CCExpressionSetState(Expression, CCExpressionGetString(Var), Item, TRUE);
                    
                    CCExpression ItemExpr = CCExpressionCopy(Expr); //So we can correctly handle expressions with cached internal state
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(ItemExpr)) });
                    CCExpressionDestroy(ItemExpr);
                }
                
                return Result;
            }
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("loop", "var:string list:list iteration:expr");
    
    return Expression;
}

CCExpression CCControlFlowExpressionRepeat(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if (ArgCount == 3)
    {
        CCExpression Var = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Var) == CCExpressionValueTypeString)
        {
            CCExpression CountExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
            CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3);
            
            if (CCExpressionGetType(CountExpr) == CCExpressionValueTypeInteger)
            {
                if (!CCExpressionGetStateStrict(Expression, CCExpressionGetString(Var)))
                {
                    CCExpressionCreateState(Expression, CCExpressionGetString(Var), CCExpressionCreateNull(CC_STD_ALLOCATOR), FALSE, NULL, FALSE);
                }
                
                CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
                for (size_t Loop = 0, Count = CCExpressionGetInteger(CountExpr); Loop < Count; Loop++)
                {
                    CCExpressionSetState(Expression, CCExpressionGetString(Var), CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)Loop), FALSE);
                    
                    CCExpression ItemExpr = CCExpressionCopy(Expr); //So we can correctly handle expressions with cached internal state
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(ItemExpr)) });
                    CCExpressionDestroy(ItemExpr);
                }
                
                return Result;
            }
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("repeat", "var:string count:integer iteration:expr");
    
    return Expression;
}

CCExpression CCControlFlowExpressionAny(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
        
        return Expression;
    }
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            if (CCExpressionGetInteger(Result)) return CCExpressionCreateInteger(CC_STD_ALLOCATOR, TRUE);
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
}

CCExpression CCControlFlowExpressionAll(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
        
        return Expression;
    }
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            if (!CCExpressionGetInteger(Result)) return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, TRUE);
}
