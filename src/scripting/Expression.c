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

#include "Expression.h"
#include <inttypes.h>
#include "ExpressionEvaluator.h"


static CCExpressionValue *CCExpressionValueCreateFromString(CCAllocatorType Allocator, const char *Input, size_t Length);
static CCExpression CCExpressionParse(const char **Source);


static void CCExpressionElementDestructor(CCCollection Collection, CCExpression *Element)
{
    CCExpressionDestroy(*Element);
}

CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type)
{
    CCExpression Expression = CCMalloc(Allocator, sizeof(*Expression), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (!Expression)
    {
        CC_LOG_ERROR("Failed to create expression due to allocation failing. Allocation size: %zu", sizeof(*Expression));
        return NULL;
    }
    
    Expression->type = Type;
    switch (Type)
    {
        case CCExpressionValueTypeAtom:
        case CCExpressionValueTypeString:
            Expression->destructor = CCFree;
            break;
            
        case CCExpressionValueTypeList:
            Expression->list = CCCollectionCreate(Allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), (CCCollectionElementDestructor)CCExpressionElementDestructor);
            Expression->destructor = (CCExpressionValueDestructor)CCCollectionDestroy;
            break;
            
        default:
            Expression->destructor = NULL;
            break;
    }
    
    return Expression;
}

void CCExpressionDestroy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (Expression->destructor) Expression->destructor(Expression->data);
    CC_SAFE_Free(Expression);
}

CCExpression CCExpressionCreateFromSource(const char *Source)
{
    CCAssertLog(Source, "Source must not be NULL");
    
    return CCExpressionParse(&Source);
}

static CCExpressionValue *CCExpressionValueCreateFromString(CCAllocatorType Allocator, const char *Input, size_t Length)
{
    CCExpressionValueType Type = CCExpressionValueTypeAtom;
    if (*Input == '"')
    {
        Type = CCExpressionValueTypeString;
        Length--;
        Input++;
    }
    
    else if (isdigit(*Input)) Type = CCExpressionValueTypeInteger;
    else if ((*Input == '.') && (Length > 1) && (isdigit(Input[1]))) Type = CCExpressionValueTypeFloat;
    else if ((*Input == '-') && (Length > 1) && (isdigit(Input[1]))) Type = CCExpressionValueTypeInteger;
    
    char *String;
    CC_SAFE_Malloc(String, sizeof(char) * (Length + 1),
                   CC_LOG_ERROR("Failed to create expression due to failure allocating string. Allocation size: %zu", sizeof(char) * (Length + 1));
                   return NULL;
                   );
    
    strncpy(String, Input, Length);
    String[Length] = 0;
    
    if ((Type == CCExpressionValueTypeInteger) && (strchr(String, '.'))) Type = CCExpressionValueTypeFloat;
    
    CCExpression Val = CCExpressionCreate(CC_STD_ALLOCATOR, Type);
    
    switch (Type)
    {
        case CCExpressionValueTypeAtom:
            Val->atom = String;
            break;
            
        case CCExpressionValueTypeInteger:
            Val->integer = atoi(String);
            CC_SAFE_Free(String);
            break;
            
        case CCExpressionValueTypeFloat:
            Val->real = (float)atof(String);
            CC_SAFE_Free(String);
            break;
            
        case CCExpressionValueTypeString:
            Val->string = String;
            break;
            
        default:
            break;
    }
    
    return Val;
}

static CCExpression CCExpressionParse(const char **Source)
{
    CCExpression Expr = NULL;
    const char *Value = NULL;
    _Bool IsStr = FALSE, IsComment = FALSE;
    for (char c = 0; (c = **Source); (*Source)++)
    {
        if ((!IsComment) && (!IsStr) && (c == '('))
        {
            if (Expr)
            {
                CCExpression Val = CCExpressionParse(Source);
                CCOrderedCollectionAppendElement(Expr->list, &Val);
            }
            
            else Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeExpression);
        }
        
        else if ((Expr) && (!IsComment) && (!IsStr) && (c == ')'))
        {
            if (Value)
            {
                CCExpression Val = CCExpressionValueCreateFromString(CC_STD_ALLOCATOR, Value, *Source - Value);
                if (!Val)
                {
                    CCExpressionDestroy(Expr);
                    return NULL;
                }
                
                CCOrderedCollectionAppendElement(Expr->list, &Val);
                
                Value = NULL;
            }
            
            break;
        }
        
        else if ((Expr) && (!IsComment) && (c == '"'))
        {
            if (IsStr)
            {
                CCExpression Val = CCExpressionValueCreateFromString(CC_STD_ALLOCATOR, Value, *Source - Value);
                if (!Val)
                {
                    CCExpressionDestroy(Expr);
                    return NULL;
                }
                
                CCOrderedCollectionAppendElement(Expr->list, &Val);
                
                Value = NULL;
            }
            
            else Value = *Source;
            
            IsStr = !IsStr;
        }
        
        else if (c == ';')
        {
            IsComment = TRUE;
        }
        
        else if (c == '\n')
        {
            IsComment = FALSE;
        }
        
        else if ((Expr) && (!IsComment) && (!IsStr))
        {
            if (!isspace(c))
            {
                if (!Value) Value = *Source;
            }
            
            else if (Value)
            {
                CCExpression Val = CCExpressionValueCreateFromString(CC_STD_ALLOCATOR, Value, *Source - Value);
                if (!Val)
                {
                    CCExpressionDestroy(Expr);
                    return NULL;
                }
                
                CCOrderedCollectionAppendElement(Expr->list, &Val);
                
                Value = NULL;
            }
        }
    }
    
    return Expr;
}

void CCExpressionPrint(CCExpression Expression)
{
    switch (Expression->type)
    {
        case CCExpressionValueTypeAtom:
            printf("%s", Expression->atom);
            break;
            
        case CCExpressionValueTypeInteger:
            printf("%" PRId32, Expression->integer);
            break;
            
        case CCExpressionValueTypeFloat:
            printf("%f", Expression->real);
            break;
            
        case CCExpressionValueTypeString:
            printf("\"%s\"", Expression->string);
            break;
            
        case CCExpressionValueTypeList:
        {
            printf("( ");
            CC_COLLECTION_FOREACH(CCExpression, Expr, Expression->list)
            {
                CCExpressionPrint(Expr);
                printf(" ");
            }
            printf(")");
            break;
        }
    }
}

CCExpression CCExpressionEvaluate(CCExpression Expression)
{
    if (Expression->type == CCExpressionValueTypeExpression)
    {
        CC_COLLECTION_FOREACH(CCExpression, Expr, Expression->list)
        {
            if (Expr->type == CCExpressionValueTypeExpression)
            {
                CCExpression Res = CCExpressionEvaluate(Expr);
                if ((Res) && (Res != Expr)) CCOrderedCollectionReplaceElementAtIndex(Expression->list, &Res, CCOrderedCollectionGetIndex(Expression->list, CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR)));
            }
        }
        
        CCExpression *Expr = CCOrderedCollectionGetElementAtIndex(Expression->list, 0);
        if ((Expr) && ((*Expr)->type == CCExpressionValueTypeAtom))
        {
            CCExpressionEvaluator Eval = CCExpressionEvaluatorForName((*Expr)->atom);
            if (Eval)
            {
                return Eval(Expression);
            }
        }
    }
    
    return Expression;
}
