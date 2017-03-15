/*
 *  Copyright (c) 2017, Stefan Johnson
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

#ifndef CommonGameKit_ComponentExpressions_h
#define CommonGameKit_ComponentExpressions_h

#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/Component.h>


typedef enum {
    CCComponentExpressionValueTypeComponent = 'cmpt'
} CCComponentExpressionValueType;

/*!
 * @brief Evaluate a component argument.
 * @description The deserializer is responsible for evaluating any arguments of the parent
 *              component as well.
 *
 * @param Component The component whose state is to be set.
 * @param Arg The expression of the argument, which takes the form of an option.
 */
typedef void (*CCComponentExpressionDeserializer)(CCComponent Component, CCExpression Arg);

/*!
 * @brief Convert a component into an expression.
 * @param Allocator The allocator to be used.
 * @param Component The component to be converted into an expression.
 * @return The expression.
 */
typedef CC_NEW CCExpression (*CCComponentExpressionSerializer)(CCAllocatorType Allocator, CCComponent Component);

typedef struct {
    CCComponentID id;
    CCComponentExpressionDeserializer deserialize;
    CCComponentExpressionSerializer serialize;
} CCComponentExpressionDescriptor;

/*!
 * @brief Register component evaluators.
 * @param Name The name of the component expression.
 * @param Descriptor The descriptor to workout how to create the component from the expression.
 * @param Wrapper Whether or not a wrapper function should be created.
 */
void CCComponentExpressionRegister(CCString CC_COPY(Name), const CCComponentExpressionDescriptor *Descriptor, _Bool Wrapper);

/*!
 * @brief Create a component expression for a given component.
 * @param Allocator The allocator to be used.
 * @param Component The component to be converted into an expression.
 * @return The expression that will evaluate into an equivalent component.
 */
CCExpression CCComponentExpressionCreate(CCAllocatorType Allocator, CCComponent Component);

CC_EXPRESSION_EVALUATOR(component) CCExpression CCComponentExpressionComponent(CCExpression Expression);

#endif
