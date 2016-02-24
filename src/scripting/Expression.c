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
    return (CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionCreateAtom : CCExpressionCreateString)(Value->allocator, CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionGetAtom(Value) : CCExpressionGetString(Value));
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
    CCExpression Expression = CCMalloc(CC_ALIGNED_ALLOCATOR(8)/*Allocator*/, sizeof(*Expression), NULL, CC_DEFAULT_ERROR_CALLBACK);
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
            Expression->destructor = (CCExpressionValueDestructor)CCStringDestroy;
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

CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, CCString Atom)
{
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeAtom);
    Expression->atom = CCStringCopy(Atom);
    
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

CCExpression CCExpressionCreateString(CCAllocatorType Allocator, CCString Input)
{
    CCString String = CCStringCopy(Input);
    if (String & 3) return (CCExpression)String;
    
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
    
    if (CCExpressionIsTagged(Expression)) return;
    
    if (Expression->destructor) Expression->destructor(CCExpressionGetData(Expression));
    if (Expression->state.values) CCCollectionDestroy(Expression->state.values);
    if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
    CC_SAFE_Free(Expression);
}

CCExpressionValueType CCExpressionGetTaggedType(uintptr_t Expression)
{
    if ((Expression & CCExpressionTaggedMask) == CCExpressionTaggedExtended)
    {
        switch (Expression & CCExpressionTaggedExtendedMask)
        {
            case CCExpressionTaggedExtendedAtom:
                return CCExpressionValueTypeAtom;
                
            case CCExpressionTaggedExtendedInteger:
                return CCExpressionValueTypeInteger;
                
            case CCExpressionTaggedExtendedFloat:
                return CCExpressionValueTypeFloat;
                
            default:
                return CCExpressionValueTypeUnspecified;
        }
    }
    
    else return CCExpressionValueTypeString;
}


void CCExpressionChangeOwnership(CCExpression Expression, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (CCExpressionIsTagged(Expression)) return;
    
    Expression->copy = Copy;
    Expression->destructor = Destructor;
}

CCExpression CCExpressionCopy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (CCExpressionIsTagged(Expression)) return Expression;
    
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

