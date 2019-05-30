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

#ifndef CommonGameKit_Expression_h
#define CommonGameKit_Expression_h

#include <CommonC/Common.h>

/*
 Enables the tagged expression optimization. Generally leave it enabled, though disabling it can
 be helpful when debugging.
 */
#define CC_EXPRESSION_ENABLE_TAGGED_TYPES 1

/*
 Enables the strict naming rules. This allows for some added optimizations to be applied, but will
 cause code that does not abide them to work differently.
 */
#define CC_EXPRESSION_STRICT_NAMING_RULES 1

/*
 Enables the constant lists optimization. Generally leave it enabled, unless under certain use cases
 where it may break things.
 */
#define CC_EXPRESSION_ENABLE_CONSTANT_LISTS 1

enum {
    CCExpressionValueTypeNull,
    CCExpressionValueTypeAtom,
    CCExpressionValueTypeInteger,
    CCExpressionValueTypeFloat,
    CCExpressionValueTypeString,
    CCExpressionValueTypeList,
    CCExpressionValueTypeExpression = CCExpressionValueTypeList,
    CCExpressionValueTypeUnspecified,
    
    CCExpressionValueTypeReservedCount = 255
};

typedef int32_t CCExpressionValueType;

enum {
    /*
     000 -> not tagged
     001 -> tagged CCString (string expression)
     010 -> tagged CCString (string expression)
     011 -> tagged CCString (string expression)
     100 -> tagged CCExpression
     
     Non-tagged CCString's are wrapped in a CCExpression.
     
     Tagged CCExpression:
     xx100 -> tagged CCExpression type
     00 -> atom
     01 -> integer
     10 -> float
     11 -> null
     */
    CCExpressionTaggedMask = 7,
    CCExpressionTaggedExtended = (1 << 2),
    CCExpressionTaggedExtendedMask = (3 << 3) | CCExpressionTaggedExtended,
    CCExpressionTaggedExtendedAtom = (0 << 3) | CCExpressionTaggedExtended,
    CCExpressionTaggedExtendedInteger = (1 << 3) | CCExpressionTaggedExtended,
    CCExpressionTaggedExtendedFloat = (2 << 3) | CCExpressionTaggedExtended,
    CCExpressionTaggedExtendedNull = (3 << 3) | CCExpressionTaggedExtended,
    
    CCExpressionTaggedBits = 5,
    CCExpressionTaggedFloatTaggedBits = CCExpressionTaggedBits,
#if CC_HARDWARE_PTR_64
    CCExpressionTaggedIntegerTaggedBits = CCExpressionTaggedBits,
    CCExpressionTaggedIntegerSignedFlag = 0,
#else
    CCExpressionTaggedIntegerSignedFlag = (1 << CCExpressionTaggedBits),
    CCExpressionTaggedIntegerTaggedBits = CCExpressionTaggedBits + 1
#endif
    
    CCExpressionTaggedAtomTaggedBits = CCExpressionTaggedBits,
    CCExpressionTaggedAtomTypeMask = 7, //CCExpressionAtomType
    
    CCExpressionTaggedFunctionIndexBits = 8,
    CCExpressionTaggedFunctionIndexMask = ~((CCExpressionTaggedAtomTypeMask << CCExpressionTaggedAtomTaggedBits) | CCExpressionTaggedMask) >> CCExpressionTaggedFunctionIndexBits
};

typedef enum {
    CCExpressionAtomTypeKindMask = 1,
    CCExpressionAtomTypeKindSymbol = (0 << 0),
    CCExpressionAtomTypeKindLookup = (1 << 0),
    
    CCExpressionAtomTypeLookupMask = (1 << 1),
    CCExpressionAtomTypeLookupState = (0 << 1),
    CCExpressionAtomTypeLookupFunction = (1 << 1),
    
    CCExpressionAtomTypeSymbol = CCExpressionAtomTypeKindSymbol,
    CCExpressionAtomTypeState = CCExpressionAtomTypeKindLookup | CCExpressionAtomTypeLookupState,
    CCExpressionAtomTypeFunction = CCExpressionAtomTypeKindLookup | CCExpressionAtomTypeLookupFunction,
    
    CCExpressionAtomTypeOperationMask = (1 << 2),
    CCExpressionAtomTypeOperationGet = (0 << 2),
    CCExpressionAtomTypeOperationSet = (1 << 2)
} CCExpressionAtomType;

/*!
 * @brief An expression.
 */
