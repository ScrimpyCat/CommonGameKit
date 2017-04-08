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


typedef struct {
    int32_t i;
    float f;
    _Bool isInteger;
} CCMathExpressionValue;

CCExpression CCMathExpressionAddition(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
        
        return Expression;
    }
    
    int32_t SumI = 0;
    float SumF = 0.0f;
    
    _Bool IsInteger = TRUE;
    CCArray Vector = NULL;
    
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
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
        {
            if (Vector)
            {
                const size_t Count = CCArrayGetCount(Vector);
                size_t Index = 0;
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                {
                    CCMathExpressionValue Value = { .i = 0, .f = 0.0f, .isInteger = TRUE };
                    if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                    {
                        Value.i = CCExpressionGetInteger(Element);
                    }
                    
                    else if (CCExpressionGetType(Element) == CCExpressionValueTypeFloat)
                    {
                        Value.f = CCExpressionGetFloat(Element);
                        Value.isInteger = FALSE;
                    }
                    
                    else
                    {
                        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                        
                        if (Vector) CCArrayDestroy(Vector);
                        
                        return Expression;
                    }
                    
                    if (Index < Count)
                    {
                        CCMathExpressionValue *Sum = CCArrayGetElementAtIndex(Vector, Index);
                        if (Value.isInteger) Sum->i += Value.i;
                        else
                        {
                            Sum->f += Value.f;
                            Sum->isInteger = FALSE;
                        }
                    }
                    
                    else CCArrayAppendElement(Vector, &Value);
                    
                    Index++;
                }
            }
            
            else
            {
                Vector = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCMathExpressionValue), 4);
                
                CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                {
                    CCMathExpressionValue Value = { .i = 0, .f = 0.0f, .isInteger = TRUE };
                    if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                    {
                        Value.i = CCExpressionGetInteger(Element);
                    }
                    
                    else if (CCExpressionGetType(Element) == CCExpressionValueTypeFloat)
                    {
                        Value.f = CCExpressionGetFloat(Element);
                        Value.isInteger = FALSE;
                    }
                    
                    else
                    {
                        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                        
                        if (Vector) CCArrayDestroy(Vector);
                        
                        return Expression;
                    }
                    
                    CCArrayAppendElement(Vector, &Value);
                }
            }
        }
        
        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
            
            if (Vector) CCArrayDestroy(Vector);
            
            return Expression;
        }
    }
    
    
    if (!Vector) return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, SumI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)SumI + SumF);
    
    
    CCExpression Vec = CCExpressionCreateList(CC_STD_ALLOCATOR);
    for (size_t Loop = 0, Count = CCArrayGetCount(Vector); Loop < Count; Loop++)
    {
        CCMathExpressionValue *Sum = CCArrayGetElementAtIndex(Vector, Loop);
        
        if (IsInteger) Sum->i += SumI;
        else
        {
            Sum->f += (float)SumI + SumF;
            Sum->isInteger = FALSE;
        }
        
        CCExpression Value = Sum->isInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, Sum->i) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)Sum->i + Sum->f);
        CCOrderedCollectionAppendElement(CCExpressionGetList(Vec), &Value);
    }
    
    CCArrayDestroy(Vector);
    
    return Vec;
}

