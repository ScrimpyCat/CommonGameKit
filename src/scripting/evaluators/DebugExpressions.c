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

#include "DebugExpressions.h"
#include <inttypes.h>

CCExpression CCDebugExpressionInspect(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) != 2)
    {
        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("inspect", "value:expr");
        return Expression;
    }
    
    CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    CCExpressionStateSetSuper(Expr, CCExpressionStateGetSuper(Expression));
    
    CCExpression Arg = CCExpressionEvaluate(Expr);
    if (Arg)
    {
        switch (CCExpressionGetType(Arg))
        {
            case CCExpressionValueTypeAtom:
                CC_EXPRESSION_EVALUATOR_LOG("%S", CCExpressionGetAtom(Arg));
                break;
                
            case CCExpressionValueTypeInteger:
                CC_EXPRESSION_EVALUATOR_LOG("%" PRId32, CCExpressionGetInteger(Arg));
                break;
                
            case CCExpressionValueTypeFloat:
                CC_EXPRESSION_EVALUATOR_LOG("%f", CCExpressionGetFloat(Arg));
                break;
                
            case CCExpressionValueTypeString:
                CC_EXPRESSION_EVALUATOR_LOG("\"%S\"", CCExpressionGetString(Arg));
                break;
                
            case CCExpressionValueTypeList:
                CC_EXPRESSION_EVALUATOR_LOG("[%p]", CCExpressionGetList(Arg));
                CCExpressionPrint(Arg);
                break;
                
            default:
                CC_EXPRESSION_EVALUATOR_LOG("(%d):%p", CCExpressionGetType(Arg), CCExpressionGetData(Arg));
                break;
        }
        
        Arg = CCExpressionRetain(Arg);
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG("(null)");
    
    return Arg;
}

CCExpression CCDebugExpressionBreak(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) != 2)
    {
        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("break", "expression:expr");
        return Expression;
    }
    
    CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    CCExpressionStateSetSuper(Expr, CCExpressionStateGetSuper(Expression));
    
#if CC_HARDWARE_ARCH_X86 || CC_HARDWARE_ARCH_X86_64
    asm("int $3");
#else
#warning Missing breakpoint feature
#endif
    
    CCExpression Result = CCExpressionEvaluate(Expr);
    
    return Result ? CCExpressionRetain(Result) : Result;
}
