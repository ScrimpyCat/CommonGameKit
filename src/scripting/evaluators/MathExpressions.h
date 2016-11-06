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

#ifndef CommonGameKit_MathExpressions_h
#define CommonGameKit_MathExpressions_h

#include "ExpressionEvaluator.h"


CC_EXPRESSION_EVALUATOR(+) CCExpression CCMathExpressionAddition(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(-) CCExpression CCMathExpressionSubtract(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(*) CCExpression CCMathExpressionMultiply(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(/) CCExpression CCMathExpressionDivide(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(min) CCExpression CCMathExpressionMinimum(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(max) CCExpression CCMathExpressionMaximum(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(random) CCExpression CCMathExpressionRandom(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(round) CCExpression CCMathExpressionRound(CCExpression Expression);

#endif