CCExpression CCMathExpressionSubtract(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 1)
    {
        CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
        
        return Expression;
    }
    
    int32_t FirstI = 0, SumI = 0;
    float FirstF = 0.0f, SumF = 0.0f;
    CCArray FirstVector = NULL, Vector = NULL;
    
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
        
        else if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
        {
            FirstVector = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCMathExpressionValue), 4);
            
            CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
            {
                CCMathExpressionValue Value = { .i = 0, .f = 0.0f, .isInteger = TRUE };
                if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                {
                    Value.i = CCExpressionGetInteger(Element);
                }
                
                else if (CCExpressionGetType(Element) == CCExpressionValueTypeFloat)
                {
                    Value.f = CCExpressionGetFloat(Element);
                    Value.isInteger = FALSE;
                }
                
                else
                {
                    CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                    
                    if (FirstVector) CCArrayDestroy(FirstVector);
                    
                    return Expression;
                }
                
                CCArrayAppendElement(FirstVector, &Value);
            }
        }

        else
        {
            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
            
            return Expression;
        }
        
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
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
            {
                if (Vector)
                {
                    const size_t Count = CCArrayGetCount(Vector);
                    size_t Index = 0;
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                    {
                        CCMathExpressionValue Value = { .i = 0, .f = 0.0f, .isInteger = TRUE };
                        if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                        {
                            Value.i = CCExpressionGetInteger(Element);
                        }
                        
                        else if (CCExpressionGetType(Element) == CCExpressionValueTypeFloat)
                        {
                            Value.f = CCExpressionGetFloat(Element);
                            Value.isInteger = FALSE;
                        }
                        
                        else
                        {
                            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                            
                            if (Vector) CCArrayDestroy(Vector);
                            if (FirstVector) CCArrayDestroy(FirstVector);
                            
                            return Expression;
                        }
                        
                        if (Index < Count)
                        {
                            CCMathExpressionValue *Sum = CCArrayGetElementAtIndex(Vector, Index);
                            if (Value.isInteger) Sum->i += Value.i;
                            else
                            {
                                Sum->f += Value.f;
                                Sum->isInteger = FALSE;
                            }
                        }
                        
                        else CCArrayAppendElement(Vector, &Value);
                        
                        Index++;
                    }
                }
                
                else
                {
                    Vector = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCMathExpressionValue), 4);
                    
                    CC_COLLECTION_FOREACH(CCExpression, Element, CCExpressionGetList(Result))
                    {
                        CCMathExpressionValue Value = { .i = 0, .f = 0.0f, .isInteger = TRUE };
                        if (CCExpressionGetType(Element) == CCExpressionValueTypeInteger)
                        {
                            Value.i = CCExpressionGetInteger(Element);
                        }
                        
                        else if (CCExpressionGetType(Element) == CCExpressionValueTypeFloat)
                        {
                            Value.f = CCExpressionGetFloat(Element);
                            Value.isInteger = FALSE;
                        }
                        
                        else
                        {
                            CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                            CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                            
                            if (Vector) CCArrayDestroy(Vector);
                            if (FirstVector) CCArrayDestroy(FirstVector);
                            
                            return Expression;
                        }
                        
                        CCArrayAppendElement(Vector, &Value);
                    }
                }
            }
            
            else
            {
                CCString Function = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0));
                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Incorrect usage of %S: (%S %s)", Function, Function, "_:number:list");
                
                if (Vector) CCArrayDestroy(Vector);
                if (FirstVector) CCArrayDestroy(FirstVector);
                
                return Expression;
            }
        }
    }
    
    
    if (!FirstVector)
    {
        if (Vector)
        {
            CCExpression Vec = CCExpressionCreateList(CC_STD_ALLOCATOR);
            for (size_t Loop = 0, Count = CCArrayGetCount(Vector); Loop < Count; Loop++)
            {
                CCMathExpressionValue *Sum = CCArrayGetElementAtIndex(Vector, Loop);
                
                if (IsInteger) Sum->i += SumI;
                else
                {
                    Sum->f += (float)SumI + SumF;
                    Sum->isInteger = FALSE;
                }
                
                CCExpression Value = Sum->isInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, FirstI - Sum->i) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((float)FirstI + FirstF) - ((float)Sum->i + Sum->f));
                CCOrderedCollectionAppendElement(CCExpressionGetList(Vec), &Value);
            }
            
            CCArrayDestroy(Vector);
            
            return Vec;
        }
        
        return IsInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, FirstI - SumI) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, ((float)FirstI + FirstF) - ((float)SumI + SumF));
    }
    
    
    const size_t CountA = CCArrayGetCount(FirstVector), CountB = Vector ? CCArrayGetCount(Vector) : 0;
    CCExpression Vec = CCExpressionCreateList(CC_STD_ALLOCATOR);
    for (size_t Loop = 0, Count = CountA > CountB ? CountA : CountB; Loop < Count; Loop++)
    {
        CCMathExpressionValue Sum = { .i = SumI, .f = SumF, .isInteger = IsInteger };
        
        if (Loop < CountB)
        {
            CCMathExpressionValue *SumB = CCArrayGetElementAtIndex(Vector, Loop);
            Sum.i += SumB->i;
            Sum.f += SumB->f;
            Sum.isInteger &= SumB->isInteger;
        }
        
        if (Loop < CountA)
        {
            CCMathExpressionValue *SumA = CCArrayGetElementAtIndex(FirstVector, Loop);
            
            if ((Sum.isInteger) && (SumA->isInteger))
            {
                Sum.i = SumA->i - Sum.i;
            }
            
            else
            {
                Sum.f = ((float)SumA->i + SumA->f) - ((float)Sum.i + Sum.f);
                Sum.isInteger = FALSE;
            }
            
            CCExpression Value = Sum.isInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, Sum.i) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, Sum.f);
            CCOrderedCollectionAppendElement(CCExpressionGetList(Vec), &Value);
        }
        
        else
        {
            CCExpression Value = Sum.isInteger ? CCExpressionCreateInteger(CC_STD_ALLOCATOR, -Sum.i) : CCExpressionCreateFloat(CC_STD_ALLOCATOR, -((float)Sum.i + Sum.f));
            CCOrderedCollectionAppendElement(CCExpressionGetList(Vec), &Value);
        }
    }
    
    if (Vector) CCArrayDestroy(Vector);
    CCArrayDestroy(FirstVector);
    
    return Vec;
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
