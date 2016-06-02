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

#include "MathExpressions.h"


CCExpression CCMathExpressionAddition(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t SumI = 0;
    float SumF = 0.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            SumI += CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            SumF += CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, SumI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)SumI + SumF);
}

CCExpression CCMathExpressionSubtract(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0, SumI = 0;
    float FirstF = 0.0f, SumF = 0.0f;
    
    _Bool IsInteger = TRUE;
    
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
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                SumI += CCExpressionGetInteger(Result);
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                SumF += CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            else
            {
                CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
                
                return Expression;
            }
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, FirstI - SumI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((float)FirstI + FirstF) - ((float)SumI + SumF));
}

CCExpression CCMathExpressionMultiply(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t MulI = 1;
    float MulF = 1.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            MulI *= CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            MulF *= CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, MulI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)MulI * MulF);
}

CCExpression CCMathExpressionDivide(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t FirstI = 0, MulI = 1;
    float FirstF = 0.0f, MulF = 1.0f;
    
    _Bool IsInteger = TRUE;
    
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
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                MulI *= CCExpressionGetInteger(Result);
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                MulF *= CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            else
            {
                CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
                
                return Expression;
            }
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, FirstI / MulI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((float)FirstI + FirstF) / ((float)MulI * MulF));
}

CCExpression CCMathExpressionMinimum(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t MinI = INT32_MAX;
    float MinF = INFINITY;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            if (CCExpressionGetInteger(Result) < MinI) MinI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            if (CCExpressionGetFloat(Result) < MinF) MinF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, MinI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)MinI < MinF ? (float)MinI : MinF);
}

CCExpression CCMathExpressionMaximum(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
        
        return Expression;
    }
    
    int32_t MaxI = INT32_MIN;
    float MaxF = -INFINITY;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            if (CCExpressionGetInteger(Result) > MaxI) MaxI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            if (CCExpressionGetFloat(Result) > MaxF) MaxF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number");
            
            return Expression;
        }
    }
    
    return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, MaxI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)MaxI > MaxF ? (float)MaxI : MaxF);
}

CCExpression CCMathExpressionRandom(CCExpression Expression)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if (!ArgCount)
    {
        Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCRandom());
    }
    
    else if (ArgCount <= 2)
    {
        int32_t MinI = 0, MaxI = 0;
        float MinF = 0, MaxF = 0;
        
        _Bool IsInteger = TRUE;
        
        CCExpression Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
        {
            MaxI = CCExpressionGetInteger(Result);
        }
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
        {
            MaxF = CCExpressionGetFloat(Result);
            IsInteger = FALSE;
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, ArgCount == 2 ? "min:number max:number" : "max:number");
            
            return Expression;
        }
        
        
        if (ArgCount == 2)
        {
            MinI = MaxI;
            MinF = MaxF;
            
            MaxI = 0;
            MaxF = 0.0f;
            
            Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                MaxI = CCExpressionGetInteger(Result);
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                MaxF = CCExpressionGetFloat(Result);
                IsInteger = FALSE;
            }
            
            else
            {
                CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "min:number max:number");
                
                return Expression;
            }
        }
        
        
        Expr = IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, CCRandomRangei(MinI, MaxI)) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, CCRandomRangef(MinF + (float)MinI, MaxF + (float)MaxI));
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("(random) (random max:number) (random min:number max:number)");
    
    return Expr;
}

CCExpression CCMathExpressionRound(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Arg = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeInteger)
        {
            return Arg;
        }
        
        else if (CCExpressionGetType(Arg) == CCExpressionValueTypeFloat)
        {
            return CCExpressionCreateFloat(CC_STD_ALLOCATOR, roundf(CCExpressionGetFloat(Arg)));
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("round", "value:number");
    
    return Expression;
}
