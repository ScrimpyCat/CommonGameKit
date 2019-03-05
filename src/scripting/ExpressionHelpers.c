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

#include "ExpressionHelpers.h"
#include "ExpressionEvaluator.h"


CCExpression CCExpressionCreateVector2(CCAllocatorType Allocator, CCVector2D v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.y) });
    
    return Expr;
}

CCExpression CCExpressionCreateVector3(CCAllocatorType Allocator, CCVector3D v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.z) });
    
    return Expr;
}

CCExpression CCExpressionCreateVector4(CCAllocatorType Allocator, CCVector4D v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.z) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, v.w) });
    
    return Expr;
}

CCExpression CCExpressionCreateVector2i(CCAllocatorType Allocator, CCVector2Di v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.y) });
    
    return Expr;
}

CCExpression CCExpressionCreateVector3i(CCAllocatorType Allocator, CCVector3Di v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.z) });
    
    return Expr;
}

CCExpression CCExpressionCreateVector4i(CCAllocatorType Allocator, CCVector4Di v)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.z) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateInteger(Allocator, v.w) });
    
    return Expr;
}

CCExpression CCExpressionCreateRect(CCAllocatorType Allocator, CCRect r)
{
    CCExpression Expr = CCExpressionCreateList(Allocator);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, r.position.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, r.position.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, r.size.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(Allocator, r.size.y) });
    
    return Expr;
}

static _Bool CCExpressionGetFloatMinArray(CCExpression Vec, float *Values, float Factor, size_t Start, size_t Min, size_t Max, size_t *Count, const char *ErrMsg, _Bool NestedList)
{
    if (CCExpressionGetType(Vec) == CCExpressionValueTypeList)
    {
        const size_t ItemCount = CCCollectionGetCount(CCExpressionGetList(Vec)) - Start;
        if ((NestedList) && (ItemCount == 1))
        {
            CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vec), Start);
            if (CCExpressionGetType(Value) == CCExpressionValueTypeList) return CCExpressionGetFloatMinArray(Value, Values, Factor, 0, Min, Max, Count, ErrMsg, FALSE);
        }
        
        if ((ItemCount >= Min) && (ItemCount <= Max))
        {
            for (size_t Loop = 0; Loop < ItemCount; Loop++)
            {
                CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vec), Loop + Start);
                
                if (CCExpressionGetType(Value) == CCExpressionValueTypeInteger)
                {
                    Values[Loop] = Factor * (float)CCExpressionGetInteger(Value);
                }
                
                else if (CCExpressionGetType(Value) == CCExpressionValueTypeFloat)
                {
                    Values[Loop] = CCExpressionGetFloat(Value);
                }
                
                else
                {
                    if (ErrMsg) CC_EXPRESSION_EVALUATOR_LOG_ERROR(ErrMsg);
                    memset(Values, 0, Max);
                    *Count = Max;
                    
                    return FALSE;
                }
            }
            
            *Count = ItemCount;
            
            return TRUE;
        }
    }
    
    if (ErrMsg) CC_EXPRESSION_EVALUATOR_LOG_ERROR(ErrMsg);
    memset(Values, 0, Max);
    *Count = Max;
    
    return FALSE;
}

static _Bool CCExpressionGetIntegerMinArray(CCExpression Vec, int32_t *Values, float Factor, size_t Start, size_t Min, size_t Max, size_t *Count, const char *ErrMsg, _Bool NestedList)
{
    if (CCExpressionGetType(Vec) == CCExpressionValueTypeList)
    {
        const size_t ItemCount = CCCollectionGetCount(CCExpressionGetList(Vec)) - Start;
        if ((NestedList) && (ItemCount == 1))
        {
            CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vec), Start);
            if (CCExpressionGetType(Value) == CCExpressionValueTypeList) return CCExpressionGetIntegerMinArray(Value, Values, Factor, 0, Min, Max, Count, ErrMsg, FALSE);
        }
        
        if ((ItemCount >= Min) && (ItemCount <= Max))
        {
            for (size_t Loop = 0; Loop < ItemCount; Loop++)
            {
                CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vec), Loop + Start);
                
                if (CCExpressionGetType(Value) == CCExpressionValueTypeInteger)
                {
                    Values[Loop] = CCExpressionGetInteger(Value);
                }
                
                else if (CCExpressionGetType(Value) == CCExpressionValueTypeFloat)
                {
                    Values[Loop] = (int32_t)(Factor * CCExpressionGetFloat(Value));
                }
                
                else
                {
                    if (ErrMsg) CC_EXPRESSION_EVALUATOR_LOG_ERROR(ErrMsg);
                    memset(Values, 0, Max);
                    *Count = Max;
                    
                    return FALSE;
                }
            }
            
            *Count = ItemCount;
            
            return TRUE;
        }
    }
    
    if (ErrMsg) CC_EXPRESSION_EVALUATOR_LOG_ERROR(ErrMsg);
    memset(Values, 0, Max);
    *Count = Max;
    
    return FALSE;
}

