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

#include "GUIExpression.h"

typedef struct {
    CCExpression data;
} GUIExpressionInfo;

static void *GUIExpressionConstructor(CCAllocatorType Allocator);
static void GUIExpressionDestructor(GUIExpressionInfo *Internal);
static void GUIExpressionRender(GUIObject Object, GFXFramebuffer Framebuffer);
static void GUIExpressionEvent(GUIObject Object, GUIEvent Event);
static CCRect GUIExpressionGetRect(GUIObject Object);
static void GUIExpressionSetRect(GUIObject Object, CCRect Rect);
static _Bool GUIExpressionGetEnabled(GUIObject Object);
static void GUIExpressionSetEnabled(GUIObject Object, _Bool Enabled);
static void GUIExpressionAddChild(GUIObject Object, GUIObject Child);
static void GUIExpressionRemoveChild(GUIObject Object, GUIObject Child);
static _Bool GUIExpressionHasChanged(GUIObject Object); //clear on render
static CCExpression GUIExpressionEvaluator(GUIObject Object, CCExpression Expression);
static CCExpression GUIExpressionGetState(GUIObject Object);


const GUIObjectInterface GUIExpressionInterface = {
    .create = (GUIObjectConstructorCallback)GUIExpressionConstructor,
    .destroy = (GUIObjectDestructorCallback)GUIExpressionDestructor,
    .render = (GUIObjectRenderCallback)GUIExpressionRender,
    .event = (GUIObjectEventCallback)GUIExpressionEvent,
    .rect = {
        .get = (GUIObjectGetRectCallback)GUIExpressionGetRect,
        .set = (GUIObjectSetRectCallback)GUIExpressionSetRect,
    },
    .enabled = {
        .get = (GUIObjectGetEnabledCallback)GUIExpressionGetEnabled,
        .set = (GUIObjectSetEnabledCallback)GUIExpressionSetEnabled,
    },
    .child = {
        .add = (GUIObjectAddChildCallback)GUIExpressionAddChild,
        .remove = (GUIObjectRemoveChildCallback)GUIExpressionRemoveChild,
    },
    .changed = (GUIObjectHasChangedCallback)GUIExpressionHasChanged,
    .evaluate = (GUIObjectEvaluatorCallback)GUIExpressionEvaluator,
    .state = (GUIObjectGetExpressionStateCallback)GUIExpressionGetState
};

const GUIObjectInterface * const GUIExpression = &GUIExpressionInterface;


GUIObject GUIExpressionCreate(CCAllocatorType Allocator, CCExpression Expression)
{
    GUIObject Object = GUIObjectCreate(Allocator, GUIExpression);
    
    ((GUIExpressionInfo*)Object->internal)->data = CCExpressionCopy(Expression);
    
    return Object;
}

//TODO: Later should probably runtime create these so they can be tagged version or maybe make a tagged macro that will get precomputed.
static CCString StrWidth = CC_STRING("width"), StrHeight = CC_STRING("height"), StrRect = CC_STRING("rect"), StrEnabled = CC_STRING("enabled"), StrRender = CC_STRING("render"), StrChildren = CC_STRING("children"), StrControl = CC_STRING("control");
static CCExpression Window = NULL;
static void *GUIExpressionConstructor(CCAllocatorType Allocator)
{
    if (!Window)
    {
        Window = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, NULL, NULL, NULL);
        CCExpressionCreateState(Window, StrWidth, CCExpressionCreateFromSource("(window-width)"), FALSE);
        CCExpressionCreateState(Window, StrHeight, CCExpressionCreateFromSource("(window-height)"), FALSE);
    }
    
    return CCMalloc(Allocator, sizeof(GUIExpressionInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
}

static void GUIExpressionDestructor(GUIExpressionInfo *Internal)
{
    CCExpressionDestroy(Internal->data);
    CC_SAFE_Free(Internal);
}

static void GUIExpressionRender(GUIObject Object, GFXFramebuffer Framebuffer)
{
    
}

static void GUIExpressionEvent(GUIObject Object, GUIEvent Event)
{
    
}

static CCRect GUIExpressionGetRect(GUIObject Object)
{
    GUIObject Parent = GUIObjectGetParent(Object);
    ((GUIExpressionInfo*)Object->internal)->data->state.super = Parent ? GUIExpressionGetState(Parent) : Window;
    
    CCExpression Rect = CCExpressionGetState(((GUIExpressionInfo*)Object->internal)->data, StrRect);
    if (Rect)
    {
        if ((CCExpressionGetType(Rect) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(Rect)) == 4))
        {
            float Values[4];
            for (int Loop = 0; Loop < 4; Loop++)
            {
                CCExpression Value = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Rect), Loop);
                
                if (CCExpressionGetType(Value) == CCExpressionValueTypeInteger)
                {
                    Values[Loop] = (float)CCExpressionGetInteger(Value);
                }
                
                else if (CCExpressionGetType(Value) == CCExpressionValueTypeFloat)
                {
                    Values[Loop] = CCExpressionGetFloat(Value);
                }
                
                else
                {
                    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Required rect state should evaluate to a list of 4 numbers. (x:number y:number width:number height:number)");
                    
                    return (CCRect){ 0.0f, 0.0f, 0.0f, 0.0f };
                }
            }
            
            return (CCRect){ Values[0], Values[1], Values[2], Values[3] };
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Required rect state is missing.");
    
    return (CCRect){ 0.0f, 0.0f, 0.0f, 0.0f };
}

