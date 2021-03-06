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

#define CC_QUICK_COMPILE
#include "ListExpressions.h"

static _Bool CCListExpressionGetElement(CCCollection(CCExpression) List, CCExpression IndexExpr, CCExpression *Element, CCExpression DefaultExpr)
{
    if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
    {
        const int32_t Index = CCExpressionGetInteger(IndexExpr);
        const size_t Count = CCCollectionGetCount(List);
        if (Index < Count)
        {
            *Element = CCExpressionRetain(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Index));
            return TRUE;
        }
        
        else if ((Index + Count) < Count)
        {
            *Element = CCExpressionRetain(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(List, Index + Count));
            return TRUE;
        }
        
        else if (DefaultExpr)
        {
            *Element = CCExpressionRetain(DefaultExpr);
            return TRUE;
        }
    }
    
    else if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeAtom)
    {
        CC_COLLECTION_FOREACH(CCExpression, Item, List)
        {
            if (CCExpressionGetType(Item) == CCExpressionValueTypeList)
            {
                if (CCCollectionGetCount(CCExpressionGetList(Item)) >= 2)
                {
                    CCExpression Tag = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Item), 0);
                    if (CCExpressionGetType(Tag) == CCExpressionValueTypeAtom)
                    {
                       if (CCStringEqual(CCExpressionGetAtom(IndexExpr), CCExpressionGetAtom(Tag)))
                       {
                           *Element = CCExpressionRetain(Item);
                           return TRUE;
                       }
                    }
                }
            }
        }
        
        if (DefaultExpr)
        {
            *Element = CCExpressionRetain(DefaultExpr);
            return TRUE;
        }
    }
    
    return FALSE;
}

CCExpression CCListExpressionGetter(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if ((ArgCount == 2) || (ArgCount == 3))
    {
        CCExpression IndexExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression ListExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression DefaultExpr = ArgCount == 3 ? CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3)) : NULL;
        if (CCExpressionGetType(ListExpr) == CCExpressionValueTypeList)
        {
            CCCollection(CCExpression) List = CCExpressionGetList(ListExpr);
            CCExpression Element;
            if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeList)
            {
                CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(IndexExpr))
                {
                    if (!CCListExpressionGetElement(List, Expr, &Element, DefaultExpr))
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
            
            else if (CCListExpressionGetElement(List, IndexExpr, &Element, DefaultExpr)) return Element;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("get", "index:integer|list list:list [default:expr]");
    
    return Expression;
}

CCExpression CCListExpressionSetter(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 3)
    {
        //TODO: Index could be a list of indices, which would set a list of elements
        CCExpression IndexExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression List = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
        {
            size_t Count = CCCollectionGetCount(CCExpressionGetList(List));
            int32_t Index = CCExpressionGetInteger(IndexExpr);
            
            if ((Index < Count) || ((Index = (int32_t)(Index + Count)) < Count))
            {
                List = CCExpressionCopy(List);
                CCOrderedCollectionReplaceElementAtIndex(CCExpressionGetList(List), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3))) }, Index);
                return List;
            }
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("set", "index:integer list:list value:expr");
    
    return Expression;
}

static int CCListExpressionIndexCompareAscending(const int32_t *Left, const int32_t *Right)
{
    if (*Left < *Right) return -1;
    else if (*Left > *Right) return 1;
    
    return 0;
}

static CCExpression CCListExpressionDropCopy(CCCollection(CCExpression) List, const int32_t *Indexes, size_t IndexCount)
{
    size_t Index = 0, SkipIndex = 0;
    CCExpression Result = CCExpressionCreateList(CC_STD_ALLOCATOR);
    CC_COLLECTION_FOREACH(CCExpression, Expr, List)
    {
        for ( ; (SkipIndex < IndexCount) && (Index > Indexes[SkipIndex]); SkipIndex++);
        
        if ((SkipIndex == IndexCount) || (Index != Indexes[SkipIndex])) CCOrderedCollectionAppendElement(CCExpressionGetList(Result), &(CCExpression){ CCExpressionRetain(Expr) });
        
        Index++;
    }
    
    return Result;
}

CCExpression CCListExpressionDrop(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression IndexExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression ListExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if (CCExpressionGetType(ListExpr) == CCExpressionValueTypeList)
        {
            CCCollection(CCExpression) List = CCExpressionGetList(ListExpr);
            size_t Count = CCCollectionGetCount(List);
            
            if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeList)
            {
                const size_t IndexCount = CCCollectionGetCount(CCExpressionGetList(IndexExpr));
                if (IndexCount == 0) return CCExpressionRetain(ListExpr);
                
                int32_t *Indexes;
                CC_SAFE_Malloc(Indexes, sizeof(int32_t) * IndexCount,
                               CC_EXPRESSION_EVALUATOR_LOG_ERROR("Failed to allocate index list. Due to insufficient memory (%zu).", sizeof(int32_t) * IndexCount);
                               return Expression;
                               );
                
                int32_t *CurIndex = Indexes;
                CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(IndexExpr))
                {
                    if (CCExpressionGetType(Expr) == CCExpressionValueTypeInteger)
                    {
                        const int32_t Index = CCExpressionGetInteger(Expr);
                        if (Index < Count) *CurIndex++ = Index;
                        else if ((Index + Count) < Count) *CurIndex++ = (int32_t)(Index + Count);
                        else
                        {
                            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of drop: indexes must be within bounds of list");
                            
                            CC_SAFE_Free(Indexes);
                            
                            return Expression;
                        }
                    }
                }
                
                qsort(Indexes, IndexCount, sizeof(int32_t), (int(*)(const void*, const void*))CCListExpressionIndexCompareAscending);
                
                CCExpression Result = CCListExpressionDropCopy(List, Indexes, IndexCount);
                
                CC_SAFE_Free(Indexes);
                
                return Result;
            }
            
            else if (CCExpressionGetType(IndexExpr) == CCExpressionValueTypeInteger)
            {
                const int32_t Index = CCExpressionGetInteger(IndexExpr);
                if (Index < Count) return CCListExpressionDropCopy(List, &Index, 1);
                else if ((Index + Count) < Count) return CCListExpressionDropCopy(List, &(int32_t){ (int32_t)(Index + Count) }, 1);
                else
                {
                    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of drop: indexes must be within bounds of list");
                    
                    return Expression;
                }
            }
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("drop", "index:integer|list list:list");
    
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
