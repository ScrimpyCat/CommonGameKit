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
static CCExpression CCExpressionRetainValueCopy(CCExpression Value);

const CCExpressionValueCopy CCExpressionRetainedValueCopy = CCExpressionRetainValueCopy;


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
    
    CCOrderedCollection List = CCExpressionGetList(Copy);
    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Value))
    {
        CCExpression CopiedElement = CCExpressionCopy(Element);
        CCExpressionStateSetSuper(CopiedElement, Copy);
        
        CCOrderedCollectionAppendElement(List, &CopiedElement);
    }
    
    return Copy;
}

static CCExpression CCExpressionRetainValueCopy(CCExpression Value)
{
    return CCExpressionCreateCustomType(Value->allocator, CCExpressionGetType(Value), CCRetain(CCExpressionGetData(Value)), Value->copy, Value->destructor);
}

static void CCExpressionDestructor(CCExpression Expression)
{
    if (Expression->destructor) Expression->destructor(CCExpressionGetData(Expression));
    if (Expression->state.values) CCDictionaryDestroy(Expression->state.values);
    if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
    if (Expression->state.private) CCExpressionDestroy(Expression->state.private);
}

CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type)
{
    CCExpression Expression = CCMalloc(CC_ALIGNED_ALLOCATOR(8)/*Allocator*/, sizeof(*Expression), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (!Expression)
    {
        CC_LOG_ERROR("Failed to create expression due to allocation failing. Allocation size: %zu", sizeof(*Expression));
        return NULL;
    }
    
    CCMemorySetDestructor(Expression, (CCMemoryDestructorCallback)CCExpressionDestructor);
    
    Expression->type = Type;
    switch (Type)
    {
        case CCExpressionValueTypeAtom:
        case CCExpressionValueTypeString:
            Expression->destructor = (CCExpressionValueDestructor)CCStringDestroy;
            Expression->copy = CCExpressionValueAtomOrStringCopy;
            break;
            
        case CCExpressionValueTypeList:
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
            Expression->list.items = CCCollectionCreateWithImplementation(Allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), (CCCollectionElementDestructor)CCExpressionElementDestructor, CCCollectionFastArray);
            Expression->list.constant = FALSE;
#else
            Expression->list = CCCollectionCreateWithImplementation(Allocator, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCExpression), (CCCollectionElementDestructor)CCExpressionElementDestructor, CCCollectionFastArray);
#endif
            Expression->destructor = (CCExpressionValueDestructor)CCCollectionDestroy;
            Expression->copy = CCExpressionValueListCopy;
            break;
            
        default:
            Expression->destructor = NULL;
            Expression->copy = NULL;
            break;
    }
    
    Expression->state = (CCExpressionState){ .values = NULL, .super = NULL, .result = NULL, .remove = NULL, .private = NULL };
    Expression->allocator = Allocator;
    
    return Expression;
}

CCExpression CCExpressionCreateNull(CCAllocatorType Allocator)
{
#if CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return (CCExpression)CCExpressionTaggedExtendedNull;
#else
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeNull);
    
    return Expression;
#endif
}

CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, CCString Atom, _Bool Copy)
{
    const CCChar Prefix = CCStringGetCharacterAtIndex(Atom, 0), Suffix = CCStringGetCharacterAtIndex(Atom, CCStringGetLength(Atom) - 1);
#if CC_EXPRESSION_ENABLE_TAGGED_TYPES && CC_EXPRESSION_STRICT_NAMING_RULES
    if ((Prefix == '.') || (Prefix == '@') || (Prefix == '&'))
    {
        
    }
    
    else if ((Prefix == ':') || (Suffix == ':'))
    {
        
    }
    
    else
    {
        uintptr_t EvalIndex = CCExpressionEvaluatorIndexForName(Atom);
        if ((EvalIndex != SIZE_MAX) && ((EvalIndex & CCExpressionTaggedFunctionIndexMask) == EvalIndex))
        {
            if (!Copy) CCStringDestroy(Atom);
            
            return (CCExpression)((EvalIndex << CCExpressionTaggedFunctionIndexBits) | (CCExpressionAtomTypeFunction << CCExpressionTaggedAtomTaggedBits) | CCExpressionTaggedExtendedAtom);
        }
    }
#endif
    
    CCExpression Expression = CCExpressionCreate(Allocator, CCExpressionValueTypeAtom);
#if CC_EXPRESSION_STRICT_NAMING_RULES
    if ((Prefix == '.') || (Prefix == '@') || (Prefix == '&'))
    {
        Expression->atom.kind = CCExpressionAtomTypeState | (Suffix == '!' ? CCExpressionAtomTypeOperationSet : CCExpressionAtomTypeOperationGet);
    }
    
    else if ((Prefix == ':') || (Suffix == ':'))
    {
        Expression->atom.kind = CCExpressionAtomTypeSymbol;
    }
    
    else Expression->atom.kind = CCExpressionAtomTypeFunction;
    
    Expression->atom.name = Copy ? CCStringCopy(Atom) : Atom;
#else
    Expression->atom = Copy ? CCStringCopy(Atom) : Atom;
#endif
    
    return Expression;
}

