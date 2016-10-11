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
#include "ExpressionHelpers.h"

typedef struct {
    CCExpression data;
    CCOrderedCollection children;
    CCExpression render;
    CCExpression control;
} GUIExpressionInfo;

static void *GUIExpressionConstructor(CCAllocatorType Allocator);
static void GUIExpressionDestructor(GUIExpressionInfo *Internal);
static void GUIExpressionRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index);
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


static void GUIExpressionChildrenElementDestructor(CCCollection Collection, GUIObject *Element)
{
    (*Element)->parent = NULL;
    GUIObjectDestroy(*Element);
}

GUIObject GUIExpressionCreate(CCAllocatorType Allocator, CCExpression Expression)
{
    GUIObject Object = GUIObjectCreate(Allocator, GUIExpression);
    
    ((GUIExpressionInfo*)Object->internal)->data = CCExpressionCopy(Expression);
    
    return Object;
}

//TODO: Later should probably runtime create these so they can be tagged version or maybe make a tagged macro that will get precomputed.
static CCString StrX = CC_STRING(".x"), StrY = CC_STRING(".y"), StrWidth = CC_STRING(".width"), StrHeight = CC_STRING(".height"), StrRect = CC_STRING(".rect"), StrEnabled = CC_STRING(".enabled"), StrRender = CC_STRING("render:"), StrChildren = CC_STRING("children:"), StrControl = CC_STRING("control:");
static CCExpression Window = NULL;
static void *GUIExpressionConstructor(CCAllocatorType Allocator)
{
    if (!Window)
    {
        Window = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, NULL, NULL, NULL);
        CCExpressionCreateState(Window, StrWidth, CCExpressionCreateFromSource("(window-width)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Window, StrHeight, CCExpressionCreateFromSource("(window-height)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
    }
    
    GUIExpressionInfo *Internal = CCMalloc(Allocator, sizeof(GUIExpressionInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    Internal->data = NULL;
    Internal->children = CCCollectionCreate(Allocator, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating | CCCollectionHintSizeSmall, sizeof(GUIObject), (CCCollectionElementDestructor)GUIExpressionChildrenElementDestructor);
    Internal->render = NULL;
    Internal->control = NULL;
    
    return Internal;
}

static void GUIExpressionDestructor(GUIExpressionInfo *Internal)
{
    if (Internal->data) CCExpressionDestroy(Internal->data);
    CCCollectionDestroy(Internal->children);
    
    CC_SAFE_Free(Internal);
}

#include "AssetManager.h"
#include "Window.h"
#include "AssetExpressions.h"
#include "GraphicsExpressions.h"

static void GUIExpressionDraw(CCExpression Render, GFXFramebuffer Framebuffer, size_t Index, GFXBuffer Projection)
{
    if (CCExpressionGetType(Render) == CCGraphicsExpressionValueTypeDraw)
    {
        const CCGraphicsExpressionValueDraw *Drawable = CCExpressionGetData(Render);
        GFXDrawSetFramebuffer(Drawable->drawer, Framebuffer, Index);
        GFXDrawSetBuffer(Drawable->drawer, CC_STRING("modelViewProjectionMatrix"), Projection);
        GFXDrawSubmit(Drawable->drawer, GFXPrimitiveTypeTriangleStrip, 0, Drawable->vertices);
    }
    
    else if (CCExpressionGetType(Render) == CCGraphicsExpressionValueTypeText)
    {
        CCOrderedCollection Drawables = CCTextGetDrawables(CCExpressionGetData(Render));
        CC_COLLECTION_FOREACH_PTR(CCTextDrawable, Drawable, Drawables)
        {
            GFXDrawSetFramebuffer(Drawable->drawer, Framebuffer, Index);
            GFXDrawSetBuffer(Drawable->drawer, CC_STRING("modelViewProjectionMatrix"), Projection);
            GFXDrawSubmitIndexed(Drawable->drawer, GFXPrimitiveTypeTriangleStrip, 0, Drawable->vertices);
        }
        CCCollectionDestroy(Drawables);
    }
}

static void GUIExpressionRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index)
{
    GUIObject Parent = GUIObjectGetParent(Object);
    ((GUIExpressionInfo*)Object->internal)->data->state.super = Parent ? GUIExpressionGetState(Parent) : Window;
    
    if (((GUIExpressionInfo*)Object->internal)->render)
    {
        int w, h;
        glfwGetFramebufferSize(CCWindow, &w, &h);
        CCMatrix4 Ortho = CCMatrix4MakeOrtho(0.0f, w, 0.0f, h, 0.0f, 1.0f);
        GFXBuffer Proj = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, sizeof(CCMatrix4), &Ortho);
        
        CC_COLLECTION_FOREACH(CCExpression, Render, CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->render))
        {
            Render->state.super = ((GUIExpressionInfo*)Object->internal)->render;
            Render = CCExpressionEvaluate(Render);
            
            if (CCExpressionGetType(Render) == CCExpressionValueTypeList)
            {
                CC_COLLECTION_FOREACH(CCExpression, Draw, CCExpressionGetList(Render))
                {
                    GUIExpressionDraw(Draw, Framebuffer, Index, Proj);
                }
            }
            
            else
            {
                GUIExpressionDraw(Render, Framebuffer, Index, Proj);
            }
        }
        
        GFXBufferDestroy(Proj);
    }
    
    CC_COLLECTION_FOREACH(GUIObject, Child, ((GUIExpressionInfo*)Object->internal)->children)
    {
        GUIObjectRender(Child, Framebuffer, Index);
    }
}