static void GUIExpressionSetRect(GUIObject Object, CCRect Rect)
{
    CCExpression Expr = CCExpressionCreateList(CC_STD_ALLOCATOR);
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, Rect.position.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, Rect.position.y) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, Rect.size.x) });
    CCOrderedCollectionAppendElement(CCExpressionGetList(Expr), &(CCExpression){ CCExpressionCreateFloat(CC_STD_ALLOCATOR, Rect.size.y) });
    
    CCExpressionSetState(((GUIExpressionInfo*)Object->internal)->data, StrRect, Expr, FALSE);
}

static _Bool GUIExpressionGetEnabled(GUIObject Object)
{
    GUIObject Parent = GUIObjectGetParent(Object);
    ((GUIExpressionInfo*)Object->internal)->data->state.super = Parent ? GUIExpressionGetState(Parent) : Window;
    
    CCExpression Enabled = CCExpressionGetState(((GUIExpressionInfo*)Object->internal)->data, StrEnabled);
    if (Enabled)
    {
        if (CCExpressionGetType(Enabled) == CCExpressionValueTypeInteger)
        {
            return CCExpressionGetInteger(Enabled);
        }
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Required enabled state is missing.");
    
    return FALSE;
}

static void GUIExpressionSetEnabled(GUIObject Object, _Bool Enabled)
{
    CCExpressionSetState(((GUIExpressionInfo*)Object->internal)->data, StrEnabled, CCExpressionCreateInteger(CC_STD_ALLOCATOR, Enabled), FALSE);
}

static void GUIExpressionAddChild(GUIObject Object, GUIObject Child)
{
    
}

static void GUIExpressionRemoveChild(GUIObject Object, GUIObject Child)
{
    
}

static _Bool GUIExpressionHasChanged(GUIObject Object)
{
    return FALSE;
}

static CCExpression GUIExpressionEvaluator(GUIObject Object, CCExpression Expression)
{
    if (CCExpressionGetType(Expression) == CCExpressionValueTypeExpression)
    {
        Expression->state.super = ((GUIExpressionInfo*)Object->internal)->data;
        Expression = CCExpressionEvaluate(Expression);
        
        if (((GUIExpressionInfo*)Object->internal)->data->state.remove)
        {
            CCCollectionDestroy(((GUIExpressionInfo*)Object->internal)->data->state.remove);
            ((GUIExpressionInfo*)Object->internal)->data->state.remove = NULL;
        }
    }
    
    return Expression;
}

static CCExpression GUIExpressionGetState(GUIObject Object)
{
    return ((GUIExpressionInfo*)Object->internal)->data;
}

void GUIExpressionInitializerElementDestructor(CCCollection Collection, CCExpression *Element)
{
    CCExpressionDestroy(*Element);
}

static CCComparisonResult GUIExpressionInitializerElementFind(const CCExpression *Initializer, CCString Name)
{
    return CCStringEqual(CCExpressionGetString(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Initializer), 1)), Name) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCCollection GUIExpressionInitializers = NULL;
