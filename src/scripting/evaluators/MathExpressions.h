//
//  MathExpressions.h
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_MathExpressions_h
#define Blob_Game_MathExpressions_h

#include "ExpressionEvaluator.h"


static CC_EXPRESSION_EVALUATOR(+) CCExpression CCMathExpressionAddition(CCExpression Expression)
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
    if (IsInteger) Expr->integer = SumI + (int32_t)SumF;
    else Expr->real = (float)SumI + SumF;
    
    return Expr;
}

static CC_EXPRESSION_EVALUATOR(-) CCExpression CCMathExpressionSubtract(CCExpression Expression)
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
    if (IsInteger) Expr->integer = (FirstI + (int32_t)FirstF) - (SumI + (int32_t)SumF);
    else Expr->real = ((float)FirstI + FirstF) - ((float)SumI + SumF);
    
    return Expr;
}

static CC_EXPRESSION_EVALUATOR(*) CCExpression CCMathExpressionMultiply(CCExpression Expression)
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
    if (IsInteger) Expr->integer = MulI * (int32_t)MulF;
        else Expr->real = (float)MulI * MulF;
            
            return Expr;
}

static CC_EXPRESSION_EVALUATOR(/) CCExpression CCMathExpressionDivide(CCExpression Expression)
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
    if (IsInteger) Expr->integer = (FirstI + (int32_t)FirstF) / (MulI * (int32_t)MulF);
    else Expr->real = ((float)FirstI + FirstF) / ((float)MulI * MulF);
            
    return Expr;
}

#endif
