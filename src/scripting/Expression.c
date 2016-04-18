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
    return (CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionCreateAtom : CCExpressionCreateString)(Value->allocator, CCExpressionGetType(Value) == CCExpressionValueTypeAtom ? CCExpressionGetAtom(Value) : CCExpressionGetString(Value), TRUE);
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

CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, CCString Atom, _Bool Copy)
{
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeAtom);
    Expression->atom = Copy ? CCStringCopy(Atom) : Atom;
    
    return Expression;
}

CCExpression CCExpressionCreateInteger(CCAllocatorType Allocator, int32_t Value)
{
#if CC_HARDWARE_PTR_64 && CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return (CCExpression)(((uintptr_t)Value << CCExpressionTaggedIntegerTaggedBits) | CCExpressionTaggedExtendedInteger);
#else
#if CC_EXPRESSION_ENABLE_TAGGED_TYPES
    _Static_assert((CCExpressionTaggedExtendedMask | CCExpressionTaggedMask) == 31, "If base tagged size changes this must be changed");
    
    /*
     (vvvvvvvv vvvvvvvv vvvvvvvv vvvvvvvv) vvvvvvvv vvvvvvvv vvvvvvvv vvsxxxxx
     x = tagged bits
     s = signed bit
     v = value bits
     
     5 bits = tagging
     1 bit = signed flag
     (sizeof(uintptr_t) * 8) - 6 = value bits
     
     If signed flag is set, then it counts down from -1 (high range).
     So 32-bits the range of a tagged integer is: -67108864 to 67108863
     64-bits can fit the entire range INT32_MIN to INT32_MAX
     */
    
    const uintptr_t MaxValue = UINTPTR_MAX >> CCExpressionTaggedIntegerTaggedBits;
    if ((Value <= MaxValue) || (Value >= (-MaxValue - 1)))
    {
        _Static_assert(sizeof(Value) == 4, "Need to change the signed bit as is for int32_t");
        
        const _Bool Signed = Value & 0x80000000;
        if (Signed)
        {
            return (CCExpression)(((uintptr_t)~(-1 + (Value + 1)) << CCExpressionTaggedIntegerTaggedBits) | CCExpressionTaggedIntegerSignedFlag | CCExpressionTaggedExtendedInteger);
        }
        
        else
        {
            return (CCExpression)(((uintptr_t)Value << CCExpressionTaggedIntegerTaggedBits) | CCExpressionTaggedExtendedInteger);
        }
    }
#endif
    
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeInteger);
    Expression->integer = Value;
    
    return Expression;
#endif
}

CCExpression CCExpressionCreateFloat(CCAllocatorType Allocator, float Value)
{
#if CC_HARDWARE_PTR_64 && CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return (CCExpression)(((uintptr_t)*(uint32_t*)&Value << CCExpressionTaggedFloatTaggedBits) | CCExpressionTaggedExtendedFloat);
#else
#if CC_HARDWARE_PTR_32 && CC_EXPRESSION_ENABLE_TAGGED_TYPES
    /*
     eeeeeeee mmmmmmmm mmmmmmmm mmsxxxxx
     x = tagged bits
     s = signed bit
     m = mantissa bits (only thing reduced from normal binary32)
     e = exponent bits
     
     This isn't a perfect implementation, as unsure what ranges of values would really
     care about. But it's fast to implement.
     
     If revisiting in the future:
        - we can probably assume we care about 0.0 - 1.0,
        - probably don't need denormals or NANs, or INFINITY
        - probably don't care about very large floats either, so probably only wouldn't want
        an exponent higher than 140 or somewhere around there.
     */
    
    _Static_assert(sizeof(Value) == 4, "Need to change the format as is for float/binary32");
    
    const uint32_t v = *(uint32_t*)&Value;
    const uint32_t Mantissa = v & 0x7fffff;
    if ((Mantissa & 0x1ffff) == Mantissa)
    {
        return (CCExpression)((uintptr_t)((v & 0xff800000) | (Mantissa << CCExpressionTaggedFloatTaggedBits) | CCExpressionTaggedExtendedFloat));
    }
#endif
    
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeFloat);
    Expression->real = Value;
    
    return Expression;
#endif
}

CCExpression CCExpressionCreateString(CCAllocatorType Allocator, CCString Input, _Bool Copy)
{
    CCString String = Copy ? CCStringCopy(Input) : Input;
#if CC_EXPRESSION_ENABLE_TAGGED_TYPES
    if (String & 3) return (CCExpression)String;
#endif
    
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
        CCExpressionCreateState(Expression, CC_STRING("@file"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Filename), FALSE), FALSE);
        
        const char *Dir = FSPathGetPathString(Path);
        CCExpressionCreateState(Expression, CC_STRING("@cd"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Dir, strlen(Dir) - strlen(Filename)), FALSE), FALSE);
    }
    
    return Expression;
}

CCExpression CCExpressionCreateFromSource(const char *Source)
{
    CCAssertLog(Source, "Source must not be NULL");
    
    return CCExpressionParse(&Source);
}

static CCString CCExpressionStringConvertEscapes(CCAllocatorType Allocator, const char *Input, size_t Length)
{
    CCString s1 = CCStringCreateWithSize(Allocator, (CCStringHint)CCStringEncodingASCII, Input, Length);
    
    CCString s2 = CCStringCreateByReplacingOccurrencesOfGroupedStrings(s1, (CCString[4]){
        CC_STRING("\\\""), CC_STRING("\\\n"), CC_STRING("\\\t"), CC_STRING("\\\\")
    }, (CCString[4]){
        CC_STRING("\""), CC_STRING("\n"), CC_STRING("\t"), CC_STRING("\\")
    }, 4);
    
    CCStringDestroy(s1);
    
    return s2;
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
            if (!strncmp(Input, "#f", Length)) return CCExpressionCreateInteger(Allocator, 0);
            else if (!strncmp(Input, "#t", Length)) return CCExpressionCreateInteger(Allocator, 1);
            else return CCExpressionCreateAtom(Allocator, CCStringCreateWithSize(Allocator, (CCStringHint)CCStringEncodingASCII, Input, Length), FALSE);
        }
            
        case CCExpressionValueTypeInteger:
            return CCExpressionCreateInteger(Allocator, (int32_t)strtol(Input, NULL, 10));
            
        case CCExpressionValueTypeFloat:
            return CCExpressionCreateFloat(Allocator, (float)strtod(Input, NULL));
            
        case CCExpressionValueTypeString:
            return CCExpressionCreateString(Allocator, CCExpressionStringConvertEscapes(Allocator, Input, Length), FALSE);
            
        default:
            break;
    }
    
    return NULL;
}

static CCExpression CCExpressionParse(const char **Source)
{
    CCExpression Expr = NULL;
    const char *Value = NULL;
    _Bool IsStr = FALSE, IsComment = FALSE, IsEscape = FALSE;
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
        
        else if ((Expr) && (!IsComment) && (!IsEscape) && (c == '"'))
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
        
        else if ((!IsStr) && (c == ';'))
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
        
        else if (IsEscape)
        {
            IsEscape = FALSE;
        }
        
        else if (c == '\\')
        {
            IsEscape = TRUE;
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
            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Expression)) printf("\"%s\"", Buffer);
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
        if (Value->value) CCExpressionStateSetSuper(Value->value, Expression);
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
