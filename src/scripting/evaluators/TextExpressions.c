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
#include "TextExpressions.h"
#include "GraphicsExpressions.h"
#include "ExpressionHelpers.h"

CCExpression CCTextExpressionGetVisibleLength(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Text = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Text) == CCGraphicsExpressionValueTypeText)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)CCTextGetVisibleLength(CCExpressionGetData(Text)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("text-visible-length", "text:text");
    
    return Expression;
}

CCExpression CCTextExpressionGetCursorPosition(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
    {
        CCExpression Text = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Offset = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Text) == CCGraphicsExpressionValueTypeText) && (CCExpressionGetType(Offset) == CCExpressionValueTypeInteger))
        {
            return CCExpressionCreateVector2(CC_STD_ALLOCATOR, CCTextGetCursorPosition(CCExpressionGetData(Text), CCExpressionGetInteger(Offset)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("text-cursor-position", "text:text offset:integer");
    
    return Expression;
}

CCExpression CCTextExpressionGetCursorOffset(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
    {
        CCExpression Text = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Position = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Text) == CCGraphicsExpressionValueTypeText) && (CCExpressionGetType(Position) == CCExpressionValueTypeList))
        {
            size_t Offset = CCTextGetCursorOffset(CCExpressionGetData(Text), CCExpressionGetVector2(Position));
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)(Offset == SIZE_MAX ? -1 : Offset));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("text-cursor-offset", "text:text position:list");
    
    return Expression;
}