static void GUIExpressionEvent(GUIObject Object, GUIEvent Event)
{
    /*
     TODO: make CCExpression threadsafe
     
     Easiest way to achieve that is to separate the return state, and instead have environment passed in
     that will keep local state (return values and temporary values, super's, etc.).
     
     Then just need to make expression state that's left such as CCExpressionState->values thread safe.
     This can probably be done by setting atomic locks in the Set/Get state functions, for simplicity
     might just use a spinlock table like Obj-C uses for atomic properties. Unless adding individual
     spinlocks to expressions won't increase the size by much.
     */
    
    GUIObject Parent = GUIObjectGetParent(Object);
    ((GUIExpressionInfo*)Object->internal)->data->state.super = Parent ? GUIExpressionGetState(Parent) : Window;
    
    if (((GUIExpressionInfo*)Object->internal)->control)
    {
        CCExpressionSetState(((GUIExpressionInfo*)Object->internal)->control, CC_STRING("@gui-event"), CCExpressionCreateCustomType(CC_STD_ALLOCATOR, CCExpressionValueTypeUnspecified, (void*)Event, NULL, NULL), FALSE);
        
        CC_COLLECTION_FOREACH(CCExpression, Control, CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->control))
        {
            Control->state.super = ((GUIExpressionInfo*)Object->internal)->control;
            Control = CCExpressionEvaluate(Control);
        }
    }
    
    CC_COLLECTION_FOREACH(GUIObject, Child, ((GUIExpressionInfo*)Object->internal)->children)
    {
        GUIObjectEvent(Child, Event);
    }
}

