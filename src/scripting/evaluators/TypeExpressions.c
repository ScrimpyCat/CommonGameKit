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
#include "TypeExpressions.h"

CCExpression CCTypeExpressionGetType(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        const CCExpressionValueType Type = CCExpressionGetType(Arg);
        
        switch (Type)
        {
            case CCExpressionValueTypeNull:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":null"), TRUE);
                
            case CCExpressionValueTypeAtom:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":atom"), TRUE);
                
            case CCExpressionValueTypeInteger:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":integer"), TRUE);
                
            case CCExpressionValueTypeFloat:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":float"), TRUE);
                
            case CCExpressionValueTypeString:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":string"), TRUE);
                
            case CCExpressionValueTypeList:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":list"), TRUE);
                
            case CCExpressionValueTypeUnspecified:
                return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":unknown"), TRUE);
                
            default:
                break;
        }
        
        char Label[5] = { 0, 0, 0, 0, 0 };
        
    #if CC_HARDWARE_ENDIAN_LITTLE
        Label[0] = (Type >> 24) & 0xff;
        Label[1] = (Type >> 16) & 0xff;
        Label[2] = (Type >> 8) & 0xff;
        Label[3] = Type & 0xff;
    #elif CC_HARDWARE_ENDIAN_BIG
        *(uint32_t*)Label = Type;
    #endif
        
        CCString LabelString = CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingASCII | CCStringHintCopy, Label);
        CCString CustomType = CCStringCreateByJoiningStrings((CCString[2]){
            CC_STRING(":custom:"),
            LabelString
        }, 2, 0);
        CCStringDestroy(LabelString);
        
        return CCExpressionCreateAtom(CC_STD_ALLOCATOR, CustomType, FALSE);
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("type", "value:expr");
    
    return Expression;
}

CCExpression CCTypeExpressionIntegerToFloat(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeInteger)
        {
            return CCExpressionCreateFloat(CC_STD_ALLOCATOR, CCExpressionGetInteger(Arg));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("integer->float", "value:integer");
    
    return Expression;
}

CCExpression CCTypeExpressionFloatToInteger(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeFloat)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCExpressionGetFloat(Arg));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("float->integer", "value:float");
    
    return Expression;
}
