/*
 *  Copyright (c) 2016, Stefan Johnson
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

#include "ListExpressions.h"

CCExpression CCListExpressionGetter(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        //TODO: Index could be a list of indices, which would return a new list
        CCExpression Index = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Index) == CCExpressionValueTypeInteger) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
        {
            size_t Count = CCCollectionGetCount(CCExpressionGetList(List));
            if (CCExpressionGetInteger(Index) < Count)
            {
                return CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(List), CCExpressionGetInteger(Index))));
            }
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("get", "index:integer list:list");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("get", "index:integer list:list");
    
    return Expression;
}

CCExpression CCListExpressionFlatten(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("flatten", "_:expr");
        
        return Expression;
    }
    
    CCExpression List = CCExpressionCreateList(CC_STD_ALLOCATOR);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
        {
            CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionRetain(Element) });
            }
        }
        
        else
        {
            CCOrderedCollectionAppendElement(CCExpressionGetList(List), &(CCExpression){ CCExpressionRetain(Result) });
        }
    }
    
    return List;
}