static _Bool CCExpressionGetFloatArray(CCExpression Vec, float *Values, size_t Start, size_t Count, const char *ErrMsg, _Bool NestedList)
{
    return CCExpressionGetFloatMinArray(Vec, Values, 1.0f, Start, Count, Count, &Count, ErrMsg, NestedList);
}

static _Bool CCExpressionGetIntegerArray(CCExpression Vec, int32_t *Values, size_t Start, size_t Count, const char *ErrMsg, _Bool NestedList)
{
    return CCExpressionGetIntegerMinArray(Vec, Values, 1.0f, Start, Count, Count, &Count, ErrMsg, NestedList);
}

static _Bool CCExpressionGetColourArray(CCExpression Colour, float *Values, size_t Start, size_t *Count, const char *ErrMsg, _Bool NestedList)
{
    return CCExpressionGetFloatMinArray(Colour, Values, 0.003921568627f, Start, 3, 4, Count, ErrMsg, NestedList);
}

_Bool CCExpressionIsVector2(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector2D){}.v, 0, 2, NULL, FALSE);
}

CCVector2D CCExpressionGetVector2(CCExpression Vec)
{
    CCVector2D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 0, 2, "Vector2D should evaluate to a list of 2 numbers. (x:number y:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsVector3(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector3D){}.v, 0, 3, NULL, FALSE);
}

CCVector3D CCExpressionGetVector3(CCExpression Vec)
{
    CCVector3D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 0, 3, "Vector3D should evaluate to a list of 3 numbers. (x:number y:number z:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsVector4(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector4D){}.v, 0, 4, NULL, FALSE);
}

CCVector4D CCExpressionGetVector4(CCExpression Vec)
{
    CCVector4D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 0, 4, "Vector4D should evaluate to a list of 4 numbers. (x:number y:number z:number w:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsVector2i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector2Di){}.v, 0, 2, NULL, FALSE);
}

CCVector2Di CCExpressionGetVector2i(CCExpression Vec)
{
    CCVector2Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 0, 2, "Vector2Di should evaluate to a list of 2 numbers. (x:number y:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsVector3i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector3Di){}.v, 0, 3, NULL, FALSE);
}

CCVector3Di CCExpressionGetVector3i(CCExpression Vec)
{
    CCVector3Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 0, 3, "Vector3Di should evaluate to a list of 3 numbers. (x:number y:number z:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsVector4i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector4Di){}.v, 0, 4, NULL, FALSE);
}

CCVector4Di CCExpressionGetVector4i(CCExpression Vec)
{
    CCVector4Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 0, 4, "Vector4Di should evaluate to a list of 4 numbers. (x:number y:number z:number w:number)", FALSE);
    
    return Result;
}

_Bool CCExpressionIsRect(CCExpression Rect)
{
    return CCExpressionIsVector4(Rect);
}

CCRect CCExpressionGetRect(CCExpression Rect)
{
    CCVector4D Result;
    CCExpressionGetFloatArray(Rect, Result.v, 0, 4, "Rect should evaluate to a list of 4 numbers. (x:number y:number width:number height:number)", FALSE);
    
    return (CCRect){ Result.v[0], Result.v[1], Result.v[2], Result.v[3] };
}

_Bool CCExpressionIsColour(CCExpression Colour)
{
    size_t Count;
    return CCExpressionGetColourArray(Colour, (CCVector4D){}.v, 0, &Count, NULL, FALSE);
}

CCColourRGBA CCExpressionGetColour(CCExpression Colour)
{
    size_t Count;
    CCVector4D Result;
    CCExpressionGetColourArray(Colour, Result.v, 0, &Count, "Colour should evaluate to a list of 3 or 4 numbers. (r:number g:number b:number [alpha:number])", FALSE);
    
    return (CCColourRGBA){ Result.v[0], Result.v[1], Result.v[2], Count == 4 ? Result.v[3] : 1.0f };
}

