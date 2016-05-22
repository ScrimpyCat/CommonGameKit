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

#ifndef Blob_Game_ExpressionEvaluator_h
#define Blob_Game_ExpressionEvaluator_h

#include "Expression.h"

/*!
 * @define CC_EXPRESSION_EVALUATOR
 * @abstract Marks a function as an expression evaluator.
 * @param va_arg The name of the evaluator.
 */
#define CC_EXPRESSION_EVALUATOR(...)

#define CC_EXPRESSION_EVALUATOR_LOG(...) CC_LOG_CUSTOM("SCRIPT", __VA_ARGS__)

#define CC_EXPRESSION_EVALUATOR_LOG_ERROR(...) CC_LOG_CUSTOM("SCRIPT_ERROR", __VA_ARGS__)

#define CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(function, args) CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %s: (%s %s)", function, function, args)

#define CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR(option, args) CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %s: (%s: %s)", option, option, args)

/*!
 * @brief An evaluator for an expression.
 * @param Expression The expression to be evaluated.
 * @return The resulting expression.
 */
typedef CCExpression (*CCExpressionEvaluator)(CCExpression Expression);

/*!
 * @brief Register an expression evaluator.
 * @param Name The atom name of the expression function.
 * @param Evaluator The evaluator function.
 */
void CCExpressionEvaluatorRegister(CCString Name, CCExpressionEvaluator Evaluator);

/*!
 * @brief Get the expression evaluator for an atom.
 * @param Name The atom name of the expression function.
 * @return The evaluator function.
 */
CCExpressionEvaluator CCExpressionEvaluatorForName(CCString Name);

#endif
