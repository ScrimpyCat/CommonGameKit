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

#ifndef Blob_Game_Expression_h
#define Blob_Game_Expression_h

#include <CommonC/Common.h>

typedef enum {
    CCExpressionValueTypeAtom,
    CCExpressionValueTypeInteger,
    CCExpressionValueTypeFloat,
    CCExpressionValueTypeString,
    CCExpressionValueTypeList,
    CCExpressionValueTypeExpression = CCExpressionValueTypeList,
    CCExpressionValueTypeUnspecified,
    
    CCExpressionValueTypeReservedCount = 20,
} CCExpressionValueType;

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
    CCCollection values;
    CCExpression super;
    CCExpression result;
    CCCollection remove;
} CCExpressionState;

typedef struct CCExpressionValue {
    //TODO: Include tagged expression types.
    /*
     Where integers (that can fit) and atoms can be tagged. Tagged atoms could be a way to avoid
     more expensive lookups.
     */
    CCExpressionValueType type;
    union {
        CCString atom;
        int32_t integer;
        float real;
        CCString string;
        CCOrderedCollection list;
        void *data;
    };
    
    CCExpressionState state;
    CCExpressionValueCopy copy;
    CCExpressionValueDestructor destructor;
    CCAllocatorType allocator;
} CCExpressionValue;


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
CCExpression CCExpressionCreateFromSourceFile(FSPath Path);

/*!
 * @brief Create an expression from source.
 * @param Source The string representation of the expression.
 * @return The created expression.
 */
CCExpression CCExpressionCreateFromSource(const char *Source);

/*!
 * @brief Create an expression of type.
 * @param Allocator The allocator to be used for the expression.
 * @param Type The type of the expression.
 * @return The created expression.
 */
CCExpression CCExpressionCreate(CCAllocatorType Allocator, CCExpressionValueType Type);

/*!
 * @brief Create a new copy of the expression.
 * @param Expression The expression to be copied.
 * @return The created expression.
 */
CCExpression CCExpressionCopy(CCExpression Expression);

/*!
 * @brief Destroy an expression.
 * @param Expression The expression to be destroyed.
 */
void CCExpressionDestroy(CCExpression Expression);

/*!
 * @brief Print the expression in a readable format.
 * @param Expression The expression to be printed.
 */
void CCExpressionPrint(CCExpression Expression);

/*!
 * @brief Evaluate an expression.
 * @description Evaluating an expression will cause it to mutate.
 * @param Expression The expression to be evaluated.
 * @return The resulting expression.
 */
CCExpression CCExpressionEvaluate(CCExpression Expression);

void CCExpressionCreateState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Copy);
CCExpression CCExpressionGetState(CCExpression Expression, CCString Name);
CCExpression CCExpressionSetState(CCExpression Expression, CCString Name, CCExpression Value, _Bool Copy);
void CCExpressionCopyState(CCExpression Source, CCExpression Destination);
void CCExpressionPrintState(CCExpression Expression);

//Convenience getters and initializers for when introducing tagged types.
CCExpression CCExpressionCreateAtom(CCAllocatorType Allocator, CCString Atom); //copies
CCExpression CCExpressionCreateInteger(CCAllocatorType Allocator, int32_t Value);
CCExpression CCExpressionCreateFloat(CCAllocatorType Allocator, float Value);
CCExpression CCExpressionCreateString(CCAllocatorType Allocator, CCString String); //copies
CCExpression CCExpressionCreateList(CCAllocatorType Allocator);
CCExpression CCExpressionCreateCustomType(CCAllocatorType Allocator, CCExpressionValueType Type, void *Data, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor);
void CCExpressionChangeOwnership(CCExpression Expression, CCExpressionValueCopy Copy, CCExpressionValueDestructor Destructor);

static inline _Bool CCExpressionIsTagged(CCExpression Expression);
static inline CCExpressionValueType CCExpressionGetType(CCExpression Expression);
static inline CCExpression CCExpressionGetResult(CCExpression Expression);
static inline CCString CCExpressionGetAtom(CCExpression Expression);
static inline int32_t CCExpressionGetInteger(CCExpression Expression);
static inline float CCExpressionGetFloat(CCExpression Expression);
static inline CCString CCExpressionGetString(CCExpression Expression);
static inline CCOrderedCollection CCExpressionGetList(CCExpression Expression);
static inline void *CCExpressionGetData(CCExpression Expression);


static inline _Bool CCExpressionIsTagged(CCExpression Expression)
{
    return FALSE;
}

static inline CCExpression CCExpressionGetResult(CCExpression Expression)
{
    return Expression->state.result;
}

static inline CCExpressionValueType CCExpressionGetType(CCExpression Expression)
{
    return Expression->type;
}

static inline CCString CCExpressionGetAtom(CCExpression Expression)
{
    return Expression->atom;
}

static inline int32_t CCExpressionGetInteger(CCExpression Expression)
{
    return Expression->integer;
}

static inline float CCExpressionGetFloat(CCExpression Expression)
{
    return Expression->real;
}

static inline CCString CCExpressionGetString(CCExpression Expression)
{
    return Expression->string;
}

static inline CCOrderedCollection CCExpressionGetList(CCExpression Expression)
{
    return Expression->list;
}

static inline void *CCExpressionGetData(CCExpression Expression)
{
    return Expression->data;
}

#endif