static CCRect GUIExpressionGetRect(GUIObject Object)
{
    GUIObject Parent = GUIObjectGetParent(Object);
    ((GUIExpressionInfo*)Object->internal)->data->state.super = Parent ? GUIExpressionGetState(Parent) : Window;
    
    CCExpression Rect = CCExpressionGetState(((GUIExpressionInfo*)Object->internal)->data, StrRect);
    if (Rect)
    {
        return CCExpressionGetRect(Rect);
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_ERROR("Required rect state is missing.");
    
    return (CCRect){ 0.0f, 0.0f, 0.0f, 0.0f };
}

static void GUIExpressionSetRect(GUIObject Object, CCRect Rect)
{
    CCExpressionSetState(((GUIExpressionInfo*)Object->internal)->data, StrRect, CCExpressionCreateRect(CC_STD_ALLOCATOR, Rect), FALSE);
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
    CCOrderedCollectionAppendElement(((GUIExpressionInfo*)Object->internal)->children, &Child);
}

static CCComparisonResult GUIExpressionFindChild(const GUIObject *left, const GUIObject *right)
{
    return *left == *right ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static void GUIExpressionRemoveChild(GUIObject Object, GUIObject Child)
{
    CCCollectionRemoveElement(((GUIExpressionInfo*)Object->internal)->children, CCCollectionFindElement(((GUIExpressionInfo*)Object->internal)->children, &Child, (CCComparator)GUIExpressionFindChild));
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
        size_t RenderIndex = 0, ControlIndex = 0;
        CCExpression BaseRender = NULL, BaseControl = NULL;
        CCOrderedCollection Children = NULL;
        
        CCExpressionCreateState(Expression, StrX, CCExpressionCreateFromSource("(get 0 .rect)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Expression, StrY, CCExpressionCreateFromSource("(get 1 .rect)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Expression, StrWidth, CCExpressionCreateFromSource("(get 2 .rect)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Expression, StrHeight, CCExpressionCreateFromSource("(get 3 .rect)"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Expression, StrRect, CCExpressionCreateFromSource("(super (.x .y .width .height))"), FALSE, CCExpressionCreateFromSource("(frame-changed?)"), FALSE);
        CCExpressionCreateState(Expression, StrEnabled, CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1), FALSE, NULL, FALSE);
        
        CC_COLLECTION_FOREACH(CCExpression, InitExpr, CCExpressionGetList(*Initializer))
        {
            CCExpressionStateSetSuper(InitExpr, Expression);
            
            if ((CCExpressionGetType(InitExpr) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(InitExpr))))
            {
                CCExpression Arg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(InitExpr), 0);
                if (CCExpressionGetType(Arg) == CCExpressionValueTypeAtom)
                {
                    CCString Atom = CCExpressionGetAtom(Arg);
                    
                    if (CCStringEqual(Atom, StrRender))
                    {
                        BaseRender = InitExpr;
                    }
                    
                    else if (CCStringEqual(Atom, StrChildren))
                    {
                        if (!Children) Children = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(GUIObject), NULL);
                        
                        CC_COLLECTION_FOREACH(CCExpression, Child, CCExpressionGetList(InitExpr))
                        {
                            CCExpressionStateSetSuper(Child, InitExpr);
                            Child = CCExpressionEvaluate(Child);
                            if (CCExpressionGetType(Child) == GUIExpressionValueTypeGUIObject)
                            {
                                CCExpressionChangeOwnership(Child, NULL, NULL);
                                CCOrderedCollectionAppendElement(Children, &(GUIObject){ CCExpressionGetData(Child) });
                            }
                        }
                    }
                    
                    else if (CCStringEqual(Atom, StrControl))
                    {
                        BaseControl = InitExpr;
                    }
                    
                    else
                    {
                        CCExpressionEvaluate(InitExpr);
                    }
                }
                
                else
                {
                    CCExpressionEvaluate(InitExpr);
                }
            }
        }
        
        if (Expression->state.remove)
        {
            CCCollectionDestroy(Expression->state.remove);
            Expression->state.remove = NULL;
        }
        
        //override
        size_t Index = 0;
        while ((Expr = CCCollectionEnumeratorNext(&Enumerator)))
        {
            Index++;
            if ((CCExpressionGetType(*Expr) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(*Expr))))
            {
                CCExpression Arg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*Expr), 0);
                if (CCExpressionGetType(Arg) == CCExpressionValueTypeAtom)
                {
                    CCString Atom = CCExpressionGetAtom(Arg);
                    
                    if (CCStringEqual(Atom, StrRender))
                    {
                        RenderIndex = Index;
                    }
                    
                    else if (CCStringEqual(Atom, StrChildren))
                    {
                        if (!Children) Children = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(GUIObject), NULL);
                        
                        CC_COLLECTION_FOREACH(CCExpression, Child, CCExpressionGetList(*Expr))
                        {
                            CCExpressionStateSetSuper(Child, *Expr);
                            Child = CCExpressionEvaluate(Child);
                            if (CCExpressionGetType(Child) == GUIExpressionValueTypeGUIObject)
                            {
                                CCExpressionChangeOwnership(Child, NULL, NULL);
                                CCOrderedCollectionAppendElement(Children, &(GUIObject){ CCExpressionGetData(Child) });
                            }
                        }
                    }
                    
                    else if (CCStringEqual(Atom, StrControl))
                    {
                        ControlIndex = Index;
                    }
                    
                    else
                    {
                        CCExpressionEvaluate(*Expr);
                    }
                }
                
                else
                {
                    CCExpressionEvaluate(*Expr);
                }
            }
        }
        
        GUIObject Object = GUIExpressionCreate(CC_STD_ALLOCATOR, Expression);
        
        if (Children)
        {
            CC_COLLECTION_FOREACH(GUIObject, Child, Children) GUIObjectAddChild(Object, Child);
            CCCollectionDestroy(Children);
        }
        
        if (BaseRender)
        {
            if (RenderIndex)
            {
                CCExpression Render = (((GUIExpressionInfo*)Object->internal)->render = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->data), RenderIndex));
                
                CCEnumerator Enumerator;
                CCCollectionGetEnumerator(CCExpressionGetList(BaseRender), &Enumerator);

                size_t Index = 1;
                for (CCExpression *RenderExpr = CCCollectionEnumeratorNext(&Enumerator); RenderExpr; RenderExpr = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCOrderedCollectionInsertElementAtIndex(CCExpressionGetList(Render), &(CCExpression){ CCExpressionCopy(*RenderExpr) }, Index++);
                }
            }
            
            else
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->data), &(CCExpression){ (((GUIExpressionInfo*)Object->internal)->render = CCExpressionCopy(BaseRender)) });
            }
            
            ((GUIExpressionInfo*)Object->internal)->render->state.super = ((GUIExpressionInfo*)Object->internal)->data;
        }
        
        if (BaseControl)
        {
            if (ControlIndex)
            {
                CCExpression Control = (((GUIExpressionInfo*)Object->internal)->control = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->data), ControlIndex));
                
                CCEnumerator Enumerator;
                CCCollectionGetEnumerator(CCExpressionGetList(BaseControl), &Enumerator);
                
                size_t Index = 1;
                for (CCExpression *ControlExpr = CCCollectionEnumeratorNext(&Enumerator); ControlExpr; ControlExpr = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCOrderedCollectionInsertElementAtIndex(CCExpressionGetList(Control), &(CCExpression){ CCExpressionCopy(*ControlExpr) }, Index++);
                }
            }
            
            else
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->data), &(CCExpression){ (((GUIExpressionInfo*)Object->internal)->control = CCExpressionCopy(BaseControl)) });
            }
            
            ((GUIExpressionInfo*)Object->internal)->control->state.super = ((GUIExpressionInfo*)Object->internal)->data;
        }
        
        else if (ControlIndex)
        {
            ((GUIExpressionInfo*)Object->internal)->control = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(((GUIExpressionInfo*)Object->internal)->data), ControlIndex);
            
            ((GUIExpressionInfo*)Object->internal)->control->state.super = ((GUIExpressionInfo*)Object->internal)->data;
        }
        
        if (((GUIExpressionInfo*)Object->internal)->control) CCExpressionCreateState(((GUIExpressionInfo*)Object->internal)->control, CC_STRING("@gui-event"), NULL, FALSE, NULL, FALSE);
        
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

