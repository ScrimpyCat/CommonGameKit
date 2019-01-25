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

#include "GUIObject.h"
#include "Window.h"

GUIObject GUIObjectCreate(CCAllocatorType Allocator, const GUIObjectInterface *Interface)
{
    CCAssertLog(Interface, "Interface must not be null");
    
    GUIObject Object = CCMalloc(Allocator, sizeof(GUIObjectInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Object)
    {
        *Object = (GUIObjectInfo){
            .interface = Interface,
            .allocator = Allocator,
            .parent = NULL,
            .internal = Interface->create(Allocator),
            .changed = TRUE
        };
        
        if (!Object->internal)
        {
            CC_LOG_ERROR("Failed to create GUI object: Implementation failure (%p)", Interface);
            CC_SAFE_Free(Object);
        }
        
        else if (mtx_init(&Object->lock, mtx_plain | mtx_recursive) != thrd_success)
        {
            CC_LOG_ERROR("Failed to create lock for GUIObject (%p)", Object);
        }
    }
    
    else CC_LOG_ERROR("Failed to create GUI object due to allocation failure. Allocation size (%zu)", sizeof(GUIObjectInfo));
    
    return Object;
}

void GUIObjectDestroy(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    GUIObject Parent = GUIObjectGetParent(Object);
    if (Parent)
    {
        GUIObjectRemoveChild(Parent, Object);
    }
    
    else
    {
        mtx_destroy(&Object->lock);
        Object->interface->destroy(Object->internal);
        CC_SAFE_Free(Object);
    }
}

void GUIObjectRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index, GFXBuffer Projection)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    if (GUIObjectGetEnabled(Object)) Object->interface->render(Object, Framebuffer, Index, Projection);
    Object->changed = FALSE;
    mtx_unlock(&Object->lock);
}

void GUIObjectEvent(GUIObject Object, GUIEvent Event)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    if (GUIObjectGetEnabled(Object)) Object->interface->event(Object, Event);
    mtx_unlock(&Object->lock);
}

CCRect GUIObjectGetRect(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const CCRect Rect = Object->interface->rect.get(Object);
    mtx_unlock(&Object->lock);
    
    return Rect;
}

void GUIObjectSetRect(GUIObject Object, CCRect Rect)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Object->interface->rect.set(Object, Rect);
    Object->changed = TRUE;
    mtx_unlock(&Object->lock);
}

_Bool GUIObjectGetEnabled(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const _Bool Enabled = Object->interface->enabled.get(Object);
    mtx_unlock(&Object->lock);
    
    return Enabled;
}

void GUIObjectSetEnabled(GUIObject Object, _Bool Enabled)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Object->interface->enabled.set(Object, Enabled);
    mtx_unlock(&Object->lock);
}

GUIObject GUIObjectGetParent(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    return Object->parent;
}

void GUIObjectAddChild(GUIObject Object, GUIObject Child)
{
    CCAssertLog(Object, "GUI object must not be null");
    CCAssertLog(Child, "Child GUI object must not be null");
    
    mtx_lock(&Object->lock);
    Child->parent = Object;
    Object->interface->child.add(Object, Child);
    Object->changed = TRUE;
    Child->changed = TRUE;
    mtx_unlock(&Object->lock);
}

void GUIObjectRemoveChild(GUIObject Object, GUIObject Child)
{
    CCAssertLog(Object, "GUI object must not be null");
    CCAssertLog(Child, "Child GUI object must not be null");
    CCAssertLog(Object == Child->parent, "The GUI object must be the parent of the child GUI object");
    
    mtx_lock(&Object->lock);
    Child->parent = NULL;
    Object->interface->child.remove(Object, Child);
    Object->changed = TRUE;
    Child->changed = TRUE;
    mtx_unlock(&Object->lock);
}

_Bool GUIObjectHasChanged(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    const _Bool HasChanged = Object->changed ? TRUE : Object->interface->changed(Object);
    mtx_unlock(&Object->lock);
    
    return HasChanged;
}

CCExpression GUIObjectEvaluateExpression(GUIObject Object, CCExpression Expression)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    CCExpression Expr = CCExpressionRetain(Object->interface->evaluate(Object, Expression));
    mtx_unlock(&Object->lock);
    
    CCExpressionStateSetResult(Expression, Expr);
    return Expr;
}

CCExpression GUIObjectGetExpressionState(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    mtx_lock(&Object->lock);
    CCExpression Expr = Object->interface->state(Object);
    mtx_unlock(&Object->lock);
    
    return Expr;
}
