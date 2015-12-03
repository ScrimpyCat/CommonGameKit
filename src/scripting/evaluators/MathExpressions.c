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
    int32_t SumI = 0;
    float SumF = 0.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        if ((*Expr)->type == CCExpressionValueTypeInteger)
        {
            SumI += (*Expr)->integer;
        }
        
        else if ((*Expr)->type == CCExpressionValueTypeFloat)
        {
            SumF += (*Expr)->real;
            IsInteger = FALSE;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = SumI;
    else Expr->real = (float)SumI + SumF;
    
    return Expr;
}

CCExpression CCMathExpressionSubtract(CCExpression Expression)
{
    int32_t FirstI = 0, SumI = 0;
    float FirstF = 0.0f, SumF = 0.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        if ((*FirstExpr)->type == CCExpressionValueTypeInteger)
        {
            FirstI = (*FirstExpr)->integer;
        }
        
        else if ((*FirstExpr)->type == CCExpressionValueTypeFloat)
        {
            FirstF = (*FirstExpr)->real;
            IsInteger = FALSE;
        }
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            if ((*Expr)->type == CCExpressionValueTypeInteger)
            {
                SumI += (*Expr)->integer;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                SumF += (*Expr)->real;
                IsInteger = FALSE;
            }
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = FirstI - SumI;
    else Expr->real = ((float)FirstI + FirstF) - ((float)SumI + SumF);
    
    return Expr;
}

CCExpression CCMathExpressionMultiply(CCExpression Expression)
{
    int32_t MulI = 1;
    float MulF = 1.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        if ((*Expr)->type == CCExpressionValueTypeInteger)
        {
            MulI *= (*Expr)->integer;
        }
        
        else if ((*Expr)->type == CCExpressionValueTypeFloat)
        {
            MulF *= (*Expr)->real;
            IsInteger = FALSE;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = MulI;
    else Expr->real = (float)MulI * MulF;
    
    return Expr;
}

CCExpression CCMathExpressionDivide(CCExpression Expression)
{
    int32_t FirstI = 0, MulI = 1;
    float FirstF = 0.0f, MulF = 1.0f;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    CCExpression *FirstExpr = CCCollectionEnumeratorNext(&Enumerator);
    if (FirstExpr)
    {
        if ((*FirstExpr)->type == CCExpressionValueTypeInteger)
        {
            FirstI = (*FirstExpr)->integer;
        }
        
        else if ((*FirstExpr)->type == CCExpressionValueTypeFloat)
        {
            FirstF = (*FirstExpr)->real;
            IsInteger = FALSE;
        }
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            if ((*Expr)->type == CCExpressionValueTypeInteger)
            {
                MulI *= (*Expr)->integer;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                MulF *= (*Expr)->real;
                IsInteger = FALSE;
            }
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = FirstI / MulI;
    else Expr->real = ((float)FirstI + FirstF) / ((float)MulI * MulF);
    
    return Expr;
}

CCExpression CCMathExpressionMinimum(CCExpression Expression)
{
    int32_t MinI = INT32_MAX;
    float MinF = INFINITY;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        if ((*Expr)->type == CCExpressionValueTypeInteger)
        {
            if ((*Expr)->integer < MinI) MinI = (*Expr)->integer;
        }
        
        else if ((*Expr)->type == CCExpressionValueTypeFloat)
        {
            if ((*Expr)->real < MinF) MinF = (*Expr)->real;
            IsInteger = FALSE;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = MinI;
    else Expr->real = (float)MinI < MinF ? (float)MinI : MinF;
    
    return Expr;
}

CCExpression CCMathExpressionMaximum(CCExpression Expression)
{
    int32_t MaxI = INT32_MIN;
    float MaxF = -INFINITY;
    
    _Bool IsInteger = TRUE;
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        if ((*Expr)->type == CCExpressionValueTypeInteger)
        {
            if ((*Expr)->integer > MaxI) MaxI = (*Expr)->integer;
        }
        
        else if ((*Expr)->type == CCExpressionValueTypeFloat)
        {
            if ((*Expr)->real > MaxF) MaxF = (*Expr)->real;
            IsInteger = FALSE;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
    if (IsInteger) Expr->integer = MaxI;
    else Expr->real = (float)MaxI > MaxF ? (float)MaxI : MaxF;
    
    return Expr;
}

CCExpression CCMathExpressionRandom(CCExpression Expression)
{    
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(Expression->list) - 1;
    
    if (!ArgCount)
    {
        Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
        Expr->integer = CCRandom();
    }
    
    else if (ArgCount <= 2)
    {
        int32_t MinI = 0, MaxI = 0;
        float MinF = 0, MaxF = 0;
        
        _Bool IsInteger = TRUE;
        
        CCExpression *Arg = CCOrderedCollectionGetElementAtIndex(Expression->list, 1);
        if ((*Arg)->type == CCExpressionValueTypeInteger)
        {
            MaxI = (*Arg)->integer;
        }
        
        else if ((*Arg)->type == CCExpressionValueTypeFloat)
        {
            MaxF = (*Arg)->real;
            IsInteger = FALSE;
        }
        
        
        if (ArgCount == 2)
        {
            MinI = MaxI;
            MinF = MaxF;
            
            MaxI = 0;
            MaxF = 0.0f;
            
            Arg = CCOrderedCollectionGetElementAtIndex(Expression->list, 2);
            if ((*Arg)->type == CCExpressionValueTypeInteger)
            {
                MaxI = (*Arg)->integer;
            }
            
            else if ((*Arg)->type == CCExpressionValueTypeFloat)
            {
                MaxF = (*Arg)->real;
                IsInteger = FALSE;
            }
        }
        
        
        Expr = CCExpressionCreate(CC_STD_ALLOCATOR, IsInteger ? CCExpressionValueTypeInteger : CCExpressionValueTypeFloat);
        if (IsInteger) Expr->integer = CCRandomRangei(MinI, MaxI);
        else Expr->real = CCRandomRangef(MinF + (float)MinI, MaxF + (float)MaxI);
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("(random) (random max:number) (random min:number max:number)");
    
    return Expr;
}
