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
    return (CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionCreateAtom : CCExpressionCreateString)(Value->allocator, CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionGetAtom(Value) : CCExpressionGetString(Value), strlen(CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionGetAtom(Value) : CCExpressionGetString(Value)));
}

static CCExpression CCExpressionValueListCopy(CCExpression Value)
{
    CCExpression Copy = CCExpressionCreateList(Value->allocator);
    
    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Value))
    {
        CCOrderedCollectionAppendElement(CCExpressionGetList(Copy), &(CCExpression){ CCExpressionCopy(Element) });
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
    
    Expression->state = (CCExpressionState){ .values = NULL, .super = NULL, .result = NULL, .remove = NULL };
    Expression->allocator = Allocator;
    
    return Expression;
}

CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, const char *Atom, size_t Length)
{
    char *String;
    CC_SAFE_Malloc(String, sizeof(char) * (Length + 1),
                   CC_LOG_ERROR("Failed to create expression due to failure allocating string. Allocation size: %zu", sizeof(char) * (Length + 1));
                   return NULL;
                   );
    
    strncpy(String, Atom, Length);
    String[Length] = 0;
    
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeAtom);
    Expression->atom = String;
    
    return Expression;
}

CCExpression CCExpressionCreateInteger(CCAllocatorType Allocator, int32_t Value)
{
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeInteger);
    Expression->integer = Value;
    
    return Expression;
}

CCExpression CCExpressionCreateFloat(CCAllocatorType Allocator, float Value)
{
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeFloat);
    Expression->real = Value;
    
    return Expression;
}

CCExpression CCExpressionCreateString(CCAllocatorType Allocator, const char *Input, size_t Length)
{
    char *String;
    CC_SAFE_Malloc(String, sizeof(char) * (Length + 1),
                   CC_LOG_ERROR("Failed to create expression due to failure allocating string. Allocation size: %zu", sizeof(char) * (Length + 1));
                   return NULL;
                   );
    
    strncpy(String, Input, Length);
    String[Length] = 0;
    
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeString);
    Expression->string = String;
    
    return Expression;
}

CCExpression CCExpressionCreateList(CCAllocatorType Allocator)
{
    return CCExpressionCreate(Allocator, CCExpressionValueTypeList);
}

CCExpression CCExpressionCreateCustomType(CCAllocatorType Allocator, CCExpressionValueType Type, void *Data, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor)
{
    CCExpression Expression = CCExpressionCreate(Allocator, Type);
    Expression->data = Data;
    Expression->copy = Copy;
    Expression->destructor = Destructor;
    
    return Expression;
}

void CCExpressionDestroy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (Expression->destructor) Expression->destructor(CCExpressionGetData(Expression));
    if (Expression->state.values) CCCollectionDestroy(Expression->state.values);
    if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
    CC_SAFE_Free(Expression);
}

void CCExpressionChangeOwnership(CCExpression Expression, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    Expression->copy = Copy;
    Expression->destructor = Destructor;
}

CCExpression CCExpressionCopy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpression Copy = NULL;
    if (Expression->copy) Copy = Expression->copy(Expression);
    else
    {
        Copy = CCExpressionCreate(Expression->allocator, CCExpressionGetType(Expression));
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
    
    if ((Type == CCExpressionValueTypeInteger) && (memchr(Input, '.', Length))) Type = CCExpressionValueTypeFloat;
    
    switch (Type)
    {
        case CCExpressionValueTypeAtom:
            return CCExpressionCreateAtom(Allocator, Input, Length);
            
        case CCExpressionValueTypeInteger:
            return CCExpressionCreateInteger(Allocator, (int32_t)strtol(Input, NULL, 10));
            
        case CCExpressionValueTypeFloat:
            return CCExpressionCreateFloat(Allocator, (float)strtod(Input, NULL));
            
        case CCExpressionValueTypeString:
            return CCExpressionCreateString(Allocator, Input, Length);
            
        default:
            break;
    }
    
    return NULL;
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
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Val);
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
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Val);
                
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
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Val);
                
                Value = NULL;
            }
            
            else Value = *Source;
            
            IsStr = !IsStr;
        }
        
        else if (c == ';')
        {
            IsComment = TRUE;
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
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &Val);
                
                Value = NULL;
            }
        }
        
        else if (c == '\n')
        {
            IsComment = FALSE;
        }
    }
    
    return Expr;
}

