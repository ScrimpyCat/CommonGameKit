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

#include "IOExpressions.h"
#include <inttypes.h>

CCExpression CCIOExpressionPrint(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        switch ((*Expr)->type)
        {
            case CCExpressionValueTypeAtom:
                CC_EXPRESSION_EVALUATOR_LOG("%s", (*Expr)->atom);
                break;
                
            case CCExpressionValueTypeInteger:
                CC_EXPRESSION_EVALUATOR_LOG("%" PRId32, (*Expr)->integer);
                break;
                
            case CCExpressionValueTypeFloat:
                CC_EXPRESSION_EVALUATOR_LOG("%f", (*Expr)->real);
                break;
                
            case CCExpressionValueTypeString:
                CC_EXPRESSION_EVALUATOR_LOG("\"%s\"", (*Expr)->string);
                break;
                
            case CCExpressionValueTypeList:
                CC_EXPRESSION_EVALUATOR_LOG("[%p]", (*Expr)->list);
                break;
                
            default:
                CC_EXPRESSION_EVALUATOR_LOG("%p", (*Expr)->data);
                break;
        }
    }
    
    return Expression;
}
