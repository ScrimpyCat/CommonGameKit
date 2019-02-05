/*
 *  Copyright (c) 2016, Stefan Johnson
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

#ifndef CommonGameKit_StringExpressions_h
#define CommonGameKit_StringExpressions_h

#include <CommonGameKit/ExpressionEvaluator.h>


CC_EXPRESSION_EVALUATOR(prefix) CCExpression CCStringExpressionPrefix(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(suffix) CCExpression CCStringExpressionSuffix(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(filename) CCExpression CCStringExpressionFilename(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(replace) CCExpression CCStringExpressionReplace(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(cat) CCExpression CCStringExpressionConcatenate(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(length) CCExpression CCStringExpressionLength(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(insert) CCExpression CCStringExpressionInsert(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(remove) CCExpression CCStringExpressionRemove(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(chop) CCExpression CCStringExpressionChop(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(format) CCExpression CCStringExpressionFormat(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(separate) CCExpression CCStringExpressionSeparate(CCExpression Expression);

#endif