CCString CCExpressionGetNamedString(CCExpression String)
{
    if ((CCExpressionGetType(String) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(String)) == 2))
    {
        CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(String), 1);
        
        if (CCExpressionGetType(Value) == CCExpressionValueTypeString)
        {
            return CCExpressionGetString(Value);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_ERROR("String should evaluate to a string.");
    
    return (CCString)NULL;
}

int32_t CCExpressionGetNamedInteger(CCExpression Value)
{
    int32_t Result;
    CCExpressionGetIntegerArray(Value, &Result, 1, 1, "Integer should evaluate to a number.", FALSE);
    
    return Result;
}

float CCExpressionGetNamedFloat(CCExpression Value)
{
    float Result;
    CCExpressionGetFloatArray(Value, &Result, 1, 1, "Float should evaluate to a number.", FALSE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector2(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector2D){}.v, 1, 2, NULL, TRUE);
}

CCVector2D CCExpressionGetNamedVector2(CCExpression Vec)
{
    CCVector2D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 1, 2, "Vector2D should evaluate to a list of 2 numbers. (x:number y:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector3(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector3D){}.v, 1, 3, NULL, TRUE);
}

CCVector3D CCExpressionGetNamedVector3(CCExpression Vec)
{
    CCVector3D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 1, 3, "Vector3D should evaluate to a list of 3 numbers. (x:number y:number z:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector4(CCExpression Vec)
{
    return CCExpressionGetFloatArray(Vec, (CCVector4D){}.v, 1, 4, NULL, TRUE);
}

CCVector4D CCExpressionGetNamedVector4(CCExpression Vec)
{
    CCVector4D Result;
    CCExpressionGetFloatArray(Vec, Result.v, 1, 4, "Vector4D should evaluate to a list of 4 numbers. (x:number y:number z:number w:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector2i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector2Di){}.v, 1, 2, NULL, TRUE);
}

CCVector2Di CCExpressionGetNamedVector2i(CCExpression Vec)
{
    CCVector2Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 1, 2, "Vector2Di should evaluate to a list of 2 numbers. (x:number y:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector3i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector3Di){}.v, 1, 3, NULL, TRUE);
}

CCVector3Di CCExpressionGetNamedVector3i(CCExpression Vec)
{
    CCVector3Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 1, 3, "Vector3Di should evaluate to a list of 3 numbers. (x:number y:number z:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedVector4i(CCExpression Vec)
{
    return CCExpressionGetIntegerArray(Vec, (CCVector4Di){}.v, 1, 4, NULL, TRUE);
}

CCVector4Di CCExpressionGetNamedVector4i(CCExpression Vec)
{
    CCVector4Di Result;
    CCExpressionGetIntegerArray(Vec, Result.v, 1, 4, "Vector4Di should evaluate to a list of 4 numbers. (x:number y:number z:number w:number)", TRUE);
    
    return Result;
}

_Bool CCExpressionIsNamedRect(CCExpression Rect)
{
    return CCExpressionIsNamedVector4(Rect);
}

CCRect CCExpressionGetNamedRect(CCExpression Rect)
{
    CCVector4D Result;
    CCExpressionGetFloatArray(Rect, Result.v, 1, 4, "Rect should evaluate to a list of 4 numbers. (x:number y:number width:number height:number)", TRUE);
    
    return (CCRect){ Result.v[0], Result.v[1], Result.v[2], Result.v[3] };
}

_Bool CCExpressionIsNamedColour(CCExpression Colour)
{
    size_t Count;
    return CCExpressionGetColourArray(Colour, (CCVector4D){}.v, 1, &Count, NULL, TRUE);
}

CCColourRGBA CCExpressionGetNamedColour(CCExpression Colour)
{
    size_t Count;
    CCVector4D Result;
    CCExpressionGetColourArray(Colour, Result.v, 1, &Count, "Colour should evaluate to a list of 3 or 4 numbers. (r:number g:number b:number [alpha:number])", TRUE);
    
    return (CCColourRGBA){ Result.v[0], Result.v[1], Result.v[2], Count == 4 ? Result.v[3] : 1.0f };
}