CCExpression CCExpressionCreateFromSourceFile(FSPath Path)
{
    CCExpression Expression = NULL;
    
    FSHandle Handle;
    if (FSHandleOpen(Path, FSHandleTypeRead, &Handle) == FSOperationSuccess)
    {
        size_t Size = FSManagerGetSize(Path);
        char *Source;
        CC_SAFE_Malloc(Source, sizeof(char) * (Size + 1));
        
        FSHandleRead(Handle, &Size, Source, FSBehaviourDefault);
        Source[Size] = 0;
        
        Expression = CCExpressionCreateFromSource(Source);
        
        FSHandleClose(Handle);
        CC_SAFE_Free(Source);
        
        const char *Filename = FSPathGetFilenameString(Path);
        CCString String = CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Filename);
        CCExpressionCreateState(Expression, CC_STRING("@file"), CCExpressionCreateString(CC_STD_ALLOCATOR, String), FALSE);
        CCStringDestroy(String);
        
        const char *Dir = FSPathGetPathString(Path);
        String = CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Dir, strlen(Dir) - strlen(Filename));
        CCExpressionCreateState(Expression, CC_STRING("@cd"), CCExpressionCreateString(CC_STD_ALLOCATOR, String), FALSE);
        CCStringDestroy(String);
    }
    
    return Expression;
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
        {
            CCString String = CCStringCreateWithSize(Allocator, (CCStringHint)CCStringEncodingASCII, Input, Length);
            CCExpression Atom = CCExpressionCreateAtom(Allocator, String);
            CCStringDestroy(String);
            
            return Atom;
        }
            
        case CCExpressionValueTypeInteger:
            return CCExpressionCreateInteger(Allocator, (int32_t)strtol(Input, NULL, 10));
            
        case CCExpressionValueTypeFloat:
            return CCExpressionCreateFloat(Allocator, (float)strtod(Input, NULL));
            
        case CCExpressionValueTypeString:
        {
            CCString String = CCStringCreateWithSize(Allocator, (CCStringHint)CCStringEncodingASCII, Input, Length);
            CCExpression Str = CCExpressionCreateString(Allocator, String);
            CCStringDestroy(String);
            
            return Str;
        }
            
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
        {
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetAtom(Expression)) printf("%s", Buffer);
            break;
        }
            
        case CCExpressionValueTypeInteger:
            printf("%" PRId32, CCExpressionGetInteger(Expression));
            break;
            
        case CCExpressionValueTypeFloat:
            printf("%f", CCExpressionGetFloat(Expression));
            break;
            
        case CCExpressionValueTypeString:
        {
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetAtom(Expression)) printf("\"%s\"", Buffer);
            break;
        }
            
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
    
    if (CCExpressionIsTagged(Expression)) return Expression;
    
    if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
    
    Expression->state.result = Expression;
    if ((CCExpressionGetType(Expression) == CCExpressionValueTypeExpression) && (CCCollectionGetCount(CCExpressionGetList(Expression))))
    {
        _Bool IsList = TRUE;
        CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
        {
            CCExpressionStateSetSuper(Expr, Expression);
        }
        
        CCExpression *Expr = CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
        if (Expr)
        {
            CCExpression Func = CCExpressionGetType(*Expr) == CCExpressionValueTypeExpression ? CCExpressionEvaluate(*Expr) : *Expr;
            
            if ((Func) && (CCExpressionGetType(Func) == CCExpressionValueTypeAtom))
            {
                CCExpressionEvaluator Eval = CCExpressionEvaluatorForName(CCExpressionGetAtom(Func));
                if (Eval)
                {
                    Expression->state.result = Eval(Expression);
                    IsList = FALSE;
                }
                
                else if (CCStringHasSuffix(CCExpressionGetAtom(Func), CC_STRING("!"))) //set state
                {
                    CCString Name = CCStringCopySubstring(CCExpressionGetAtom(Func), 0, CCStringGetLength(CCExpressionGetAtom(*Expr)) - 1);
                    CCExpression State = NULL;
                    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
                    {
                       State = CCExpressionSetState(Expression, Name, CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1)), TRUE);
                    }
                    
                    else
                    {
                        CCExpression Value = CCExpressionCreateList(Expression->allocator);
                        
                        CCEnumerator Enumerator;
                        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
                        
                        for (CCExpression *Arg = CCCollectionEnumeratorNext(&Enumerator); Arg; Arg = CCCollectionEnumeratorNext(&Enumerator))
                        {
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Value), &(CCExpression){ CCExpressionCopy(CCExpressionEvaluate(*Arg)) });
                        }
                        
                        State = CCExpressionSetState(Expression, Name, Value, FALSE);
                    }
                    
                    CCStringDestroy(Name);
                    
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
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
            
            CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
            if (Expr)
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expression->state.result), &(CCExpression){ CCExpressionCopy(CCExpressionGetResult(*Expr) ? CCExpressionGetResult(*Expr) : CCExpressionEvaluate(*Expr)) });
                
                while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                {
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expression->state.result), &(CCExpression){ CCExpressionCopy(CCExpressionEvaluate(*Expr)) });
                }
            }
        }
    }
    
    else if ((Expression->state.super) && (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)) //get state
    {
        CCExpression State = CCExpressionGetState(Expression->state.super, CCExpressionGetAtom(Expression));
        if (State) Expression->state.result = CCExpressionCopy(State);
    }
    
    if (Expression->state.result) CCExpressionStateSetSuper(Expression->state.result, Expression->state.super);
    
    return Expression->state.result;
}

typedef struct {
    CCString name;
    CCExpression value;
} CCExpressionStateValue;