typedef struct CCExpressionValue *CCExpression;

/*!
 * @brief The expression value copy.
 * @description Create a new copy of your expression value.
 * @param Value The expression value to be copied.
 */
typedef CCExpression (*CCExpressionValueCopy)(CCExpression Value);

/*!
 * @brief The expression value destructor.
 * @description Perform all destruction for your expression value.
 * @param Data The data held by the expression value.
 */
typedef void (*CCExpressionValueDestructor)(void *Data);

typedef struct CCExpressionState {
    CCDictionary values;
    CCExpression super;
    CCExpression result;
    CCCollection remove;
    CCExpression private;
} CCExpressionState;

typedef struct CCExpressionValue {
    CCExpressionValueType type;
    union {
#if CC_EXPRESSION_STRICT_NAMING_RULES
        struct {
            CCString name;
            CCExpressionAtomType kind;
        } atom;
#else
        CCString atom;
#endif
        int32_t integer;
        float real;
        CCString string;
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
        struct {
            CCOrderedCollection items;
            _Bool constant;
        } list;
#else
        CCOrderedCollection list;
#endif
        void *data;
    };
    
    CCExpressionState state;
    CCExpressionValueCopy copy;
    CCExpressionValueDestructor destructor;
    CCAllocatorType allocator;
} CCExpressionValue;

extern const CCExpressionValueCopy CCExpressionRetainedValueCopy;


/*!
 * @brief Create an expression from a source file.
 * @description Sets some state associated with the file.
 *
 *              @b \@cd : Directory containing the file.
 *
 *              @b \@file : File name of the file.
 *
 * @param Path The path to the file containing the source.
 * @return The created expression.
 */
CC_NEW CCExpression CCExpressionCreateFromSourceFile(FSPath CC_COPY(Path));

/*!
 * @brief Create an expression from source.
 * @param Source The string representation of the expression.
 * @return The created expression.
 */
CC_NEW CCExpression CCExpressionCreateFromSource(const char *Source);

/*!
 * @brief Create an expression of type.
 * @param Allocator The allocator to be used for the expression.
 * @param Type The type of the expression.
 * @return The created expression.
 */
CC_NEW CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type);

/*!
 * @brief Create a new copy of the expression.
 * @param Expression The expression to be copied.
 * @return The created expression.
 */
CC_NEW CCExpression CCExpressionCopy(CCExpression Expression);

/*!
 * @brief Create a new copy of the expression and its super and siblings.
 * @param Expression The expression to be copied.
 * @return The created expression. This must be destroyed by obtaining the super root
 *         and destroying that (not the child expression returned by this function).
 */
CC_NEW CCExpression CCExpressionDeepCopy(CCExpression Expression);

/*!
 * @brief Retain the expression.
 * @param Expression The expression to be retained.
 * @return The retained expression.
 */
CC_NEW CCExpression CCExpressionRetain(CCExpression Expression);

/*!
 * @brief Destroy an expression.
 * @param Expression The expression to be destroyed.
 */
void CCExpressionDestroy(CCExpression CC_DESTROY(Expression));

/*!
 * @brief Print the expression in a readable format.
 * @param Expression The expression to be printed.
 */
void CCExpressionPrint(CCExpression Expression);

/*!
 * @brief Evaluate an expression.
 * @param Expression The expression to be evaluated.
 * @return The resulting expression.
 */
CCExpression CCExpressionEvaluate(CCExpression Expression);

#pragma mark - State functions

/*!
 * @brief Create state to be associated with the expression.
 * @param Expression The expression store the state.
 * @param Name The name of the state.
 * @param Value The value of the state. May be NULL.
 * @param Retain Whether the @b value should be retained or stored directly.
 * @param Invalidator The invalidator of the state. May be NULL. Invalidators are used to determine
 *        if retrieving the state should cause it to be evaluated or not (reuse its current return
 *        value).
 *
 * @param InvalidatorRetain Whether the @b invalidator should be retained or stored directly.
 */
void CCExpressionCreateState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Retain, CCExpression Invalidator, _Bool InvalidatorRetain);

/*!
 * @brief Get the state of an expression.
 * @description Checks for the existence of state in the expression and but does not check any supers.
 * @param Expression The expression to search.
 * @param Name The name of the state.
 * @return The state value or NULL if it does not exist (or currently holds NULL).
 */
CCExpression CCExpressionGetStateStrict(CCExpression Expression, CCString Name);

