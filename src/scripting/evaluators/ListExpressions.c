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

static _Bool CCListExpressionGetElement(CCCollection List, CCExpression IndexExpr, CCExpression *Element)
{
    if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
    {
        const int32_t Index = CCExpressionGetInteger(IndexExpr);
        const size_t Count = CCCollectionGetCount(List);
        if (Index < Count)
        {
            *Element = CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Index)));
            return TRUE;
        }
        
        else if ((Index + Count) < Count)
        {
            *Element = CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Index + Count)));
            return TRUE;
        }
    }
    
    return FALSE;
}

CCExpression CCListExpressionGetter(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression IndexExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression ListExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if (CCExpressionGetType(ListExpr) == CCExpressionValueTypeList)
        {
            CCCollection List = CCExpressionGetList(ListExpr);
            CCExpression Element;
            if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeList)
            {
                CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(IndexExpr))
                {
                    if (!CCListExpressionGetElement(List, Expr, &Element))
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Index should be a list of integers.");
                        
                        CCExpressionDestroy(Result);
                        Result = NULL;
                        break;
                    }
                    
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &Element);
                }
                
                if (Result) return Result;
            }
            
            else if (CCListExpressionGetElement(List, IndexExpr, &Element)) return Element;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("get", "index:integer|list list:list");
    
    return Expression;
}

CCExpression CCListExpressionSetter(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 3)
    {
        //TODO: Index could be a list of indices, which would set a list of elements
        CCExpression Index = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Index) == CCExpressionValueTypeInteger) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
        {
            size_t Count = CCCollectionGetCount(CCExpressionGetList(List));
            if (CCExpressionGetInteger(Index) < Count)
            {
                List = CCExpressionCopy(List);
                CCOrderedCollectionReplaceElementAtIndex(CCExpressionGetList(List), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3))) }, CCExpressionGetInteger(Index));
                return List;
            }
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("set", "index:integer list:list value:expr");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("set", "index:integer list:list value:expr");
    
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

CCExpression CCListExpressionParts(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Chunks = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Chunks) == CCExpressionValueTypeInteger) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
        {
            const size_t Count = CCCollectionGetCount(CCExpressionGetList(List)), ChunkCount = CCExpressionGetInteger(Chunks);
            
            size_t ItemsPerChunk = ChunkCount ? Count / ChunkCount : 0;
            if (ItemsPerChunk * ChunkCount < Count) ItemsPerChunk++;
            
            CCExpression Parts = CCExpressionCreateList(CC_STD_ALLOCATOR);
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(List), &Enumerator);
            
            for (size_t Loop = 0; Loop < ChunkCount; Loop++)
            {
                CCExpression Part = CCExpressionCreateList(CC_STD_ALLOCATOR);
                
                for (size_t Loop2 = 0; Loop2 < ItemsPerChunk; Loop2++)
                {
                    const size_t Index = (Loop * ItemsPerChunk) + Loop2;
                    if (Index >= Count) break;
                    
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Part), &(CCExpression){ CCExpressionRetain(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(List), Index)) });
                }
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Parts), &(CCExpression){ Part });
            }
            
            return Parts;
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("parts", "chunks:integer list:list");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("parts", "chunks:integer list:list");
    
    return Expression;
}

CCExpression CCListExpressionSplit(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Indexes = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Indexes) == CCExpressionValueTypeList) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
        {
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Indexes), &Enumerator);
            
            CCExpression *SplitIndex = CCCollectionEnumeratorGetCurrent(&Enumerator);
            if ((SplitIndex) && (CCExpressionGetType(*SplitIndex) != CCExpressionValueTypeInteger))
            {
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of split: indexes must contain integers only");
                
                return Expression;
            }
            
            CCExpression Parts = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCExpression Part = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Parts), &(CCExpression){ Part });
            
            size_t Index = 0;
            CC_COLLECTION_FOREACH(CCExpression, Item, CCExpressionGetList(List))
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Part), &(CCExpression){ CCExpressionRetain(Item) });
                
                if ((SplitIndex) && (CCExpressionGetInteger(*SplitIndex) == Index))
                {
                    Part = CCExpressionCreateList(CC_STD_ALLOCATOR);
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Parts), &(CCExpression){ Part });
                    
                    SplitIndex = CCCollectionEnumeratorNext(&Enumerator);
                    if ((SplitIndex) && ((CCExpressionGetType(*SplitIndex) != CCExpressionValueTypeInteger) || (CCExpressionGetInteger(*SplitIndex) <= Index)))
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of split: indexes must contain a sorted list of integers only");
                        
                        CCExpressionDestroy(Parts);
                        
                        return Expression;
                    }
                }
                
                Index++;
            }
            
            return Parts;
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("split", "indexes:list list:list");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("split", "indexes:list list:list");
    
    return Expression;
}

CCExpression CCListExpressionCount(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(List) == CCExpressionValueTypeList)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)CCCollectionGetCount(CCExpressionGetList(List)));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("split", "list:list");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("count", "list:list");
    
    return Expression;
}