static _Bool GUIExpressionOnEventCursorPredicate(GUIEvent Event, CCExpression Args, size_t ArgCount, _Bool *Predicate)
{
    _Bool IsEvent = FALSE;
    if ((IsEvent = (Event->type == GUIEventTypeMouse)) && (ArgCount == 1))
    {
        CCExpression RectArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 1);
        if (CCExpressionGetType(RectArg) == CCExpressionValueTypeList)
        {
            CCRect Rect = CCExpressionGetRect(RectArg);
            *Predicate = ((Rect.position.x <= Event->mouse.state.position.x) && (Rect.position.x + Rect.size.x >= Event->mouse.state.position.x) &&
                          (Rect.position.y <= Event->mouse.state.position.y) && (Rect.position.y + Rect.size.y >= Event->mouse.state.position.y));
            
            if (CCExpressionGetStateStrict(CCExpressionStateGetSuper(Args), CC_STRING("@pos")))
            {
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@pos"), CCExpressionCreateVector2(CC_STD_ALLOCATOR, Event->mouse.state.position), FALSE);
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@pos-x"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.position.x), FALSE);
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@pos-y"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.position.y), FALSE);
            }
            
            else
            {
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@pos"), CCExpressionCreateVector2(CC_STD_ALLOCATOR, Event->mouse.state.position), FALSE, NULL, FALSE);
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@pos-x"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.position.x), FALSE, NULL, FALSE);
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@pos-y"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.position.y), FALSE, NULL, FALSE);
            }
        }
    }
    
    return IsEvent;
}