/*!
 * @brief Get the state of an expression.
 * @description Checks for the existence of state in the expression and supers.
 * @param Expression The expression to begin the search from.
 * @param Name The name of the state.
 * @return The state value or NULL if it does not exist (or currently holds NULL).
 */
CCExpression CCExpressionGetState(CCExpression Expression, CCString Name);

/*!
 * @brief Set the state of an expression.
 * @param Expression The expression to store the state.
 * @param Name The name of the state.
 * @param Value The value of the state. May be NULL.
 * @param Retain Whether the @b value should be retained or stored directly.
 * @return The state value or NULL if it does not exist (or currently holds NULL).
 */
CCExpression CCExpressionSetState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Retain);

/*!
 * @brief Set the invalidator of an expression's state.
 * @param Expression The expression containing the state.
 * @param Name The name of the state.
 * @param Invalidator The invalidator of the state. May be NULL. Invalidators are used to determine
 *        if retrieving the state should cause it to be evaluated or not (reuse its current return
 *        value).
 *
 * @param Retain Whether the @b invalidator should be retained or stored directly.
 */
void CCExpressionSetStateInvalidator(CCExpression Expression, CCString Name, CCExpression Invalidator, _Bool Retain);

/*!
 * @brief Copy the the state of one expression to another.
 * @param Source The expression to copy the state from.
 * @param Destination The expression to copy the state into.
 */
void CCExpressionCopyState(CCExpression Source, CCExpression Destination);

/*!
 * @brief Print the state of an expression.
 * @param Expression The expression to print the state of.
 */
void CCExpressionPrintState(CCExpression Expression);

#pragma mark -

//Convenience getters and initializers for when introducing tagged types.
CC_NEW CCExpression CCExpressionCreateNull(CCAllocatorType Allocator);
CC_NEW CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, CCString Atom, _Bool Copy);
CC_NEW CCExpression CCExpressionCreateInteger(CCAllocatorType Allocator, int32_t Value);
CC_NEW CCExpression CCExpressionCreateFloat(CCAllocatorType Allocator, float Value);
CC_NEW CCExpression CCExpressionCreateString(CCAllocatorType Allocator, CCString String, _Bool Copy);
CC_NEW CCExpression CCExpressionCreateList(CCAllocatorType Allocator);
CC_NEW CCExpression CCExpressionCreateCustomType(CCAllocatorType Allocator, CCExpressionValueType Type, void *Data, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor);
void CCExpressionChangeOwnership(CCExpression Expression, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor);

static inline _Bool CCExpressionIsTagged(CCExpression Expression);
CCExpressionValueType CCExpressionGetTaggedType(uintptr_t Expression);
static inline CCExpressionValueType CCExpressionGetType(CCExpression Expression);
static inline CCExpressionValueCopy CCExpressionGetCopy(CCExpression Expression);
static inline CCExpressionValueDestructor CCExpressionGetDestructor(CCExpression Expression);
static inline CCExpression CCExpressionGetResult(CCExpression Expression);
static inline CCString CCExpressionGetAtom(CCExpression Expression);
#if CC_EXPRESSION_STRICT_NAMING_RULES
CCString CCExpressionGetAtomFunctionName(size_t Index);
static inline CCExpressionAtomType CCExpressionGetAtomType(CCExpression Expression);
#endif
static inline int32_t CCExpressionGetInteger(CCExpression Expression);
static inline float CCExpressionGetFloat(CCExpression Expression);
static inline CCString CCExpressionGetString(CCExpression Expression);
static inline CCOrderedCollection CCExpressionGetList(CCExpression Expression);
static inline void *CCExpressionGetData(CCExpression Expression);

static inline CCExpression CCExpressionStateGetSuperRoot(CCExpression Expression);
static inline CCExpression CCExpressionStateGetSuper(CCExpression Expression);
static inline void CCExpressionStateSetSuper(CCExpression Expression, CCExpression Super);
static inline CCExpression CCExpressionStateGetResult(CCExpression Expression);
static inline void CCExpressionStateSetResult(CCExpression Expression, CCExpression Result);
static inline CCExpression CCExpressionStateGetPrivate(CCExpression Expression);
static inline void CCExpressionStateSetPrivate(CCExpression Expression, CCExpression Private);



static inline _Bool CCExpressionIsTagged(CCExpression Expression)
{
#if CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return (uintptr_t)Expression & CCExpressionTaggedMask;
#else
    return FALSE;
#endif
}