#if CC_EXPRESSION_STRICT_NAMING_RULES
CCString CCExpressionGetAtomFunctionName(size_t Index)
{
    return CCExpressionEvaluatorNameForIndex(Index);
}
#endif

CCExpressionEvaluator CCExpressionGetFunctionEvaluator(CCExpression Expression)
{
    CCExpressionEvaluator Eval = NULL;
    if ((Expression) && (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom))
    {
#if CC_EXPRESSION_STRICT_NAMING_RULES
        if (CCExpressionGetAtomType(Expression) == CCExpressionAtomTypeFunction)
        {
            Eval = CCExpressionIsTagged(Expression) ? CCExpressionEvaluatorForIndex((uintptr_t)Expression >> CCExpressionTaggedFunctionIndexBits) : CCExpressionEvaluatorForName(CCExpressionGetAtom(Expression));
        }
#else
        Eval = CCExpressionEvaluatorForName(CCExpressionGetAtom(Expression));
#endif
    }
    
    return Eval;
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
    
    CC_SAFE_Free(Expression);
}

CCExpressionValueType CCExpressionGetTaggedType(uintptr_t Expression)
{
    if ((Expression & CCExpressionTaggedMask) == CCExpressionTaggedExtended)
    {
        switch (Expression & CCExpressionTaggedExtendedMask)
        {
            case CCExpressionTaggedExtendedNull:
                return CCExpressionValueTypeNull;
                
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

static CCExpression CCExpressionDeepFindEquivalentExpression(CCExpression Root, CCExpression Expression)
{
    CCExpression Super = CCExpressionStateGetSuper(Expression);
    if (!Super) return Root;
    
    Root = CCExpressionDeepFindEquivalentExpression(Root, Super);
    
    CCOrderedCollection List = CCExpressionGetList(Super);
    const size_t Index = CCOrderedCollectionGetIndex(List, CCCollectionFindElement(List, &Expression, NULL));
    
    return *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Root), Index);
}

CCExpression CCExpressionDeepCopy(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if (CCExpressionIsTagged(Expression)) return Expression;
    
    CCExpression Root = CCExpressionCopy(CCExpressionStateGetSuperRoot(Expression));
    
    return CCExpressionDeepFindEquivalentExpression(Root, Expression);
}

CCExpression CCExpressionRetain(CCExpression Expression)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    return CCExpressionIsTagged(Expression) ? Expression : CCRetain(Expression);
}

CCExpression CCExpressionCreateFromSourceFile(FSPath Path)
{
    CCExpression Expression = NULL;
    
    FSHandle Handle;
    if (FSHandleOpen(Path, FSHandleTypeRead, &Handle) == FSOperationSuccess)
    {
        size_t Size = FSManagerGetSize(Path);
        char *Source;
        CC_SAFE_Malloc(Source, sizeof(char) * (Size + 1),
                       CC_LOG_ERROR("Failed to create expression from source due to allocation failure. Allocation size (%zu)", sizeof(char) * (Size + 1));
                       return NULL;
                       );
        
        FSHandleRead(Handle, &Size, Source, FSBehaviourDefault);
        Source[Size] = 0;
        
        Expression = CCExpressionCreateFromSource(Source);
        
        FSHandleClose(Handle);
        CC_SAFE_Free(Source);
        
        const char *Filename = FSPathGetFilenameString(Path);
        CCExpressionCreateState(Expression, CC_STRING("@file"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Filename), FALSE), FALSE, NULL, FALSE);
        
        const char *Dir = FSPathGetPathString(Path);
        CCExpressionCreateState(Expression, CC_STRING("@cd"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreateWithSize(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, Dir, strlen(Dir) - strlen(Filename)), FALSE), FALSE, NULL, FALSE);
    }
    
    else CC_LOG_ERROR("Failed to create expression from source due to error opening source: %s", FSPathGetFullPathString(Path));
    
    return Expression;
}

