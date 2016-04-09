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

#include "ColourExpressions.h"
#include "ExpressionHelpers.h"
#include "Colour.h"

CCExpression CCColourExpressionLighten(CCExpression Expression)
{
    CCExpression Expr = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
    {
        CCExpression Colour = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Change = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        
        if ((CCExpressionGetType(Colour) == CCExpressionValueTypeList) && ((CCExpressionGetType(Change) == CCExpressionValueTypeInteger) || (CCExpressionGetType(Change) == CCExpressionValueTypeFloat)))
        {
            CCColour ColourHSV = CCColourConversion(CCColourCreateFromRGBA(CCExpressionGetColour(Colour)), CCColourFormatHSVA32Float);
            if (CCExpressionGetType(Change) == CCExpressionValueTypeInteger) ColourHSV.channel[2].f32 += ColourHSV.channel[2].f32 * ((float)CCExpressionGetInteger(Change) / 100.0f);
            else ColourHSV.channel[2].f32 += CCExpressionGetFloat(Change);
            
            ColourHSV.channel[2].f32 = fmaxf(ColourHSV.channel[2].f32, 1.0f);
            
            CCColour ColourRGB = CCColourConversion(ColourHSV, CCColourFormatRGBA32Float);
            if (CCCollectionGetCount(CCExpressionGetList(Colour)) == 4) Expr = CCExpressionCreateVector4(CC_STD_ALLOCATOR, CCVector4DMake(ColourRGB.channel[0].f32, ColourRGB.channel[1].f32, ColourRGB.channel[2].f32, ColourRGB.channel[3].f32));
            else Expr = CCExpressionCreateVector3(CC_STD_ALLOCATOR, CCVector3DMake(ColourRGB.channel[0].f32, ColourRGB.channel[1].f32, ColourRGB.channel[2].f32));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("lighten", "colour:list change:number");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("lighten", "colour:list change:number");
    
    return Expr;
}

CCExpression CCColourExpressionDarken(CCExpression Expression)
{
    CCExpression Expr = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 3)
    {
        CCExpression Colour = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Change = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        
        if ((CCExpressionGetType(Colour) == CCExpressionValueTypeList) && ((CCExpressionGetType(Change) == CCExpressionValueTypeInteger) || (CCExpressionGetType(Change) == CCExpressionValueTypeFloat)))
        {
            CCColour ColourHSV = CCColourConversion(CCColourCreateFromRGBA(CCExpressionGetColour(Colour)), CCColourFormatHSVA32Float);
            if (CCExpressionGetType(Change) == CCExpressionValueTypeInteger) ColourHSV.channel[2].f32 -= ColourHSV.channel[2].f32 * ((float)CCExpressionGetInteger(Change) / 100.0f);
            else ColourHSV.channel[2].f32 -= CCExpressionGetFloat(Change);
            
            ColourHSV.channel[2].f32 = fmaxf(ColourHSV.channel[2].f32, 0.0f);
            
            CCColour ColourRGB = CCColourConversion(ColourHSV, CCColourFormatRGBA32Float);
            if (CCCollectionGetCount(CCExpressionGetList(Colour)) == 4) Expr = CCExpressionCreateVector4(CC_STD_ALLOCATOR, CCVector4DMake(ColourRGB.channel[0].f32, ColourRGB.channel[1].f32, ColourRGB.channel[2].f32, ColourRGB.channel[3].f32));
            else Expr = CCExpressionCreateVector3(CC_STD_ALLOCATOR, CCVector3DMake(ColourRGB.channel[0].f32, ColourRGB.channel[1].f32, ColourRGB.channel[2].f32));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("darken", "colour:list change:number");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("darken", "colour:list change:number");
    
    return Expr;
}
