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

#ifndef CommonGameKit_ECSExpression_h
#define CommonGameKit_ECSExpression_h

#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/ECSContext.h>


/*!
 * @brief Set the component expression names.
 * @warning This must be set prior to calling @b ECSExpressionRegisterComponentEvaluators.
 */
extern const CCString *ECSExpressionComponentExpressionNames;

/*!
 * @brief Set the component expression maps.
 * @warning This must be set prior to evaluating expressions.
 */
extern const CCDictionary *ECSExpressionComponentExpressionMaps;

/*!
 * @brief The entity relationship options.
 */
extern CCDictionary ECSExpressionEntityRelationships;

/*!
 * @brief The ecs contexts.
 */
extern CCDictionary ECSExpressionECSContexts;

/*!
 * @brief A predefined relationship option.
 * @description By default this is set to the @b ECSParentChildRelationships link.
 */
extern const CCString ECSExpressionEntityChildrenRelationshipOption;


void ECSExpressionRegisterComponentEvaluators(void);

CC_EXPRESSION_EVALUATOR(component) CCExpression ECSExpressionComponent(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity) CCExpression ECSExpressionEntity(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(ecs) CCExpression ECSExpressionECS(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity-register) CCExpression ECSExpressionEntityRegister(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity-unregister) CCExpression ECSExpressionEntityUnregister(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity-lookup) CCExpression ECSExpressionEntityLookup(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity-link) CCExpression ECSExpressionEntityLink(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(entity-unlink) CCExpression ECSExpressionEntityUnlink(CCExpression Expression);

#endif
