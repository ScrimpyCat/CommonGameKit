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

#ifndef CommonGameKit_GUIObject_h
#define CommonGameKit_GUIObject_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/GFX.h>
#include <CommonGameKit/InputSystem.h>
#include <CommonGameKit/Expression.h>
#include <threads.h>

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

/*!
 * @brief A GUI object.
 */
typedef struct GUIObjectInfo *GUIObject;

typedef void *(*GUIObjectConstructorCallback)(CCAllocatorType Allocator);
typedef void (*GUIObjectDestructorCallback)(void *Internal);
typedef void (*GUIObjectRenderCallback)(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index, GFXBuffer Projection);
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

typedef enum {
    GUIObjectCacheStrategyMask = 0xffff,
    GUIObjectCacheStrategyNone = 0,
    GUIObjectCacheStrategyInvalidateOnEvent = (1 << 0),
    GUIObjectCacheStrategyInvalidateOnResize = (1 << 1),
    GUIObjectCacheStrategyInvalidateOnMove = (1 << 2),
    
    GUIObjectCacheDirtyMask = (0xffff << 16),
    GUIObjectCacheDirtyIndex = 16,
    GUIObjectCacheDirtyEvent = (GUIObjectCacheStrategyInvalidateOnEvent << 16),
    GUIObjectCacheDirtyResize = (GUIObjectCacheStrategyInvalidateOnResize << 16),
    GUIObjectCacheDirtyMove = (GUIObjectCacheStrategyInvalidateOnMove << 16),
} GUIObjectCache;

typedef struct GUIObjectInfo {
    const GUIObjectInterface *interface;
    CCAllocatorType allocator;
    GUIObject parent;
    mtx_t lock; //Temporary until CCExpression becomes threadsafe and can handle it better/more efficiently
    void *internal;
    struct {
        GUIObjectCache strategy;
        CCRect rect;
        GFXFramebuffer store;
        _Bool changed;
    } cache;
} GUIObjectInfo;


/*!
 * @brief Create a GUI object.
 * @description Typically do not use this function directly, but instead a creator provided
 *              by an impementation.
 *
 * @param Allocator The allocator to be used.
 * @param Interface The interface to the GUI element's implementation.
 * @return The GUI object. Must be destroyed.
 */
CC_NEW GUIObject GUIObjectCreate(CCAllocatorType Allocator, const GUIObjectInterface *Interface);

/*!
 * @brief Destroy a GUI object.
 * @description Destroys all of the child elements. And removes it from the parent element
 *              if applicable.
 *
 * @param Object The GUI object to be destroyed.
 */
void GUIObjectDestroy(GUIObject CC_DESTROY(Object));

/*!
 * @brief Render a GUI object.
 * @description Renders all of the child elements.
 * @param Object The GUI object to be rendered.
 * @param Framebuffer The framebuffer to be used as the rendering destination.
 * @param Index The framebuffer attachment to be used as the target.
 * @param Projection The projection matrix.
 */
void GUIObjectRender(GUIObject Object, GFXFramebuffer Framebuffer, size_t Index, GFXBuffer Projection);

/*!
 * @brief Handle an event.
 * @description Passes an event to the element and all child elements.
 * @param Object The GUI object to handle the event.
 * @param Event The event to be handled.
 */
void GUIObjectEvent(GUIObject Object, GUIEvent Event);

/*!
 * @brief Get the rect bounds of a GUI object.
 * @param Object The GUI object to get the bounds of.
 * @return The rect bounds of the object.
 */
CCRect GUIObjectGetRect(GUIObject Object);

/*!
 * @brief Set the rect bounds of a GUI object.
 * @param Object The GUI object to set the bounds of.
 * @param Rect The new rect.
 */
void GUIObjectSetRect(GUIObject Object, CCRect Rect);

/*!
 * @brief Check if a GUI object is enabled.
 * @param Object The GUI object to check if it's enabled.
 * @return TRUE if it is enabled, FALSE if it is not enabled.
 */
_Bool GUIObjectGetEnabled(GUIObject Object);

/*!
 * @brief Set whether the GUI object is enabled.
 * @param Object The GUI object to set whether its enabled.
 * @param Enabled The enabled flag.
 */
void GUIObjectSetEnabled(GUIObject Object, _Bool Enabled);

/*!
 * @brief Get the cache strategy used by the GUI object.
 * @param Object The GUI object to get the cache strategy of.
 * @return The cache strategy.
 */
GUIObjectCache GUIObjectGetCacheStrategy(GUIObject Object);

/*!
 * @brief Set cache strategy of the GUI object.
 * @param Object The GUI object to set the cache strategy of.
 * @param Strategy The cache strategy.
 */
void GUIObjectSetCacheStrategy(GUIObject Object, GUIObjectCache Strategy);

/*!
 * @brief Get the parent of the GUI object.
 * @param Object The GUI object to get the parent of.
 * @return The parent element, or NULL if there is none.
 */
GUIObject GUIObjectGetParent(GUIObject Object);

/*!
 * @brief Add a child element to the GUI object.
 * @param Object The GUI object to be the parent.
 * @param Child The GUI object to be the child.
 */
void GUIObjectAddChild(GUIObject Object, GUIObject Child);

/*!
 * @brief Remove a child element from a GUI object.
 * @param Object The GUI object that is the parent.
 * @param Child The GUI object to be removed.
 */
void GUIObjectRemoveChild(GUIObject Object, GUIObject Child);

/*!
 * @brief Check if a GUI object has changed.
 * @description Changed means something has visually changed in the GUI. So requires to be
 *              rendered again. Rendering the GUI will reset this state.
 *
 * @param Object The GUI object to check whether it has changed.
 * @return TRUE if the object has changed/is visually different, FALSE if it has not changed/has
 *         the same appearance as the last time it was rendered.
 */
_Bool GUIObjectHasChanged(GUIObject Object);

/*!
 * @brief Evaluate an expression inside a GUI object.
 * @description The expression is evaluated in the context of the object.
 * @param Object The GUI object to evaluate the expression.
 * @param Expression The expression to be evaluated.
 * @return The resulting expression.
 */
CCExpression GUIObjectEvaluateExpression(GUIObject Object, CCExpression Expression);

/*!
 * @brief Get the expression state of a GUI object.
 * @warning Returned state should only be used during a GUIManager lock.
 * @param Object The GUI object to get the state from.
 * @return The resulting expression state.
 */
CCExpression GUIObjectGetExpressionState(GUIObject Object);

#endif
