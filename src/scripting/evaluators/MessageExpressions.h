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

#ifndef CommonGameKit_MessageExpressions_h
#define CommonGameKit_MessageExpressions_h

#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/Message.h>

typedef enum {
    CCMessageExpressionValueTypeRouter = 'rout'
} CCMessageExpressionValueType;

/*!
 * @brief Send the message.
 * @param Router The message router to be used.
 * @param id The type of message.
 * @param Args The optional arguments passed with the message. May be NULL.
 * @return Whether the message was successfully posted (TRUE) or not (FALSE).
 */
typedef _Bool (*CCMessageExpressionPoster)(CCMessageRouter *Router, CCMessageID id, CCExpression Args);

typedef struct {
    CCComponentID id;
    CCMessageExpressionPoster post;
} CCMessageExpressionDescriptor;

/*!
 * @brief Register a message poster.
 * @param Name The name of the message expression.
 * @param Descriptor The descriptor to workout how to handle delivery of the message.
 */
void CCMessageExpressionRegister(CCString CC_COPY(Name), const CCMessageExpressionDescriptor *Descriptor);

CC_EXPRESSION_EVALUATOR(component-router) CCExpression CCMessageExpressionComponentRouter(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(message) CCExpression CCMessageExpressionMessage(CCExpression Expression);

#endif
