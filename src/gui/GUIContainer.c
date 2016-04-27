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

#include "GUIContainer.h"

static void *GUIContainerConstructor(CCAllocatorType Allocator);
static void GUIContainerDestructor(void *Internal);
static void GUIContainerRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index);
static void GUIContainerEvent(GUIObject Object, GUIEvent Event);
static CCRect GUIContainerGetRect(GUIObject Object);
static void GUIContainerSetRect(GUIObject Object, CCRect Rect);
static _Bool GUIContainerGetEnabled(GUIObject Object);
static void GUIContainerSetEnabled(GUIObject Object, _Bool Enabled);
static void GUIContainerAddChild(GUIObject Object, GUIObject Child);
static void GUIContainerRemoveChild(GUIObject Object, GUIObject Child);
static _Bool GUIContainerHasChanged(GUIObject Object); //clear on render
static CCExpression GUIContainerEvaluator(GUIObject Object, CCExpression Expression);


const GUIObjectInterface GUIContainerInterface = {
    .create = (GUIObjectConstructorCallback)GUIContainerConstructor,
    .destroy = (GUIObjectDestructorCallback)GUIContainerDestructor,
    .render = (GUIObjectRenderCallback)GUIContainerRender,
    .event = (GUIObjectEventCallback)GUIContainerEvent,
    .rect = {
        .get = (GUIObjectGetRectCallback)GUIContainerGetRect,
        .set = (GUIObjectSetRectCallback)GUIContainerSetRect,
    },
    .enabled = {
        .get = (GUIObjectGetEnabledCallback)GUIContainerGetEnabled,
        .set = (GUIObjectSetEnabledCallback)GUIContainerSetEnabled,
    },
    .child = {
        .add = (GUIObjectAddChildCallback)GUIContainerAddChild,
        .remove = (GUIObjectRemoveChildCallback)GUIContainerRemoveChild,
    },
    .changed = (GUIObjectHasChangedCallback)GUIContainerHasChanged,
    .evaluate = (GUIObjectEvaluatorCallback)GUIContainerEvaluator
};

const GUIObjectInterface * const GUIContainer = &GUIContainerInterface;


static void *GUIContainerConstructor(CCAllocatorType Allocator)
{
    return NULL;
}

static void GUIContainerDestructor(void *Internal)
{
    
}

static void GUIContainerRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index)
{
    
}

static void GUIContainerEvent(GUIObject Object, GUIEvent Event)
{
    
}

static CCRect GUIContainerGetRect(GUIObject Object)
{
    return (CCRect){ 0.0f, 0.0f, 0.0f, 0.0f };
}

static void GUIContainerSetRect(GUIObject Object, CCRect Rect)
{
    
}

static _Bool GUIContainerGetEnabled(GUIObject Object)
{
    return FALSE;
}

static void GUIContainerSetEnabled(GUIObject Object, _Bool Enabled)
{
    
}

static void GUIContainerAddChild(GUIObject Object, GUIObject Child)
{
    
}

static void GUIContainerRemoveChild(GUIObject Object, GUIObject Child)
{
    
}

static _Bool GUIContainerHasChanged(GUIObject Object)
{
    return FALSE;
}

static CCExpression GUIContainerEvaluator(GUIObject Object, CCExpression Expression)
{
    return NULL;
}