static _Bool GUIExpressionOnEventClickPredicate(GUIEvent Event, CCExpression Args, size_t ArgCount, _Bool *Predicate)
{
    _Bool IsEvent = FALSE;
    if ((Event->type == GUIEventTypeMouse) && (IsEvent = (Event->mouse.event == CCMouseEventButton)) && (ArgCount == 2))
    {
        CCExpression Button = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 1);
        if (CCExpressionGetType(Button) == CCExpressionValueTypeAtom)
        {
            uint32_t Mods = 0;
            if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("shift")) != SIZE_MAX) Mods |= GLFW_MOD_SHIFT;
            if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("ctrl")) != SIZE_MAX) Mods |= GLFW_MOD_CONTROL;
            if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("alt")) != SIZE_MAX) Mods |= GLFW_MOD_ALT;
            if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("cmd")) != SIZE_MAX) Mods |= GLFW_MOD_SUPER;
            
            uint32_t Key = UINT32_MAX;
            if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("left")) != SIZE_MAX) Key = GLFW_MOUSE_BUTTON_LEFT;
            else if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("right")) != SIZE_MAX) Key = GLFW_MOUSE_BUTTON_RIGHT;
            else if (CCStringFindSubstring(CCExpressionGetAtom(Button), 0, CC_STRING("middle")) != SIZE_MAX) Key = GLFW_MOUSE_BUTTON_MIDDLE;
            
            if ((Event->mouse.state.button.button == Key) && (Event->mouse.state.button.flags == Mods))
            {
                CCExpression RectArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 2);
                if (CCExpressionGetType(RectArg) == CCExpressionValueTypeList)
                {
                    CCRect Rect = CCExpressionGetRect(RectArg);
                    *Predicate = ((Rect.position.x <= Event->mouse.state.position.x) && (Rect.position.x + Rect.size.x >= Event->mouse.state.position.x) &&
                                  (Rect.position.y <= Event->mouse.state.position.y) && (Rect.position.y + Rect.size.y >= Event->mouse.state.position.y));
                    
                    if (CCExpressionGetStateStrict(CCExpressionStateGetSuper(Args), CC_STRING("@press")))
                    {
                        CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@press"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Event->mouse.state.button.state.down), FALSE);
                    }
                    
                    else
                    {
                        CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@press"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Event->mouse.state.button.state.down), FALSE, NULL, FALSE);
                    }
                }
            }
        }
    }
    
    return IsEvent;
}

static _Bool GUIExpressionOnEventScrollPredicate(GUIEvent Event, CCExpression Args, size_t ArgCount, _Bool *Predicate)
{
    _Bool IsEvent = FALSE;
    if ((Event->type == GUIEventTypeMouse) && (IsEvent = (Event->mouse.event == CCMouseEventScroll)) && (ArgCount == 1))
    {
        CCExpression RectArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 1);
        if (CCExpressionGetType(RectArg) == CCExpressionValueTypeList)
        {
            CCRect Rect = CCExpressionGetRect(RectArg);
            *Predicate = ((Rect.position.x <= Event->mouse.state.position.x) && (Rect.position.x + Rect.size.x >= Event->mouse.state.position.x) &&
                          (Rect.position.y <= Event->mouse.state.position.y) && (Rect.position.y + Rect.size.y >= Event->mouse.state.position.y));
            
            if (CCExpressionGetStateStrict(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta")))
            {
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta"), CCExpressionCreateVector2(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta), FALSE);
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta-x"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta.x), FALSE);
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta-y"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta.y), FALSE);
            }
            
            else
            {
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta"), CCExpressionCreateVector2(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta), FALSE, NULL, FALSE);
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta-x"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta.x), FALSE, NULL, FALSE);
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@scroll-delta-y"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, Event->mouse.state.scroll.delta.y), FALSE, NULL, FALSE);
            }
        }
    }
    
    return IsEvent;
}

static _Bool GUIExpressionOnEventDropPredicate(GUIEvent Event, CCExpression Args, size_t ArgCount, _Bool *Predicate)
{
    _Bool IsEvent = FALSE;
    if ((Event->type == GUIEventTypeMouse) && (IsEvent = (Event->mouse.event == CCMouseEventDrop)) && (ArgCount == 1))
    {
        CCExpression RectArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 1);
        if (CCExpressionGetType(RectArg) == CCExpressionValueTypeList)
        {
            CCRect Rect = CCExpressionGetRect(RectArg);
            *Predicate = ((Rect.position.x <= Event->mouse.state.position.x) && (Rect.position.x + Rect.size.x >= Event->mouse.state.position.x) &&
                          (Rect.position.y <= Event->mouse.state.position.y) && (Rect.position.y + Rect.size.y >= Event->mouse.state.position.y));
            
            CCExpression Files = CCExpressionCreateList(CC_STD_ALLOCATOR);
            CC_COLLECTION_FOREACH(CCString, Path, Event->mouse.state.drop.files)
            {
                CCOrderedCollectionAppendElement(CCExpressionGetList(Files), &(CCExpression){ CCExpressionCreateString(CC_STD_ALLOCATOR, Path, TRUE) });
            }
            
            if (CCExpressionGetStateStrict(CCExpressionStateGetSuper(Args), CC_STRING("@drop-files")))
            {
                CCExpressionSetState(CCExpressionStateGetSuper(Args), CC_STRING("@drop-files"), Files, FALSE);
            }
            
            else
            {
                CCExpressionCreateState(CCExpressionStateGetSuper(Args), CC_STRING("@drop-files"), Files, FALSE, NULL, FALSE);
            }
        }
    }
    
    return IsEvent;
}