static void CCExpressionPrintStatement(CCExpression Expression)
{
    switch (CCExpressionGetType(Expression))
    {
        case CCExpressionValueTypeAtom:
            printf("%s", CCExpressionGetAtom(Expression));
            break;
            
        case CCExpressionValueTypeInteger:
            printf("%" PRId32, CCExpressionGetInteger(Expression));
            break;
            
        case CCExpressionValueTypeFloat:
            printf("%f", CCExpressionGetFloat(Expression));
            break;
            
        case CCExpressionValueTypeString:
            printf("\"%s\"", CCExpressionGetString(Expression));
            break;
            
        case CCExpressionValueTypeList:
        {
            printf("( ");
            CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
            {
                CCExpressionPrintStatement(Expr);
                printf(" ");
            }
            printf(")");
            break;
        }
            
        default:
            break;
    }
}

void CCExpressionPrint(CCExpression Expression)
{
    CCExpressionPrintStatement(Expression); printf("\n");
}

CCExpression CCExpressionEvaluate(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
    
    Expression->state.result = Expression;
    if (CCExpressionGetType(Expression) == CCExpressionValueTypeExpression)
    {
        _Bool IsList = TRUE;
        CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
        {
            Expr->state.super = Expression;
        }
        
        CCExpression *Expr = CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
        if (Expr)
        {
            CCExpression Func = CCExpressionGetType(*Expr) == CCExpressionValueTypeExpression ? CCExpressionEvaluate(*Expr) : *Expr;
            
            if (CCExpressionGetType(Func) == CCExpressionValueTypeAtom)
            {
                CCExpressionEvaluator Eval = CCExpressionEvaluatorForName(CCExpressionGetAtom(Func));
                if (Eval)
                {
                    Expression->state.result = Eval(Expression);
                    IsList = FALSE;
                }
                
                else //set state
                {
                    CCExpression State = NULL;
                    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
                    {
                        State = CCExpressionSetState(Expression, CCExpressionGetAtom(Func), CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1)));
                    }
                    
                    else
                    {
                        State = CCExpressionSetState(Expression, CCExpressionGetAtom(Func), Expression);
                        if (State)
                        {
                            CCOrderedCollectionRemoveElementAtIndex(CCExpressionGetList(State), 0);
                        }
                    }
                    
                    if (State)
                    {
                        Expression->state.result = CCExpressionCopy(State);
                        IsList = FALSE;
                    }
                }
            }
            
            if (!Expression->state.result) //mark expression for removal
            {
                CCExpression Super = Expression->state.super;
                if (!Super->state.remove) Super->state.remove = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall, sizeof(CCCollectionEntry), NULL);
                
                CCCollectionInsertElement(Super->state.remove, &(CCCollectionEntry){ CCCollectionFindElement(CCExpressionGetList(Super), &Expression, NULL) });
            }
        }
        
        if (Expression->state.remove) //remove expression
        {
            CC_COLLECTION_FOREACH(CCCollectionEntry, Entry, Expression->state.remove)
            {
                CCCollectionRemoveElement(CCExpressionGetList(Expression), Entry);
            }

            CCCollectionDestroy(Expression->state.remove);
            Expression->state.remove = NULL;
        }
        
        if (IsList) //evaluate list
        {
            Expression->state.result = CCExpressionCreateList(Expression->allocator);
            
            CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expression->state.result), &(CCExpression){ CCExpressionCopy(CCExpressionEvaluate(Expr)) });
            }
        }
    }
    
    else if (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom) //get state
    {
        CCExpression State = CCExpressionGetState(Expression->state.super, CCExpressionGetAtom(Expression));
        if (State) Expression->state.result = CCExpressionCopy(State);
    }
    
    return Expression->state.result;
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
    
    Destination->state.remove = NULL;
    Destination->state.result = NULL;
}
