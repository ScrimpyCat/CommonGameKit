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

#ifndef CommonGameKit_ExpressionHelpers_h
#define CommonGameKit_ExpressionHelpers_h

#include <CommonGameKit/Expression.h>

CC_NEW CCExpression CCExpressionCreateVector2(CCAllocatorType Allocator, CCVector2D v);
CC_NEW CCExpression CCExpressionCreateVector3(CCAllocatorType Allocator, CCVector3D v);
CC_NEW CCExpression CCExpressionCreateVector4(CCAllocatorType Allocator, CCVector4D v);
CC_NEW CCExpression CCExpressionCreateVector2i(CCAllocatorType Allocator, CCVector2Di v);
CC_NEW CCExpression CCExpressionCreateVector3i(CCAllocatorType Allocator, CCVector3Di v);
CC_NEW CCExpression CCExpressionCreateVector4i(CCAllocatorType Allocator, CCVector4Di v);
CC_NEW CCExpression CCExpressionCreateRect(CCAllocatorType Allocator, CCRect r);

_Bool CCExpressionIsVector2(CCExpression Vec);
_Bool CCExpressionIsVector3(CCExpression Vec);
_Bool CCExpressionIsVector4(CCExpression Vec);
_Bool CCExpressionIsVector2i(CCExpression Vec);
_Bool CCExpressionIsVector3i(CCExpression Vec);
_Bool CCExpressionIsVector4i(CCExpression Vec);
_Bool CCExpressionIsRect(CCExpression Rect);
_Bool CCExpressionIsColour(CCExpression Colour);

CCVector2D CCExpressionGetVector2(CCExpression Vec);
CCVector3D CCExpressionGetVector3(CCExpression Vec);
CCVector4D CCExpressionGetVector4(CCExpression Vec);
CCVector2Di CCExpressionGetVector2i(CCExpression Vec);
CCVector3Di CCExpressionGetVector3i(CCExpression Vec);
CCVector4Di CCExpressionGetVector4i(CCExpression Vec);
CCRect CCExpressionGetRect(CCExpression Rect);
CCColourRGBA CCExpressionGetColour(CCExpression Colour);

_Bool CCExpressionIsNamedVector2(CCExpression Vec);
_Bool CCExpressionIsNamedVector3(CCExpression Vec);
_Bool CCExpressionIsNamedVector4(CCExpression Vec);
_Bool CCExpressionIsNamedVector2i(CCExpression Vec);
_Bool CCExpressionIsNamedVector3i(CCExpression Vec);
_Bool CCExpressionIsNamedVector4i(CCExpression Vec);
_Bool CCExpressionIsNamedRect(CCExpression Rect);
_Bool CCExpressionIsNamedColour(CCExpression Colour);

CCString CCExpressionGetNamedString(CCExpression String);
int32_t CCExpressionGetNamedInteger(CCExpression Value);
float CCExpressionGetNamedFloat(CCExpression Value);
CCVector2D CCExpressionGetNamedVector2(CCExpression Vec);
CCVector3D CCExpressionGetNamedVector3(CCExpression Vec);
CCVector4D CCExpressionGetNamedVector4(CCExpression Vec);
CCVector2Di CCExpressionGetNamedVector2i(CCExpression Vec);
CCVector3Di CCExpressionGetNamedVector3i(CCExpression Vec);
CCVector4Di CCExpressionGetNamedVector4i(CCExpression Vec);
CCRect CCExpressionGetNamedRect(CCExpression Rect);
CCColourRGBA CCExpressionGetNamedColour(CCExpression Colour);

#endif
