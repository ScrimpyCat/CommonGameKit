/*
 *  Copyright (c) 2024, Stefan Johnson
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

#ifndef CommonGameKit_ExpressionMap_h
#define CommonGameKit_ExpressionMap_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/Expression.h>

typedef struct {
    /// The name of the new field.
    CCString name;
    /// The value to map to. If null the original value is used.
    CCExpression value;
} CCExpressionMapTo;

typedef struct CCExpressionMapArg CCExpressionMapArg;

/*!
 * @brief A custom expression mapper.
 * @param ArgExpr The first argument of the expression.
 * @param Enumerator The enumerator for any additional arguments.
 * @param Results The result expression.
 * @return Return TRUE if the expression was mapped, otherwise return FALSE if it was not.
 */
typedef _Bool (*CCExpressionMapCustomMapper)(CCExpression ArgExpr, CCEnumerator *Enumerator, CCOrderedCollection Results);

typedef struct {
    enum {
        CCExpressionMapMatchTypeAlways,
        CCExpressionMapMatchTypeArgs,
        CCExpressionMapMatchTypeAtom,
        CCExpressionMapMatchTypeString,
        CCExpressionMapMatchTypeInteger,
        CCExpressionMapMatchTypeEnums,
        CCExpressionMapMatchTypeValueType
    } type;
    union {
        /// A null terminated list of map args.
        const CCExpressionMapArg **args;
        CCString atom;
        CCString string;
        uintmax_t integer;
        CCDictionary(CCString, uintmax_t) enums;
        CCExpressionValueType valueType;
    };
} CCExpressionMapMatch;

struct CCExpressionMapArg {
    CCString name;
    const CCExpressionMapMatch *match;
    union {
        /// A custom mapper should be provided when match is null
        CCExpressionMapCustomMapper map;
        /// A null terminated list of map destinations.
        const CCExpressionMapTo **to;
    };
};

/*!
 * @brief Transform an expression using the provided mapping.
 * @param Expression The expression to transform.
 * @param Map The mapping to be used.
 * @return Returns the transformed expression.
 */
CCExpression CCExpressionMap(CCExpression Expression, CCDictionary Map);

#endif