CCExpression CCExpressionCreateFromSource(const char *Source)
{
    CCAssertLog(Source, "Source must not be NULL");
    
    return CCExpressionParse(&Source);
}

static CCString CCExpressionStringConvertEscapes(CCAllocatorType Allocator, const char *Input, size_t Length)
{
    CCString s1 = CCStringCreateWithSize(Allocator, CCStringEncodingASCII | CCStringHintCopy, Input, Length);
    
    CCString s2 = CCStringCreateByReplacingOccurrencesOfGroupedStrings(s1, (CCString[4]){
        CC_STRING("\\\""), CC_STRING("\\n"), CC_STRING("\\t"), CC_STRING("\\\\")
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
            else return CCExpressionCreateAtom(Allocator, CCStringCreateWithSize(Allocator, CCStringEncodingASCII | CCStringHintCopy, Input, Length), FALSE);
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

#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
static _Bool CCExpressionIsConstant(CCExpression Expression)
{
    switch (CCExpressionGetType(Expression))
    {
        case CCExpressionValueTypeInteger:
        case CCExpressionValueTypeFloat:
        case CCExpressionValueTypeString:
            return TRUE;
            
        case CCExpressionValueTypeList:
            return Expression->list.constant;
            
        case CCExpressionValueTypeAtom:
            return (CCExpressionGetAtomType(Expression) & CCExpressionAtomTypeKindMask) == CCExpressionAtomTypeKindSymbol;
    }
    
    return FALSE;
}
#endif

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
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
                if (Expr->list.constant) Expr->list.constant = CCExpressionIsConstant(Val);
#endif
            }
            
            else
            {
                Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeExpression);
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
                Expr->list.constant = TRUE;
#endif
            }
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
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
                if (Expr->list.constant) Expr->list.constant = CCExpressionIsConstant(Val);
#endif
                
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
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
                if (Expr->list.constant) Expr->list.constant = CCExpressionIsConstant(Val);
#endif
                
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
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS && CC_EXPRESSION_STRICT_NAMING_RULES
                if (Expr->list.constant) Expr->list.constant = CCExpressionIsConstant(Val);
#endif
                
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
    const CCExpressionValueType Type = CCExpressionGetType(Expression);
    switch (Type)
    {
        case CCExpressionValueTypeNull:
            printf("<null>");
            break;
            
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
        {
#if CC_HARDWARE_ENDIAN_LITTLE
            char Identifier[5] = { (Type >> 24) & 0xff, (Type >> 16) & 0xff, (Type >> 8) & 0xff, Type & 0xff, 0 };
#else
            char Identifier[5] = { Type & 0xff, (Type >> 8) & 0xff, (Type >> 16) & 0xff, (Type >> 24) & 0xff, 0 };
#endif
            printf("<%p:%s>", Expression, Identifier);
            break;
        }
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
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
        if (Expression->list.constant) return Expression;
#endif
        
        _Bool IsList = TRUE;
        CC_COLLECTION_FOREACH(CCExpression, Expr, CCExpressionGetList(Expression))
        {
            CCExpressionStateSetSuper(Expr, Expression);
        }
        
        CCExpression *Expr = CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
        if (Expr)
        {
            CCExpression Func = CCExpressionEvaluate(*Expr);
            
            if ((Func) && (CCExpressionGetType(Func) == CCExpressionValueTypeAtom))
            {
                CCExpressionEvaluator Eval = CCExpressionGetFunctionEvaluator(Func);
                
                if (Eval)
                {
                    Expression->state.result = Eval(Expression);
                    IsList = FALSE;
                }
                
#if CC_EXPRESSION_STRICT_NAMING_RULES
                else if (CCExpressionGetAtomType(Func) == (CCExpressionAtomTypeState | CCExpressionAtomTypeOperationSet)) //set state
#else
                else if (CCStringHasSuffix(CCExpressionGetAtom(Func), CC_STRING("!"))) //set state
#endif
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
                            CCOrderedCollectionAppendElement(CCExpressionGetList(Value), &(CCExpression){ CCExpressionRetain(CCExpressionEvaluate(*Arg)) });
                        }
                        
                        State = CCExpressionSetState(Expression, Name, Value, FALSE);
                    }
                    
                    CCStringDestroy(Name);
                    
                    if (State)
                    {
                        Expression->state.result = CCExpressionRetain(State);
                        IsList = FALSE;
                    }
                }
            }
            
            if (!Expression->state.result) //mark expression for removal
            {
                CCExpression Super = Expression->state.super;
                if (!Super->state.remove) Super->state.remove = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall, sizeof(CCCollectionEntry), NULL);
                
                CCCollectionEntry Entry = CCCollectionFindElement(CCExpressionGetList(Super), &Expression, NULL);
                if (Entry) CCCollectionInsertElement(Super->state.remove, &Entry);
                
                Expression->state.result = CCExpressionCreateNull(CC_STD_ALLOCATOR);
            }
        }
        
        if (Expression->state.remove) //remove expression
        {
            CCCollectionRemoveCollection(CCExpressionGetList(Expression), Expression->state.remove);
            CCCollectionDestroy(Expression->state.remove);
            Expression->state.remove = NULL;
        }
        
        if (IsList) //evaluate list
        {
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
            Expression->list.constant = TRUE;
#endif
            
            Expression->state.result = CCExpressionCreateList(Expression->allocator);
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
            
            CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
            if (Expr)
            {
                CCExpression Item = CCExpressionRetain(CCExpressionGetResult(*Expr));
                CCOrderedCollectionAppendElement(CCExpressionGetList(Expression->state.result), &Item);
                
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
                if ((Item != *Expr) || ((CCExpressionGetType(Item) == CCExpressionValueTypeExpression) && (!Item->list.constant))) Expression->list.constant = FALSE;
#endif
                
                while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
                {
                    Item = CCExpressionRetain(CCExpressionEvaluate(*Expr));
                    CCOrderedCollectionAppendElement(CCExpressionGetList(Expression->state.result), &Item);
                    
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
                    if ((Item != *Expr) || ((CCExpressionGetType(Item) == CCExpressionValueTypeExpression) && (!Item->list.constant))) Expression->list.constant = FALSE;
#endif
                }
            }
        }
    }
    
