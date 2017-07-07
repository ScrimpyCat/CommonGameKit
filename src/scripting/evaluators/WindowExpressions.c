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

#include "WindowExpressions.h"
#include "Window.h"
#include "ExpressionHelpers.h"

static CCExpression CCWindowExpressionPercentage(CCExpression Expression, const char *Name, _Bool UseHeight)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if (ArgCount == 1)
    {
        const CCVector2Di Size = CCWindowGetFrameSize();
        
        CCExpression Percent = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Percent) == CCExpressionValueTypeInteger)
        {
            Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)((float)Size.v[UseHeight] * ((float)CCExpressionGetInteger(Percent) / 100)));
        }
        
        else if (CCExpressionGetType(Percent) == CCExpressionValueTypeFloat)
        {
            Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)Size.v[UseHeight] * CCExpressionGetFloat(Percent));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Name, "percent:number");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Name, "percent:number");
    
    return Expr;
}

CCExpression CCWindowExpressionPercentageWidth(CCExpression Expression)
{
    return CCWindowExpressionPercentage(Expression, "window-percent-width", FALSE);
}

CCExpression CCWindowExpressionPercentageHeight(CCExpression Expression)
{
    return CCWindowExpressionPercentage(Expression, "window-percent-height", TRUE);
}

static inline CCExpression CCWindowExpressionSize(_Bool UseHeight)
{
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCWindowGetFrameSize().v[UseHeight]);
}

CCExpression CCWindowExpressionWidth(CCExpression Expression)
{
    return CCWindowExpressionSize(FALSE);
}

CCExpression CCWindowExpressionHeight(CCExpression Expression)
{
    return CCWindowExpressionSize(TRUE);
}

CCString StringWindowSize = CC_STRING("@window-size");
CCExpression CCWindowExpressionWindowResized(CCExpression Expression)
{
    _Bool Changed = TRUE;
    const CCVector2Di CurrentSize = CCWindowGetFrameSize();
    
    CCExpression Size = CCExpressionGetStateStrict(Expression, StringWindowSize);
    if (Size)
    {
        const CCVector2Di OldSize = CCExpressionGetVector2i(Size);
        if ((Changed = ((OldSize.x != CurrentSize.x) || (OldSize.y != CurrentSize.y))))
        {
            CCExpressionSetState(Expression, StringWindowSize, CCExpressionCreateVector2i(CC_STD_ALLOCATOR, CurrentSize), FALSE);
        }
    }
    
    else CCExpressionCreateState(Expression, StringWindowSize, CCExpressionCreateVector2i(CC_STD_ALLOCATOR, CurrentSize), FALSE, NULL, FALSE);
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Changed);
}

CCString StringFrameID = CC_STRING("@frame-id");
CCExpression CCWindowExpressionFrameChanged(CCExpression Expression)
{
    _Bool Changed = TRUE;
    const uint32_t FrameID = CCWindowGetFrameID();
    
    CCExpression Frame = CCExpressionGetStateStrict(Expression, StringFrameID);
    if (Frame)
    {
        if ((Changed = (CCExpressionGetInteger(Frame) != FrameID)))
        {
            CCExpressionSetState(Expression, StringFrameID, CCExpressionCreateInteger(CC_STD_ALLOCATOR, FrameID), FALSE);
        }
    }
    
    else CCExpressionCreateState(Expression, StringFrameID, CCExpressionCreateInteger(CC_STD_ALLOCATOR, FrameID), FALSE, NULL, FALSE);
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Changed);
}
