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

#ifndef CommonGameKit_GraphicsExpressions_h
#define CommonGameKit_GraphicsExpressions_h

#include <CommonGameKit/ExpressionEvaluator.h>
#include <CommonGameKit/GFX.h>
#include <CommonGameKit/Text.h>


typedef enum {
    CCGraphicsExpressionValueTypeDraw = 'drw\0',
    CCGraphicsExpressionValueTypeText = 'txt\0'
} CCGraphicsExpressionValueType;

typedef struct {
    GFXDraw drawer;
    size_t vertices;
} CCGraphicsExpressionValueDraw;


/*!
 * @brief Get the text options from the expression.
 * @param Enumerator The pointer to the current expression enumerator.
 * @param AttributedStrings An ordered collection to store the @b CCTextAttribute or
 *        NULL if no attributed strings are needed.
 *
 * @param Offset A pointer to where the offset should be store, or NULL if this should
 *        be ignored.
 *
 * @param Length A pointer to where the length should be store, or NULL if this should
 *        be ignored.
 *
 * @param Alignment A pointer to where the alignment should be store, or NULL if this
 *        should be ignored.
 *
 * @param Visibility A pointer to where the visibility should be store, or NULL if this
 *        should be ignored.
 */
void CCGraphicsExpressionGetTextOptions(CCEnumerator *Enumerator, CCOrderedCollection AttributedStrings, size_t *Offset, size_t *Length, CCTextAlignment *Alignment, CCTextVisibility *Visibility);

CC_EXPRESSION_EVALUATOR(render-rect) CCExpression CCGraphicsExpressionRenderRect(CCExpression Expression);
CC_EXPRESSION_EVALUATOR(render-text) CCExpression CCGraphicsExpressionRenderText(CCExpression Expression);

#endif
