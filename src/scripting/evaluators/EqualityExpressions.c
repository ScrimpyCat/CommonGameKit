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

#include "EqualityExpressions.h"

CCExpression CCEqualityExpressionEqual(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    _Bool Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        CCExpressionValueType Type = CCExpressionGetType(Result);
        switch (Type)
        {
            case CCExpressionValueTypeInteger:
            {
                int32_t FirstI = CCExpressionGetInteger(Result);
                for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
                {
                    Equal = FALSE;
                    
                    Result = CCExpressionEvaluate(*Expr);
                    if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
                    {
                        Equal = FirstI == CCExpressionGetInteger(Result);
                    }
                    
                    else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
                    {
                        Equal = (float)FirstI == CCExpressionGetFloat(Result);
                    }
                    
                    else
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("=", "_:number");
                        return Expression;
                    }
                    
                    if (!Equal) break;
                }
                break;
            }
                
            case CCExpressionValueTypeFloat:
            {
                float FirstF = CCExpressionGetFloat(Result);
                for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
                {
                    Equal = FALSE;
                    
                    Result = CCExpressionEvaluate(*Expr);
                    if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
                    {
                        Equal = FirstF == (float)CCExpressionGetInteger(Result);
                    }
                    
                    else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
                    {
                        Equal = FirstF == CCExpressionGetFloat(Result);
                    }
                    
                    else
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("=", "_:number");
                        return Expression;
                    }
                    
                    if (!Equal) break;
                }
                break;
            }
            
            case CCExpressionValueTypeString:
            case CCExpressionValueTypeAtom:
            {
                CCString FirstS = (Type == CCExpressionValueTypeString ? CCExpressionGetString : CCExpressionGetAtom)(Result);
                for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
                {
                    Equal = FALSE;
                    
                    Result = CCExpressionEvaluate(*Expr);
                    if ((CCExpressionGetType(Result) == CCExpressionValueTypeString) || (CCExpressionGetType(Result) == CCExpressionValueTypeAtom))
                    {
                        Equal = CCStringEqual(FirstS, (Type == CCExpressionValueTypeString ? CCExpressionGetString : CCExpressionGetAtom)(Result));
                    }
                    
                    else
                    {
                        CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("=", "_:string/atom");
                        return Expression;
                    }
                    
                    if (!Equal) break;
                }
                break;
            }
                
            default:
                return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionNotEqual(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            FirstI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            FirstF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Equal = FALSE;
            
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI != CCExpressionGetInteger(Result);
                else Equal = FirstF != (float)CCExpressionGetInteger(Result);
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI != CCExpressionGetFloat(Result);
                else Equal = FirstF != CCExpressionGetFloat(Result);
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionLessThanEqual(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            FirstI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            FirstF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Equal = FALSE;
            
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI <= CCExpressionGetInteger(Result);
                else Equal = FirstF <= (float)CCExpressionGetInteger(Result);
                
                FirstI = CCExpressionGetInteger(Result);
                IsInteger = TRUE;
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI <= CCExpressionGetFloat(Result);
                else Equal = FirstF <= CCExpressionGetFloat(Result);
                
                FirstF = CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionGreaterThanEqual(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            FirstI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            FirstF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Equal = FALSE;
            
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI >= CCExpressionGetInteger(Result);
                else Equal = FirstF >= (float)CCExpressionGetInteger(Result);
                
                FirstI = CCExpressionGetInteger(Result);
                IsInteger = TRUE;
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI >= CCExpressionGetFloat(Result);
                else Equal = FirstF >= CCExpressionGetFloat(Result);
                
                FirstF = CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionLessThan(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            FirstI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            FirstF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Equal = FALSE;
            
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI < CCExpressionGetInteger(Result);
                else Equal = FirstF < (float)CCExpressionGetInteger(Result);
                
                FirstI = CCExpressionGetInteger(Result);
                IsInteger = TRUE;
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI < CCExpressionGetFloat(Result);
                else Equal = FirstF < CCExpressionGetFloat(Result);
                
                FirstF = CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionGreaterThan(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        CCExpression Result = CCExpressionEvaluate(*FirstExpr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            FirstI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            FirstF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else return CCExpressionCreateInteger(CC_STD_ALLOCATOR, FALSE);
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Equal = FALSE;
            
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI > CCExpressionGetInteger(Result);
                else Equal = FirstF > (float)CCExpressionGetInteger(Result);
                
                FirstI = CCExpressionGetInteger(Result);
                IsInteger = TRUE;
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI > CCExpressionGetFloat(Result);
                else Equal = FirstF > CCExpressionGetFloat(Result);
                
                FirstF = CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionNot(CCExpression Expression)
{
    CCExpression Expr = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            return CCExpressionCreateInteger(CC_STD_ALLOCATOR, !CCExpressionGetInteger(Result));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("not", "value:integer");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("not", "value:integer");
    
    return Expr;
}
