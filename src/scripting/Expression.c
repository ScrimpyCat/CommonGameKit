//
//  Expression.c
//  Blob Game
//
//  Created by Stefan Johnson on 21/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#include "Expression.h"
#include <inttypes.h>


static CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type);
static CCExpressionValue *CCExpressionValueCreateFromString(CCAllocatorType Allocator, const char *Input, size_t Length);
static CCExpression CCExpressionParse(const char **Source);


static void CCExpressionElementDestructor(CCCollection Collection, CCExpression *Element)
{
    CCExpressionDestroy(*Element);
}

static CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type)
{
    CCExpression Expression = CCMalloc(Allocator, sizeof(*Expression), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (!Expression)
    {
        CC_LOG_ERROR("Failed to create expression due to allocation failing. Allocation size: %zu", sizeof(*Expression));
        return NULL;
    }
    
    Expression->type = Type;
    if (Type == CCExpressionValueTypeList)
    {
        Expression->list = CCCollectionCreate(Allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), (CCCollectionElementDestructor)CCExpressionElementDestructor);
    }
    
    return Expression;
}

void CCExpressionDestroy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    switch (Expression->type)
    {
        case CCExpressionValueTypeAtom:
            CC_SAFE_Free(Expression->atom);
            break;
            
        case CCExpressionValueTypeString:
            CC_SAFE_Free(Expression->string);
            break;
            
        case CCExpressionValueTypeList:
            CCCollectionDestroy(Expression->list);
            break;
            
        default:
            break;
    }
    
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
    else if (*Input == '.') Type = CCExpressionValueTypeFloat;
    else if (*Input == '-') Type = CCExpressionValueTypeInteger;
    
    char *String;
    CC_SAFE_Malloc(String, sizeof(char) * Length,
                   CC_LOG_ERROR("Failed to create expression due to failure allocating string. Allocation size: %zu", sizeof(char) * Length);
                   return NULL;
                   );
    
    strncpy(String, Input, Length);
    
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
