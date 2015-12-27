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

static CCExpression CCExpressionValueAtomOrStringCopy(CCExpression Value)
{
    char *String;
    const size_t Length = strlen(Value->type == CCExpressionValueTypeAtom ? Value->atom : Value->string);
    CC_SAFE_Malloc(String, sizeof(char) * (Length + 1),
                   CC_LOG_ERROR("Failed to copy expression due to failure allocating string. Allocation size: %zu", sizeof(char) * (Length + 1));
                   return NULL;
                   );
    
    strcpy(String, Value->type == CCExpressionValueTypeAtom ? Value->atom : Value->string);
    
    CCExpression Copy = CCExpressionCreate(CC_STD_ALLOCATOR, Value->type);
    Copy->destructor = Value->destructor;
    Copy->copy = Value->copy;
    *(Value->type == CCExpressionValueTypeAtom ? &Copy->atom : &Copy->string) = String;
    
    return Copy;
}

static CCExpression CCExpressionValueListCopy(CCExpression Value)
{
    CCExpression Copy = CCExpressionCreate(CC_STD_ALLOCATOR, Value->type);
    Copy->destructor = Value->destructor;
    Copy->copy = Value->copy;
    Copy->list = CCCollectionCreate(Value->list->allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), Value->list->destructor);
    
    CC_COLLECTION_FOREACH(CCExpression, Element, Value->list)
    {
        CCOrderedCollectionAppendElement(Copy->list, &(CCExpression){ CCExpressionCopy(Element) });
    }
    
    return Copy;
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
            Expression->copy = CCExpressionValueAtomOrStringCopy;
            break;
            
        case CCExpressionValueTypeList:
            Expression->list = CCCollectionCreate(Allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), (CCCollectionElementDestructor)CCExpressionElementDestructor);
            Expression->destructor = (CCExpressionValueDestructor)CCCollectionDestroy;
            Expression->copy = CCExpressionValueListCopy;
            break;
            
        default:
            Expression->destructor = NULL;
            Expression->copy = NULL;
            break;
    }
    
    Expression->state = (CCExpressionState){ .values = NULL, .super = NULL };
    Expression->allocator = Allocator;
    
    return Expression;
}

void CCExpressionDestroy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (Expression->destructor) Expression->destructor(Expression->data);
    if (Expression->state.values) CCCollectionDestroy(Expression->state.values);
    CC_SAFE_Free(Expression);
}

CCExpression CCExpressionCopy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpression Copy = NULL;
    if (Expression->copy) Copy = Expression->copy(Expression);
    else
    {
        Copy = CCExpressionCreate(Expression->allocator, Expression->type);
        memcpy(Copy, Expression, sizeof(CCExpressionValue));
    }
    
    if (Copy)
    {
        CCExpressionCopyState(Expression, Copy);
    }
    
    return Copy;
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
            
        default:
            break;
    }
}

typedef struct {
    CCCollectionEntry entry;
    CCExpression temp;
} CCExpressionTemp;

CCExpression CCExpressionEvaluate(CCExpression Expression) //TODO: Either get one of the implementations working or rewrite this because it's very inefficient.
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpression Result = Expression;
    if (Expression->type == CCExpressionValueTypeExpression)
    {
        CCCollection Remove = NULL, Temp = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall, sizeof(CCExpressionTemp), NULL);
        CC_COLLECTION_FOREACH(CCExpression, Expr, Expression->list)
        {
            if (Expr->type == CCExpressionValueTypeExpression)
            {
                Expr->state.super = Expression;
                
                CCExpression Res = CCExpressionEvaluate(Expr);
                if (!Res)
                {
                    if (!Remove) Remove = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall, sizeof(CCCollectionEntry), NULL);
                    
                    CCCollectionInsertElement(Remove, &(CCCollectionEntry){ CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR) });
                }
                
                else if (Res != Expr)
                {
                    CCCollectionEntry Entry = CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR);
                    CCExpression *Arg = CCCollectionGetElement(Expression->list, Entry);
                    
                    CCCollectionInsertElement(Temp, &(CCExpressionTemp){
                        .entry = Entry,
                        .temp = *Arg
                    });
                    
                    *Arg = Res;
                }
                
                Expr->state.super = NULL;
            }
            
            else if ((Expr->type == CCExpressionValueTypeAtom) && (CCOrderedCollectionGetIndex(Expression->list, CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR))))
            {
                CCExpression State = CCExpressionGetState(Expression, Expr->atom);
                if (State)
                {
                    CCCollectionEntry Entry = CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR);
                    CCExpression *Arg = CCCollectionGetElement(Expression->list, Entry);
                    
                    CCCollectionInsertElement(Temp, &(CCExpressionTemp){
                        .entry = Entry,
                        .temp = *Arg
                    });
                    
                    *Arg = CCExpressionCopy(State);
                }
            }
        }
        
        if (Remove)
        {
            CC_COLLECTION_FOREACH(CCCollectionEntry, Entry, Remove)
            {
                CCCollectionRemoveElement(Expression->list, Entry);
            }
            
            CCCollectionDestroy(Remove);
        }
        
        CCExpression *Expr = CCOrderedCollectionGetElementAtIndex(Expression->list, 0);
        if ((Expr) && ((*Expr)->type == CCExpressionValueTypeAtom))
        {
            CCExpressionEvaluator Eval = CCExpressionEvaluatorForName((*Expr)->atom);
            if (Eval)
            {
                Result = Eval(Expression);
            }
            
            else //set state
            {
                CCExpression State = NULL;
                if (CCCollectionGetCount(Expression->list) == 2)
                {
                    State = CCExpressionSetState(Expression, (*Expr)->atom, *(CCExpression*)CCOrderedCollectionGetElementAtIndex(Expression->list, 1));
                }
                
                else
                {
                    State = CCExpressionSetState(Expression, (*Expr)->atom, Expression);
                    if (State)
                    {
                        CCOrderedCollectionRemoveElementAtIndex(State->list, 0);
                    }
                }
                
                if (State) Result = CCExpressionCopy(State);
            }
        }
        
        
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(Temp, &Enumerator);
        
        for (CCExpressionTemp *Saved = CCCollectionEnumeratorGetCurrent(&Enumerator); Saved; Saved = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCOrderedCollectionReplaceElementAtIndex(Expression->list, &Saved->temp, CCOrderedCollectionGetIndex(Expression->list, Saved->entry));
        }
        
        CCCollectionDestroy(Temp);
    }
    
    return Result;
}

