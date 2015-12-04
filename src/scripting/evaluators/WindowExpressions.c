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

static CCExpression CCWindowExpressionPercentage(CCExpression Expression, const char *Name, _Bool UseHeight)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(Expression->list) - 1;
    
    if (ArgCount == 1)
    {
        int Size[2];
        glfwGetFramebufferSize(CCWindow, Size, Size + 1);
        
        CCExpression Percent = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(Expression->list, 1);
        if (Percent->type == CCExpressionValueTypeInteger)
        {
            Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
            Expr->integer = (int32_t)((float)Size[UseHeight] * ((float)Percent->integer / 100));
        }
        
        else if (Percent->type == CCExpressionValueTypeFloat)
        {
            Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeFloat);
            Expr->real = (float)Size[UseHeight] * Percent->real;
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

static CCExpression CCWindowExpressionSize(_Bool UseHeight)
{
    int Size[2];
    glfwGetFramebufferSize(CCWindow, Size, Size + 1);
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Size[UseHeight];
    
    return Expr;
}

CCExpression CCWindowExpressionWidth(CCExpression Expression)
{
    return CCWindowExpressionSize(FALSE);
}

CCExpression CCWindowExpressionHeight(CCExpression Expression)
{
    return CCWindowExpressionSize(TRUE);
}
