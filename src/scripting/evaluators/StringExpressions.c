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

#include "StringExpressions.h"

CCExpression CCStringExpressionPrefix(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Prefix = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Prefix) == CCExpressionValueTypeString) && (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCStringHasPrefix(CCExpressionGetString(String), CCExpressionGetString(Prefix)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("prefix", "prefix:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionSuffix(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 2)
    {
        CCExpression Suffix = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        if ((CCExpressionGetType(Suffix) == CCExpressionValueTypeString) && (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCStringHasSuffix(CCExpressionGetString(String), CCExpressionGetString(Suffix)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("suffix", "suffix:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionFilename(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 1)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeString)
        {
            CCExpression Result = Expression;
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Arg))
            {
                FSPath Path = FSPathCreate(Buffer);
                
                const char *Filename = FSPathGetFilenameString(Path);
                if (Filename) Result = CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Filename), FALSE);
                else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Path does not contain a filename: %s", Buffer);
                
                FSPathDestroy(Path);
            }
            
            return Result;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("filename", "path:string");
    
    return Expression;
}

CCExpression CCStringExpressionReplace(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 3)
    {
        CCExpression Occurrence = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Replacement = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        
        if ((CCExpressionGetType(Occurrence) == CCExpressionValueTypeString) &&
            (CCExpressionGetType(Replacement) == CCExpressionValueTypeString) &&
            (CCExpressionGetType(String) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByReplacingOccurrencesOfString(CCExpressionGetString(String), CCExpressionGetString(Occurrence), CCExpressionGetString(Replacement)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("replace", "occurrence:string replacement:string string:string");
    
    return Expression;
}

CCExpression CCStringExpressionConcatenate(CCExpression Expression)
{
    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if ((ArgCount == 1) || (ArgCount == 2))
    {
        CCExpression StringsExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(StringsExpr) == CCExpressionValueTypeList)
        {
            CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCString), NULL);
            CC_COLLECTION_FOREACH(CCExpression, String, CCExpressionGetList(StringsExpr))
            {
                if (CCExpressionGetType(String) == CCExpressionValueTypeString)
                {
                    CCOrderedCollectionAppendElement(Strings, &(CCString){ CCExpressionGetString(String) });
                }
                
                else
                {
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
                    CCCollectionDestroy(Strings);
                    
                    return Expression;
                }
            }
            
            CCString Separator = 0;
            if (ArgCount == 2)
            {
                CCExpression SeparatorExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
                if (CCExpressionGetType(SeparatorExpr) != CCExpressionValueTypeString)
                {
                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
                    CCCollectionDestroy(Strings);
                    
                    return Expression;
                }
                
                Separator = CCExpressionGetString(SeparatorExpr);
            }
            
            Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByJoiningEntries(Strings, Separator), FALSE);
            CCCollectionDestroy(Strings);
            
            return Expression;
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("cat", "strings:list [separator:string]");
    
    return Expression;
}

CCExpression CCStringExpressionLength(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression String = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(String) == CCExpressionValueTypeString)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)CCStringGetLength(CCExpressionGetString(String)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("length", "string:string");
    
    return Expression;
}

CCExpression CCStringExpressionInsert(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
    {
        CCExpression SrcExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression DstExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression OffsetExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        if ((CCExpressionGetType(SrcExpr) == CCExpressionValueTypeString) && (CCExpressionGetType(DstExpr) == CCExpressionValueTypeString) && (CCExpressionGetType(OffsetExpr) == CCExpressionValueTypeInteger))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateByInsertingString(CCExpressionGetString(DstExpr), CCExpressionGetInteger(OffsetExpr), CCExpressionGetString(SrcExpr)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("insert", "source:string destination:string offset:integer");
    
    return Expression;
}

CCExpression CCStringExpressionRemove(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 4)
    {
        CCExpression OffsetExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression LengthExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression StringExpr = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        if ((CCExpressionGetType(OffsetExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetType(LengthExpr) == CCExpressionValueTypeInteger) && (CCExpressionGetType(StringExpr) == CCExpressionValueTypeString))
        {
            return CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateWithoutRange(CCExpressionGetString(StringExpr), CCExpressionGetInteger(OffsetExpr), CCExpressionGetInteger(LengthExpr)), FALSE);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("remove", "offset:integer length:integer offset:integer");
    
    return Expression;
}