#if CC_EXPRESSION_STRICT_NAMING_RULES
    else if ((Expression->state.super) && (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom) && (CCExpressionGetAtomType(Expression) == (CCExpressionAtomTypeState | CCExpressionAtomTypeOperationGet))) //get state
#else
    else if ((Expression->state.super) && (CCExpressionGetType(Expression) == CCExpressionValueTypeAtom)) //get state
#endif
    {
        CCExpression State = CCExpressionGetState(Expression->state.super, CCExpressionGetAtom(Expression));
        if (State) Expression->state.result = CCExpressionRetain(State);
    }
    
    if (Expression->state.result) CCExpressionStateSetSuper(Expression->state.result, Expression->state.super);
    
    return Expression->state.result;
}

typedef struct {
    CCExpression value;
    CCExpression invalidate;
} CCExpressionStateValue;

static CCExpressionStateValue *CCExpressionGetStateValue(CCExpression Expression, CCString Name)
{
    if (!Expression) return NULL;
    
    CCExpressionStateValue *Value = NULL;
    if (Expression->state.values) Value = CCDictionaryGetValue(Expression->state.values, &Name);
    
    if (Value)
    {
        if (Value->value) CCExpressionStateSetSuper(Value->value, Expression);
        if (Value->invalidate) CCExpressionStateSetSuper(Value->invalidate, Expression);
        return Value;
    }
    
    return CCExpressionGetStateValue(Expression->state.super, Name);
}

static void CCExpressionStateValueElementDestructor(CCDictionary Dictionary, CCExpressionStateValue *Element)
{
    if (Element->value) CCExpressionDestroy(Element->value);
    if (Element->invalidate) CCExpressionDestroy(Element->invalidate);
}

void CCExpressionCreateState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Retain, CCExpression Invalidator, _Bool InvalidatorRetain)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    if ((Expression->state.values) && (CCDictionaryFindKey(Expression->state.values, &Name)))
    {
        CC_LOG_ERROR_CUSTOM("Creating duplicate state (%S)", Name);
    }
    
    else
    {
        if (!Expression->state.values) Expression->state.values = CCDictionaryCreate(Expression->allocator, CCDictionaryHintSizeSmall | CCDictionaryHintHeavyFinding, sizeof(CCString), sizeof(CCExpressionStateValue), &(CCDictionaryCallbacks){
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary,
            .keyDestructor = CCStringDestructorForDictionary,
            .valueDestructor = (CCDictionaryElementDestructor)CCExpressionStateValueElementDestructor
        });
        
        CCExpressionStateValue StateValue = { .value = NULL, .invalidate = NULL };
        if (Invalidator)
        {
            StateValue.invalidate = InvalidatorRetain ? CCExpressionRetain(Invalidator) : Invalidator;
            
            if (Value)
            {
                StateValue.value = CCExpressionCopy(Value);
                if (!Retain) CCExpressionDestroy(Value);
            }
        }
        
        else if (Value) StateValue.value = Retain ? CCExpressionRetain(Value) : Value;
        
        CCDictionarySetValue(Expression->state.values, &(CCString){ CCStringCopy(Name) }, &StateValue);
    }
}