typedef struct {
    char *name;
    CCExpression value;
} CCExpressionStateValue;

static CCComparisonResult CCExpressionStateValueElementFind(const CCExpressionStateValue *left, const CCExpressionStateValue *right)
{
    return !strcmp(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCExpressionStateValue *CCExpressionGetStateValue(CCExpression Expression, const char *Name)
{
    if (!Expression) return NULL;
    
    CCExpressionStateValue *Value = NULL;
    if (Expression->state.values) Value = CCCollectionGetElement(Expression->state.values, CCCollectionFindElement(Expression->state.values, &(CCExpressionStateValue){ .name = (char*)Name }, (CCComparator)CCExpressionStateValueElementFind));
    
    return Value ? Value : CCExpressionGetStateValue(Expression->state.super, Name);
}

static void CCExpressionStateValueElementDestructor(CCCollection Collection, CCExpressionStateValue *Element)
{
    CC_SAFE_Free(Element->name);
    CCExpressionDestroy(Element->value);
}

void CCExpressionCreateState(CCExpression Expression, const char *Name, CCExpression Value)
{
    if ((Expression->state.values) && (CCCollectionFindElement(Expression->state.values, &(CCExpressionStateValue){ .name = (char*)Name }, (CCComparator)CCExpressionStateValueElementFind)))
    {
        CC_LOG_ERROR("Creating duplicate state (%s)", Name);
    }
    
    else
    {
        if (!Expression->state.values) Expression->state.values = CCCollectionCreate(Expression->allocator, CCCollectionHintHeavyFinding, sizeof(CCExpressionStateValue), (CCCollectionElementDestructor)CCExpressionStateValueElementDestructor);
        
        char *ValueName;
        CC_SAFE_Malloc(ValueName, sizeof(char) * (strlen(Name) + 1),
                       CC_LOG_ERROR("Failed to add expression state (%s) due to allocation space for name failing. Allocation size: %zu", Name, sizeof(char) * (strlen(Name) + 1));
                       return;
                       );
        
        strcpy(ValueName, Name);
        
        CCCollectionInsertElement(Expression->state.values, &(CCExpressionStateValue){
            .name = ValueName,
            .value = Value ? CCExpressionCopy(Value) : NULL
        });
    }
}

CCExpression CCExpressionGetState(CCExpression Expression, const char *Name)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    
    return State ? State->value : NULL;
}

CCExpression CCExpressionSetState(CCExpression Expression, const char *Name, CCExpression Value)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    if (State)
    {
        if (State->value) CCExpressionDestroy(State->value);
        State->value = CCExpressionCopy(Value);
        
        return State->value;
    }
    
    return NULL;
}

void CCExpressionCopyState(CCExpression Source, CCExpression Destination)
{
    CCAssertLog(Source && Destination, "Source and destination expressions must not be NULL");
    
    if (Source->state.values)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(Source->state.values, &Enumerator);
        
        for (CCExpressionStateValue *State = CCCollectionEnumeratorGetCurrent(&Enumerator); State; State = CCCollectionEnumeratorNext(&Enumerator))
        {
            CCExpressionCreateState(Destination, State->name, State->value);
        }
    }
    
    else if (Destination->state.values)
    {
        CCCollectionDestroy(Destination->state.values);
        Destination->state.values = NULL;
    }
}
