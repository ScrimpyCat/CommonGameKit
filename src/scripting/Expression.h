//
//  Expression.h
//  Blob Game
//
//  Created by Stefan Johnson on 21/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

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
    
    CCExpressionValueTypeReservedCount = 20
} CCExpressionValueType;

/*!
 * @brief An expression.
 */
typedef struct CCExpressionValue *CCExpression;

/*!
 * @brief The expression value destructor.
 * @description Perform all destruction for your expression value.
 * @param Data The data held by the expression value.
 */
typedef void (*CCExpressionValueDestructor)(void *Data);

typedef struct CCExpressionValue {
    CCExpressionValueType type;
    union {
        char *atom;
        int32_t integer;
        float real;
        char *string;
        CCOrderedCollection list;
        void *data;
    };
    
    CCExpressionValueDestructor destructor;
} CCExpressionValue;


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

#endif