CCExpression CCExpressionGetStateStrict(CCExpression Expression, CCString Name)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = NULL;
    if (Expression->state.values) State = CCDictionaryGetValue(Expression->state.values, &Name);
    
    if ((State) && (State->value))
    {
        CCExpression Result = CCExpressionGetResult(State->value);
        if ((Result) && (State->invalidate))
        {
            CCExpression Invalidate = CCExpressionEvaluate(State->invalidate);
            if (CCExpressionGetType(Invalidate) == CCExpressionValueTypeInteger)
            {
                if (!CCExpressionGetInteger(Invalidate)) return Result;
            }
            
            else CC_LOG_ERROR_CUSTOM("State (%S) invalidator is not valid, should return a boolean", Name);
        }
        
        return CCExpressionEvaluate(State->value);
    }
    
    return NULL;
}

CCExpression CCExpressionGetState(CCExpression Expression, CCString Name)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    
    if ((State) && (State->value))
    {
        CCExpression Result = CCExpressionGetResult(State->value);
        if (State->invalidate)
        {
            CCExpression Invalidate = CCExpressionEvaluate(State->invalidate);
            if (Result)
            {
                if (CCExpressionGetType(Invalidate) == CCExpressionValueTypeInteger)
                {
                    if (!CCExpressionGetInteger(Invalidate)) return Result;
                }
                
                else CC_LOG_ERROR_CUSTOM("State (%S) invalidator is not valid, should return a boolean", Name);
            }
        }
        
        return CCExpressionEvaluate(State->value);
    }
    
    return NULL;
}

CCExpression CCExpressionSetState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Retain)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    if (State)
    {
        if (State->value) CCExpressionDestroy(State->value);
        
        if (State->invalidate)
        {
            if (Value)
            {
                State->value = CCExpressionCopy(Value);
                if (!Retain) CCExpressionDestroy(Value);
            }
        }
        
        else if (Value) State->value = Retain ? CCExpressionRetain(Value) : Value;
        
        return State->value;
    }
    
    return NULL;
}

void CCExpressionSetStateInvalidator(CCExpression Expression, CCString Name, CCExpression Invalidator, _Bool Retain)
{
    CCAssertLog(Expression, "Expression must not be NULL");
    
    CCExpressionStateValue *State = CCExpressionGetStateValue(Expression, Name);
    if (State)
    {
        if (State->invalidate) CCExpressionDestroy(State->invalidate);
        else if (State->value)
        {
            CCExpression Value = State->value;
            State->value = CCExpressionCopy(Value);
            CCExpressionDestroy(Value);
        }
        
        State->invalidate = Retain ? (Invalidator ? CCExpressionRetain(Invalidator) : NULL) : Invalidator;
    }
}

void CCExpressionCopyState(CCExpression Source, CCExpression Destination)
{
    CCAssertLog(Source && Destination, "Source and destination expressions must not be NULL");
    
    if ((CCExpressionIsTagged(Source)) || (CCExpressionIsTagged(Destination))) return;
    
    if (Source->state.values)
    {
        CC_DICTIONARY_FOREACH_KEY(CCString, Key, Source->state.values)
        {
            CCExpressionStateValue *State = CCDictionaryGetEntry(Source->state.values, CCDictionaryEnumeratorGetEntry(&CC_DICTIONARY_CURRENT_KEY_ENUMERATOR));
            CCExpressionCreateState(Destination, Key, State->value, TRUE, State->invalidate, TRUE);
        }
    }
    
    else if (Destination->state.values)
    {
        CCDictionaryDestroy(Destination->state.values);
        Destination->state.values = NULL;
    }
    
    Destination->state.private = NULL;
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
        CC_DICTIONARY_FOREACH_KEY(CCString, Key, Expression->state.values)
        {
            CCExpressionStateValue *Value = CCDictionaryGetEntry(Expression->state.values, CCDictionaryEnumeratorGetEntry(&CC_DICTIONARY_CURRENT_KEY_ENUMERATOR));
            
            CC_STRING_TEMP_BUFFER(Buffer, Key) printf("\t%s:%p: ", Buffer, Value->value);
            
            if (Value->value) CCExpressionPrint(Value->value);
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
