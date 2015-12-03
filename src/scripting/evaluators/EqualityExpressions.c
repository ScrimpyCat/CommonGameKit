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
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI == (*Expr)->integer;
                else Equal = FirstF == (float)(*Expr)->integer;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI == (*Expr)->real;
                else Equal = FirstF == (*Expr)->real;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}

CCExpression CCEqualityExpressionNotEqual(CCExpression Expression)
{
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI != (*Expr)->integer;
                else Equal = FirstF != (float)(*Expr)->integer;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI != (*Expr)->real;
                else Equal = FirstF != (*Expr)->real;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}

CCExpression CCEqualityExpressionLessThanEqual(CCExpression Expression)
{
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI <= (*Expr)->integer;
                else Equal = FirstF <= (float)(*Expr)->integer;
                
                FirstI = (*Expr)->integer;
                IsInteger = TRUE;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI <= (*Expr)->real;
                else Equal = FirstF <= (*Expr)->real;
                
                FirstF = (*Expr)->real;
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}

CCExpression CCEqualityExpressionGreaterThanEqual(CCExpression Expression)
{
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI >= (*Expr)->integer;
                else Equal = FirstF >= (float)(*Expr)->integer;
                
                FirstI = (*Expr)->integer;
                IsInteger = TRUE;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI >= (*Expr)->real;
                else Equal = FirstF >= (*Expr)->real;
                
                FirstF = (*Expr)->real;
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}

CCExpression CCEqualityExpressionLessThan(CCExpression Expression)
{
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI < (*Expr)->integer;
                else Equal = FirstF < (float)(*Expr)->integer;
                
                FirstI = (*Expr)->integer;
                IsInteger = TRUE;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI < (*Expr)->real;
                else Equal = FirstF < (*Expr)->real;
                
                FirstF = (*Expr)->real;
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}

CCExpression CCEqualityExpressionGreaterThan(CCExpression Expression)
{
    int32_t FirstI = 0;
    float FirstF = 0.0f;
    
    _Bool IsInteger = TRUE, Equal = FALSE;
    
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
                if (IsInteger) Equal = FirstI > (*Expr)->integer;
                else Equal = FirstF > (float)(*Expr)->integer;
                
                FirstI = (*Expr)->integer;
                IsInteger = TRUE;
            }
            
            else if ((*Expr)->type == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI > (*Expr)->real;
                else Equal = FirstF > (*Expr)->real;
                
                FirstF = (*Expr)->real;
                IsInteger = FALSE;
            }
            
            if (!Equal) break;
        }
    }
    
    CCExpression Expr = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeInteger);
    Expr->integer = Equal;
    
    return Expr;
}