CCExpression GUIExpressionCreateObject(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
    CCExpression *Initializer = CCCollectionGetElement(GUIExpressionInitializers, CCCollectionFindElement(GUIExpressionInitializers, (void*)CCExpressionGetAtom(*Expr), (CCComparator)GUIExpressionInitializerElementFind));
    
    if (Initializer)
    {
        CCExpressionCreateState(Expression, StrWidth, CCExpressionCreateFromSource("(get 2 rect)"), FALSE);
        CCExpressionCreateState(Expression, StrHeight, CCExpressionCreateFromSource("(get 3 rect)"), FALSE);
        CCExpressionCreateState(Expression, StrRect, CCExpressionCreateFromSource("(super (0 0 width height))"), FALSE);
        CCExpressionCreateState(Expression, StrEnabled, CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1), FALSE);
        
        CC_COLLECTION_FOREACH(CCExpression, InitExpr, CCExpressionGetList(*Initializer))
        {
            InitExpr->state.super = Expression;
            
            if (CCExpressionGetType(InitExpr) == CCExpressionValueTypeList)
            {
                CCString Atom = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(InitExpr), 0));
                
                if (CCStringEqual(Atom, StrRender))
                {
                    
                }
                
                else if (CCStringEqual(Atom, StrChildren))
                {
                    
                }
                
                else if (CCStringEqual(Atom, StrControl))
                {
                    
                }
                
                else
                {
                    CCExpressionEvaluate(InitExpr);
                }
            }
        }
        
        CCCollectionDestroy(Expression->state.remove);
        Expression->state.remove = NULL;
        
        //override
        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
        {
            if (CCExpressionGetType(*Expr) == CCExpressionValueTypeList)
            {
                CCString Atom = CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0));
                
                if (CCStringEqual(Atom, StrRender))
                {
                    
                }
                
                else if (CCStringEqual(Atom, StrChildren))
                {
                    
                }
                
                else if (CCStringEqual(Atom, StrControl))
                {
                    
                }
                
                else
                {
                    CCExpressionEvaluate(*Expr);
                }
            }
        }
        
        
        GUIObject Object = GUIExpressionCreate(CC_STD_ALLOCATOR, Expression);
        
        CCExpression GUI = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)GUIExpressionValueTypeGUIObject, Object, NULL, (CCExpressionValueDestructor)GUIObjectDestroy);
        
        return GUI;
    }
    
    else
    {
        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find GUI object for type: %s", CCExpressionGetAtom(*Expr));
    }
    
    return Expression;
}

CCExpression GUIExpressionRegisterObject(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator);
    if (Expr)
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeString)
        {
            CCExpression Object = CCExpressionCopy(Expression);
            
            if (!GUIExpressionInitializers)
            {
                GUIExpressionInitializers = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyFinding, sizeof(CCExpression), (CCCollectionElementDestructor)GUIExpressionInitializerElementDestructor);
            }
            
            CCCollectionInsertElement(GUIExpressionInitializers, &Object);
            
            CCExpressionEvaluatorRegister(CCExpressionGetString(Result), GUIExpressionCreateObject);
        }
    }
    
    return Expression;
}

static CCExpression GUIExpressionPercentage(CCExpression Expression, const char *Name, _Bool UseHeight)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if (ArgCount == 1)
    {
        CCExpression Constraint = CCExpressionGetState(Expression, UseHeight ? StrHeight : StrWidth);
        
        int32_t Size = 0;
        if (Constraint)
        {
            if (CCExpressionGetType(Constraint) == CCExpressionValueTypeInteger) Size = CCExpressionGetInteger(Constraint);
            else if (CCExpressionGetType(Constraint) == CCExpressionValueTypeFloat) Size = (int32_t)CCExpressionGetFloat(Constraint);
        }
        
        CCExpression Percent = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Percent) == CCExpressionValueTypeInteger)
        {
            Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, (int32_t)((float)Size * ((float)CCExpressionGetInteger(Percent) / 100)));
        }
        
        else if (CCExpressionGetType(Percent) == CCExpressionValueTypeFloat)
        {
            Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, (float)Size * CCExpressionGetFloat(Percent));
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Name, "percent:number");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Name, "percent:number");
    
    return Expr;
}

CCExpression GUIExpressionPercentWidth(CCExpression Expression)
{
    return GUIExpressionPercentage(Expression, "percent-width", FALSE);
}

CCExpression GUIExpressionPercentHeight(CCExpression Expression)
{
    return GUIExpressionPercentage(Expression, "percent-height", TRUE);
}
