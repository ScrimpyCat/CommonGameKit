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
            .internal = Interface->create(Allocator)
        };
        
        if (!Object->internal)
        {
            CC_LOG_ERROR("Failed to create GUI object: Implementation failure (%p)", Interface);
            CC_SAFE_Free(Object);
        }
    }
    
    else CC_LOG_ERROR("Failed to create GUI object due to allocation failure. Allocation size (%zu)", sizeof(GUIObjectInfo));
    
    return Object;
}

void GUIObjectDestroy(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    Object->interface->destroy(Object);
    CC_SAFE_Free(Object);
}

void GUIObjectRender(GUIObject Object, GFXFramebuffer Framebuffer)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    Object->interface->render(Object, Framebuffer);
}

void GUIObjectEvent(GUIObject Object, GUIEvent Event)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    Object->interface->event(Object, Event);
}

CCRect GUIObjectGetRect(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    return Object->interface->getRect(Object);
}

void GUIObjectSetRect(GUIObject Object, CCRect Rect)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    Object->interface->setRect(Object, Rect);
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
    
    Object->interface->addChild(Object, Child);
    Child->parent = Object;
}

void GUIObjectRemoveChild(GUIObject Object, GUIObject Child)
{
    CCAssertLog(Object, "GUI object must not be null");
    CCAssertLog(Child, "Child GUI object must not be null");
    
    Object->interface->removeChild(Object, Child);
    Child->parent = NULL;
}

_Bool GUIObjectHasChanged(GUIObject Object)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    return Object->interface->changed(Object);
}

CCExpression GUIObjectEvaluateExpression(GUIObject Object, CCExpression Expression)
{
    CCAssertLog(Object, "GUI object must not be null");
    
    return Object->interface->evaluate(Object, Expression);
}