static CCString NamedKeys[GLFW_KEY_LAST + 1] = {
    [32] = CC_STRING(":space"),
    [39] = CC_STRING(":apostrophe"),           /* ' */
    [44] = CC_STRING(":comma"),                /* , */
    [45] = CC_STRING(":minus"),                /* - */
    [46] = CC_STRING(":period"),               /* . */
    [47] = CC_STRING(":slash"),                /* / */
    [48] = CC_STRING(":0"),
    [49] = CC_STRING(":1"),
    [50] = CC_STRING(":2"),
    [51] = CC_STRING(":3"),
    [52] = CC_STRING(":4"),
    [53] = CC_STRING(":5"),
    [54] = CC_STRING(":6"),
    [55] = CC_STRING(":7"),
    [56] = CC_STRING(":8"),
    [57] = CC_STRING(":9"),
    [59] = CC_STRING(":semicolon"),            /* ; */
    [61] = CC_STRING(":equal"),                /* = */
    [65] = CC_STRING(":a"),
    [66] = CC_STRING(":b"),
    [67] = CC_STRING(":c"),
    [68] = CC_STRING(":d"),
    [69] = CC_STRING(":e"),
    [70] = CC_STRING(":f"),
    [71] = CC_STRING(":g"),
    [72] = CC_STRING(":h"),
    [73] = CC_STRING(":i"),
    [74] = CC_STRING(":j"),
    [75] = CC_STRING(":k"),
    [76] = CC_STRING(":l"),
    [77] = CC_STRING(":m"),
    [78] = CC_STRING(":n"),
    [79] = CC_STRING(":o"),
    [80] = CC_STRING(":p"),
    [81] = CC_STRING(":q"),
    [82] = CC_STRING(":r"),
    [83] = CC_STRING(":s"),
    [84] = CC_STRING(":t"),
    [85] = CC_STRING(":u"),
    [86] = CC_STRING(":v"),
    [87] = CC_STRING(":w"),
    [88] = CC_STRING(":x"),
    [89] = CC_STRING(":y"),
    [90] = CC_STRING(":z"),
    [91] = CC_STRING(":left-bracket"),         /* [ */
    [92] = CC_STRING(":backslash"),            /* \ */
    [93] = CC_STRING(":right-bracket"),        /* ] */
    [96] = CC_STRING(":grave-accent"),         /* ` */
    [161] = CC_STRING(":world-1"),             /* non-us #1 */
    [162] = CC_STRING(":world-2"),             /* non-us #2 */
    
    /* function keys */
    [256] = CC_STRING(":escape"),
    [257] = CC_STRING(":enter"),
    [258] = CC_STRING(":tab"),
    [259] = CC_STRING(":backspace"),
    [260] = CC_STRING(":insert"),
    [261] = CC_STRING(":delete"),
    [262] = CC_STRING(":right"),
    [263] = CC_STRING(":left"),
    [264] = CC_STRING(":down"),
    [265] = CC_STRING(":up"),
    [266] = CC_STRING(":page-up"),
    [267] = CC_STRING(":page-down"),
    [268] = CC_STRING(":home"),
    [269] = CC_STRING(":end"),
    [280] = CC_STRING(":caps-lock"),
    [281] = CC_STRING(":scroll-lock"),
    [282] = CC_STRING(":num-lock"),
    [283] = CC_STRING(":print-screen"),
    [284] = CC_STRING(":pause"),
    [290] = CC_STRING(":f1"),
    [291] = CC_STRING(":f2"),
    [292] = CC_STRING(":f3"),
    [293] = CC_STRING(":f4"),
    [294] = CC_STRING(":f5"),
    [295] = CC_STRING(":f6"),
    [296] = CC_STRING(":f7"),
    [297] = CC_STRING(":f8"),
    [298] = CC_STRING(":f9"),
    [299] = CC_STRING(":f10"),
    [300] = CC_STRING(":f11"),
    [301] = CC_STRING(":f12"),
    [302] = CC_STRING(":f13"),
    [303] = CC_STRING(":f14"),
    [304] = CC_STRING(":f15"),
    [305] = CC_STRING(":f16"),
    [306] = CC_STRING(":f17"),
    [307] = CC_STRING(":f18"),
    [308] = CC_STRING(":f19"),
    [309] = CC_STRING(":f20"),
    [310] = CC_STRING(":f21"),
    [311] = CC_STRING(":f22"),
    [312] = CC_STRING(":f23"),
    [313] = CC_STRING(":f24"),
    [314] = CC_STRING(":f25"),
    [320] = CC_STRING(":kp-0"),
    [321] = CC_STRING(":kp-1"),
    [322] = CC_STRING(":kp-2"),
    [323] = CC_STRING(":kp-3"),
    [324] = CC_STRING(":kp-4"),
    [325] = CC_STRING(":kp-5"),
    [326] = CC_STRING(":kp-6"),
    [327] = CC_STRING(":kp-7"),
    [328] = CC_STRING(":kp-8"),
    [329] = CC_STRING(":kp-9"),
    [330] = CC_STRING(":kp-decimal"),
    [331] = CC_STRING(":kp-divide"),
    [332] = CC_STRING(":kp-multiply"),
    [333] = CC_STRING(":kp-subtract"),
    [334] = CC_STRING(":kp-add"),
    [335] = CC_STRING(":kp-enter"),
    [336] = CC_STRING(":kp-equal"),
    [340] = CC_STRING(":left-shift"),
    [341] = CC_STRING(":left-control"),
    [342] = CC_STRING(":left-alt"),
    [343] = CC_STRING(":left-super"),
    [344] = CC_STRING(":right-shift"),
    [345] = CC_STRING(":right-control"),
    [346] = CC_STRING(":right-alt"),
    [347] = CC_STRING(":right-super"),
    [348] = CC_STRING(":menu")
};
static _Bool GUIExpressionOnEventKeyPredicate(GUIEvent Event, CCExpression Args, size_t ArgCount, _Bool *Predicate)
{
    _Bool IsEvent = FALSE;
    if ((Event->type == GUIEventTypeKey) && ((ArgCount == 1) || (ArgCount == 2)))
    {
        /*
         TODO: Workout option arguments
         
         (key: :any) ; matches any key, no false expression
         (key: :grapheme) ; matches any printable key, false for any other key.
         (key: :control) ; matches any control combination key, false for any other key.
         (key: :delete) ; matches the delete key, false for any other key.
         (key: :enter) ; matches the enter key, false for any other key.
         (key: :tab) ; matches the tab key, false for any other key.
         (key: "s")  ; matches the "s" character, any other character will be false. caveat is "s" is only exposed on key-down not key-up.
         (key: 70)   ; matches keycode 70, any other keycode will be false.
         (key: key, :none) ; matches key with no modifiers, any other key will be false.
         (key: key :alt) ; matches key with alt modifier, any other key will be false.
         (key: key, :cmd-alt) ; matches key with cmd+alt modifier, any other key will be false.
         
         Default modifier matching behaviour will match any modifier combination.
         */
        CCExpression Mode = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Args), 1);
        if (CCExpressionGetType(Mode) == CCExpressionValueTypeAtom)
        {
            if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":any")))
            {
                IsEvent = TRUE;
                *Predicate = TRUE;
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":grapheme")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.character != 0) && !(Event->key.state.flags & ~(GLFW_MOD_SHIFT | GLFW_MOD_ALT));
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":control")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.flags & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER));
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":delete")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.keycode == GLFW_KEY_BACKSPACE);
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":enter")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.keycode == GLFW_KEY_ENTER);
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":tab")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.keycode == GLFW_KEY_TAB);
            }
            
            else if (CCStringEqual(CCExpressionGetAtom(Mode), CC_STRING(":direction")))
            {
                IsEvent = TRUE;
                *Predicate = (Event->key.state.keycode == GLFW_KEY_LEFT) || (Event->key.state.keycode == GLFW_KEY_RIGHT) || (Event->key.state.keycode == GLFW_KEY_UP) || (Event->key.state.keycode == GLFW_KEY_DOWN);
            }
        }
        
        
        if (IsEvent)
        {
            CCString Key = (Event->key.state.keycode != GLFW_KEY_UNKNOWN ? NamedKeys[Event->key.state.keycode] : 0);
            
            CCExpression Flags = CCExpressionCreateList(CC_STD_ALLOCATOR);
            if (Event->key.state.flags & GLFW_MOD_SHIFT) CCOrderedCollectionAppendElement(CCExpressionGetList(Flags), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":shift"), TRUE) });
            if (Event->key.state.flags & GLFW_MOD_ALT) CCOrderedCollectionAppendElement(CCExpressionGetList(Flags), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":alt"), TRUE) });
            if (Event->key.state.flags & GLFW_MOD_CONTROL) CCOrderedCollectionAppendElement(CCExpressionGetList(Flags), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":control"), TRUE) });
            if (Event->key.state.flags & GLFW_MOD_SUPER) CCOrderedCollectionAppendElement(CCExpressionGetList(Flags), &(CCExpression){ CCExpressionCreateAtom(CC_STD_ALLOCATOR, CC_STRING(":cmd"), TRUE) });
            
            struct {
                CCString name;
                CCExpression value;
            } Inputs[] = {
                { CC_STRING("@press"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Event->key.state.state.down) },
                { CC_STRING("@char"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, (CCStringHint)CCStringEncodingUTF8, (char*)(CCChar[2]){ Event->key.state.character, 0 }), FALSE) }, //TODO: Implement CCStringCreateCharacter() to safely handle multi-byte CCChar
                { CC_STRING("@keycode"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Event->key.state.keycode) },
                { CC_STRING("@repeat"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, Event->key.state.state.repeat) },
                { CC_STRING("@flags"), Flags },
                { CC_STRING("@key"), CCExpressionCreateString(CC_STD_ALLOCATOR, (Key ? Key : CC_STRING(":unknown")), TRUE) }
            };
            
            for (size_t Loop = 0; Loop < sizeof(Inputs) / sizeof(typeof(*Inputs)); Loop++)
            {
                if (CCExpressionGetStateStrict(CCExpressionStateGetSuper(Args), Inputs[Loop].name))
                {
                    CCExpressionSetState(CCExpressionStateGetSuper(Args), Inputs[Loop].name, Inputs[Loop].value, FALSE);
                }
                
                else
                {
                    CCExpressionCreateState(CCExpressionStateGetSuper(Args), Inputs[Loop].name, Inputs[Loop].value, FALSE, NULL, FALSE);
                }
            }
        }
    }
    
    return IsEvent;
}

