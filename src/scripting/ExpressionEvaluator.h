//
//  ExpressionEvaluator.h
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_ExpressionEvaluator_h
#define Blob_Game_ExpressionEvaluator_h

#include "Expression.h"

/*!
 * @define CC_EXPRESSION_EVALUATOR
 * @abstract Marks a function as an expression evaluator.
 * @param va_arg The name of the evaluator.
 */
#define CC_EXPRESSION_EVALUATOR(...)

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
void CCExpressionEvaluatorRegister(const char *Name, CCExpressionEvaluator Evaluator);

/*!
 * @brief Get the expression evaluator for an atom.
 * @param Name The atom name of the expression function.
 * @return The evaluator function.
 */
CCExpressionEvaluator CCExpressionEvaluatorForName(const char *Name);

#endif