static inline CCExpression CCExpressionGetResult(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? Expression : Expression->state.result;
}

static inline CCExpressionValueCopy CCExpressionGetCopy(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? NULL : Expression->copy;
}

static inline CCExpressionValueDestructor CCExpressionGetDestructor(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? NULL : Expression->destructor;
}

static inline CCExpressionValueType CCExpressionGetType(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? CCExpressionGetTaggedType((uintptr_t)Expression) : Expression->type;
}

static inline CCString CCExpressionGetAtom(CCExpression Expression)
{
#if CC_EXPRESSION_STRICT_NAMING_RULES
    if (CCExpressionIsTagged(Expression))
    {
        return CCExpressionGetAtomFunctionName((((uintptr_t)Expression >> CCExpressionTaggedFunctionIndexBits) & CCExpressionTaggedFunctionIndexMask));
    }
    
    return Expression->atom.name;
#else
    return Expression->atom;
#endif
}

#if CC_EXPRESSION_STRICT_NAMING_RULES
static inline CCExpressionAtomType CCExpressionGetAtomType(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? (CCExpressionAtomType)(((uintptr_t)Expression >> CCExpressionTaggedAtomTaggedBits) & CCExpressionTaggedAtomTypeMask) : Expression->atom.kind;
}
#endif

static inline int32_t CCExpressionGetInteger(CCExpression Expression)
{
#if CC_HARDWARE_PTR_64 && CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return (int32_t)((uintptr_t)Expression >> CCExpressionTaggedIntegerTaggedBits);
#else
    return CCExpressionIsTagged(Expression) ? (((uintptr_t)Expression & CCExpressionTaggedIntegerSignedFlag) ? (int32_t)~((uintptr_t)Expression >> CCExpressionTaggedIntegerTaggedBits) : (int32_t)((uintptr_t)Expression >> CCExpressionTaggedIntegerTaggedBits)) : Expression->integer;
#endif
}

static inline float CCExpressionGetFloat(CCExpression Expression)
{
#if CC_HARDWARE_PTR_64 && CC_EXPRESSION_ENABLE_TAGGED_TYPES
    return *(float*)&(uint32_t){ (uint32_t)((uintptr_t)Expression >> CCExpressionTaggedFloatTaggedBits) };
#else
    return CCExpressionIsTagged(Expression) ? *(float*)&(int32_t){ ((uintptr_t)Expression & 0xff800000) | (((uintptr_t)Expression & 0x1ffff) >> CCExpressionTaggedFloatTaggedBits) } : Expression->real;
#endif
}

static inline CCString CCExpressionGetString(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? (CCString)Expression : Expression->string;
}

static inline CCOrderedCollection CCExpressionGetList(CCExpression Expression)
{
#if CC_EXPRESSION_ENABLE_CONSTANT_LISTS
    return Expression->list.items;
#else
    return Expression->list;
#endif
}

static inline void *CCExpressionGetData(CCExpression Expression)
{
    return Expression->data;
}

static inline CCExpression CCExpressionStateGetSuperRoot(CCExpression Expression)
{
    CCExpression Root = Expression;
    for (CCExpression Super = Expression; (Super = CCExpressionStateGetSuper(Super)); Root = Super);
    
    return Root;
}

static inline CCExpression CCExpressionStateGetSuper(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? NULL : Expression->state.super;
}

static inline void CCExpressionStateSetSuper(CCExpression Expression, CCExpression Super)
{
    if (!CCExpressionIsTagged(Expression)) Expression->state.super = Super;
}

static inline CCExpression CCExpressionStateGetResult(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? NULL : Expression->state.result;
}

static inline void CCExpressionStateSetResult(CCExpression Expression, CCExpression Result)
{
    if (!CCExpressionIsTagged(Expression))
    {
        if ((Expression->state.result) && (Expression->state.result != Expression)) CCExpressionDestroy(Expression->state.result);
        Expression->state.result = Result;
    }
}

static inline CCExpression CCExpressionStateGetPrivate(CCExpression Expression)
{
    return CCExpressionIsTagged(Expression) ? NULL : Expression->state.private;
}

static inline void CCExpressionStateSetPrivate(CCExpression Expression, CCExpression Private)
{
    if (!CCExpressionIsTagged(Expression))
    {
        if (Expression->state.private) CCExpressionDestroy(Expression->state.private);
        Expression->state.private = Private;
    }
}

#endif