CCExpression GUIExpressionOnEvent(CCExpression Expression)
{
    CCExpression Expr = Expression;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    
    if ((ArgCount >= 2) && (ArgCount <= 3))
    {
        CCExpression Result = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        
        _Bool Predicate = FALSE, IsEvent = FALSE;
        if (CCExpressionGetType(Result) == CCExpressionValueTypeList)
        {
            CCOrderedCollection EventPredicate = CCExpressionGetList(Result);
            const size_t EventPredicateArgCount = CCCollectionGetCount(EventPredicate);
            
            if (EventPredicateArgCount)
            {
                CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(EventPredicate, 0);
                if (CCExpressionGetType(Type) == CCExpressionValueTypeAtom)
                {
                    CCExpression EventState = CCExpressionGetState(Expression, CC_STRING("@gui-event"));
                    if ((EventState) && (CCExpressionGetType(EventState) == CCExpressionValueTypeUnspecified))
                    {
                        struct {
                            CCString name;
                            _Bool (*predicate)(GUIEvent, CCExpression, size_t, _Bool *);
                        } EventPredicates[] = {
                            { CC_STRING("cursor:"), GUIExpressionOnEventCursorPredicate },
                            { CC_STRING("click:"), GUIExpressionOnEventClickPredicate },
                            { CC_STRING("scroll:"), GUIExpressionOnEventScrollPredicate },
                            { CC_STRING("drop:"), GUIExpressionOnEventDropPredicate },
                            { CC_STRING("key:"), GUIExpressionOnEventKeyPredicate }
                        };
                        
                        GUIEvent Event = CCExpressionGetData(EventState);
                        for (size_t Loop = 0; Loop < sizeof(EventPredicates) / sizeof(typeof(*EventPredicates)); Loop++)
                        {
                            if (CCStringEqual(CCExpressionGetAtom(Type), EventPredicates[Loop].name))
                            {
                                IsEvent = EventPredicates[Loop].predicate(Event, Result, EventPredicateArgCount - 1, &Predicate);
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if (IsEvent)
        {
            if ((Predicate) || (ArgCount == 3)) Expr = CCExpressionRetain(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), Predicate ? 2 : 3)));
        }
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("on", "event-predicate:expr true:expr [false:expr]");
    
    return Expr;
}
