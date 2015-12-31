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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeInteger)
            {
                if (IsInteger) Equal = FirstI == CCExpressionGetInteger(Result);
                else Equal = FirstF == (float)CCExpressionGetInteger(Result);
            }
            
            else if (CCExpressionGetType(Result) == CCExpressionValueTypeFloat)
            {
                if (IsInteger) Equal = (float)FirstI == CCExpressionGetFloat(Result);
                else Equal = FirstF == CCExpressionGetFloat(Result);
            }
            
            if (!Equal) break;
        }
    }
    
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, Equal);
}

CCExpression CCEqualityExpressionNotEqual(CCExpression Expression)
{
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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
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
        
        for (CCExpression *Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
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