static CCComparisonResult CCExpressionStateValueElementFind(const CCExpressionStateValue *left, const CCExpressionStateValue *right)
{
    return CCStringEqual(left->name, right->name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCExpressionStateValue *CCExpressionGetStateValue(CCExpression Expression, CCString Name)
{
    if (!Expression) return NULL;
    
    CCExpressionStateValue *Value = NULL;
    if (Expression->state.values) Value = CCCollectionGetElement(Expression->state.values, CCCollectionFindElement(Expression->state.values, &(CCExpressionStateValue){ .name = Name }, (CCComparator)CCExpressionStateValueElementFind));
    
    if (Value)
    {
        if (Value->value) Value->value->state.super = Expression;
        return Value;
    }
    
    return CCExpressionGetStateValue(Expression->state.super, Name);
}

static void CCExpressionStateValueElementDestructor(CCCollection Collection, CCExpressionStateValue *Element)
{
    CCStringDestroy(Element->name);
    if (Element->value) CCExpressionDestroy(Element->value);
}

void CCExpressionCreateState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Copy)
{
    if ((Expression->state.values) && (CCCollectionFindElement(Expression->state.values, &(CCExpressionStateValue){ .name = Name }, (CCComparator)CCExpressionStateValueElementFind)))
    {
        CC_LOG_ERROR_CUSTOM("Creating duplicate state (%S)", Name);
    }
    
    else
    {
        if (!Expression->state.values) Expression->state.values = CCCollectionCreate(Expression->allocator, CCCollectionHintHeavyFinding, sizeof(CCExpressionStateValue), (CCCollectionElementDestructor)CCExpressionStateValueElementDestructor);
        
        CCCollectionInsertElement(Expression->state.values, &(CCExpressionStateValue){
            .name = CCStringCopy(Name),
            .value = Copy ? (Value ? CCExpressionCopy(Value) : NULL) : Value
        });
    }
}

CCExpression CCExpressionGetState(CCExpression Expression, CCString Name)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    
    return State && State->value ? CCExpressionEvaluate(State->value) : NULL;
}

CCExpression CCExpressionSetState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Copy)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    if (State)
    {
        if (State->value) CCExpressionDestroy(State->value);
        State->value = Copy ? CCExpressionCopy(Value) : Value;
        
        return CCExpressionEvaluate(State->value);
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
            CCExpressionCreateState(Destination, State->name, State->value, TRUE);
        }
    }
    
    else if (Destination->state.values)
    {
        CCCollectionDestroy(Destination->state.values);
        Destination->state.values = NULL;
    }
    
    Destination->state.remove = NULL;
    Destination->state.result = NULL;
    Destination->state.super = Source->state.super;
}

void CCExpressionPrintState(CCExpression Expression)
{
    if (Expression->state.super)
    {
        printf("super:%p: ", Expression->state.super); CCExpressionPrint(Expression->state.super);
    }
    
    if (Expression->state.result)
    {
        printf("result:%p: ", Expression->state.result); CCExpressionPrint(Expression->state.result);
    }
    
    if (Expression->state.values)
    {
        printf("state:\n{\n");
        CC_COLLECTION_FOREACH(CCExpressionStateValue, State, Expression->state.values)
        {
            CC_STRING_TEMP_BUFFER(Buffer, State.name) printf("\t%s:%p: ", Buffer, State.value);
            
            if (State.value) CCExpressionPrint(State.value);
            else printf("\n");
        }
        printf("}\n");
    }
    
    if (Expression->state.remove)
    {
        printf("remove:\n{\n");
        CC_COLLECTION_FOREACH(CCCollectionEntry, Entry, Expression->state.remove)
        {
            CCExpression *Expr = CCCollectionGetElement(CCExpressionGetList(Expression), Entry);
            printf("\t%p: ", Expr);
            if (Expr) CCExpressionPrint(*Expr);
            else printf("\n");
        }
        printf("}\n");
    }
}
