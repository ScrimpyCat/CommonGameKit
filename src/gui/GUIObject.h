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

#ifndef Blob_Game_GUIObject_h
#define Blob_Game_GUIObject_h

#include <CommonC/Common.h>
#include "GFX.h"
#include "InputSystem.h"
#include "Expression.h"

typedef const struct {
    enum {
        GUIEventTypeKey,
        GUIEventTypeMouse
    } type;
    union {
        struct {
            CCKeyboardMap state;
        } key;
        struct {
            CCMouseEvent event;
            CCMouseMap state;
        } mouse;
    };
} GUIEventInfo, *GUIEvent;

typedef struct GUIObjectInfo *GUIObject;

typedef void *(*GUIObjectConstructorCallback)(CCAllocatorType Allocator);
typedef void (*GUIObjectDestructorCallback)(void *Internal);
typedef void (*GUIObjectRenderCallback)(GUIObject Object, GFXFramebuffer Framebuffer);
typedef void (*GUIObjectEventCallback)(GUIObject Object, GUIEvent Event);
typedef CCRect (*GUIObjectGetRectCallback)(GUIObject Object);
typedef void (*GUIObjectSetRectCallback)(GUIObject Object, CCRect Rect);
typedef _Bool (*GUIObjectGetEnabledCallback)(GUIObject Object);
typedef void (*GUIObjectSetEnabledCallback)(GUIObject Object, _Bool Enabled);
typedef void (*GUIObjectAddChildCallback)(GUIObject Object, GUIObject Child);
typedef void (*GUIObjectRemoveChildCallback)(GUIObject Object, GUIObject Child);
typedef _Bool (*GUIObjectHasChangedCallback)(GUIObject Object); //clear on render
typedef CCExpression (*GUIObjectEvaluatorCallback)(GUIObject Object, CCExpression Expression);
typedef CCExpression (*GUIObjectGetExpressionStateCallback)(GUIObject Object);

typedef struct {
    GUIObjectConstructorCallback create;
    GUIObjectDestructorCallback destroy;
    GUIObjectRenderCallback render;
    GUIObjectEventCallback event;
    struct {
        GUIObjectGetRectCallback get;
        GUIObjectSetRectCallback set;
    } rect;
    struct {
        GUIObjectGetEnabledCallback get;
        GUIObjectSetEnabledCallback set;
    } enabled;
    struct {
        GUIObjectAddChildCallback add;
        GUIObjectRemoveChildCallback remove;
    } child;
    GUIObjectHasChangedCallback changed;
    GUIObjectEvaluatorCallback evaluate;
    GUIObjectGetExpressionStateCallback state;
} GUIObjectInterface;

typedef struct GUIObjectInfo {
    const GUIObjectInterface *interface;
    CCAllocatorType allocator;
    GUIObject parent;
    void *internal;
} GUIObjectInfo;


GUIObject GUIObjectCreate(CCAllocatorType Allocator, const GUIObjectInterface *Interface);
void GUIObjectDestroy(GUIObject Object);
void GUIObjectRender(GUIObject Object, GFXFramebuffer Framebuffer);
void GUIObjectEvent(GUIObject Object, GUIEvent Event);
CCRect GUIObjectGetRect(GUIObject Object);
void GUIObjectSetRect(GUIObject Object, CCRect Rect);
_Bool GUIObjectGetEnabled(GUIObject Object);
void GUIObjectSetEnabled(GUIObject Object, _Bool Enabled);
GUIObject GUIObjectGetParent(GUIObject Object);
void GUIObjectAddChild(GUIObject Object, GUIObject Child);
void GUIObjectRemoveChild(GUIObject Object, GUIObject Child);
_Bool GUIObjectHasChanged(GUIObject Object);
CCExpression GUIObjectEvaluateExpression(GUIObject Object, CCExpression Expression);
CCExpression GUIObjectGetExpressionState(GUIObject Object);

#endif
