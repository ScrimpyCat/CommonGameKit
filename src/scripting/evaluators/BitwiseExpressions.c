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

#include "BitwiseExpressions.h"


CCExpression CCBitwiseExpressionNot(CCExpression Expression)
{
    CCExpression Expr = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, !CCExpressionGetInteger(Result));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("not", "value:integer");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("not", "value:integer");
    
    return Expr;
}

CCExpression CCBitwiseExpressionAnd(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
        
        return Expression;
    }
    
    uint32_t SumI = UINT32_MAX;
    
    CCArray Vector = NULL;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            SumI &= CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
        {
            if (Vector)
            {
                const size_t Count = CCArrayGetCount(Vector);
                size_t Index = 0;
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                {
                    uint32_t Value = 0;
                    if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                    {
                        Value = CCExpressionGetInteger(Element);
                    }
                    
                    else
                    {
                        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
                        
                        if (Vector) CCArrayDestroy(Vector);
                        
                        return Expression;
                    }
                    
                    if (Index < Count)
                    {
                        uint32_t *Sum = CCArrayGetElementAtIndex(Vector, Index);
                        *Sum &= Value;
                    }
                    
                    else CCArrayAppendElement(Vector, &Value);
                    
                    Index++;
                }
            }
            
            else
            {
                Vector = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(uint32_t), 4);
                
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                {
                    uint32_t Value = 0;
                    if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                    {
                        Value = CCExpressionGetInteger(Element);
                    }
                    
                    else
                    {
                        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
                        
                        if (Vector) CCArrayDestroy(Vector);
                        
                        return Expression;
                    }
                    
                    CCArrayAppendElement(Vector, &Value);
                }
            }
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:integer:list");
            
            if (Vector) CCArrayDestroy(Vector);
            
            return Expression;
        }
    }
    
    
    if (!Vector) return CCExpressionCreateInteger(CC_STD_ALLOCATOR, SumI);
    
    
    CCExpression Vec = CCExpressionCreateList(CC_STD_ALLOCATOR);
    for (size_t Loop = 0, Count = CCArrayGetCount(Vector); Loop < Count; Loop++)
    {
        uint32_t *Sum = CCArrayGetElementAtIndex(Vector, Loop);
        
        CCOrderedCollectionAppendElement(CCExpressionGetList(Vec), &(CCExpression){ CCExpressionCreateInteger(CC_STD_ALLOCATOR, *Sum & SumI) });
    }
    
    CCArrayDestroy(Vector);
    
    return Vec;
}
