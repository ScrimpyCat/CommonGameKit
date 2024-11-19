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

#ifndef CommonGameKit_Mouse_h
#define CommonGameKit_Mouse_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/Keyboard.h>

/// The button input for the mouse input event. This is compatible with GLFW mouse buttons.
typedef enum {
    CCMouseButton1 =        0,
    CCMouseButton2 =        1,
    CCMouseButton3 =        2,
    CCMouseButton4 =        3,
    CCMouseButton5 =        4,
    CCMouseButton6 =        5,
    CCMouseButton7 =        6,
    CCMouseButton8 =        7,
    
    CCMouseButtonCount,
    
    CCMouseButtonLeft =     CCMouseButton1,
    CCMouseButtonRight =    CCMouseButton2,
    CCMouseButtonMiddle =   CCMouseButton3,
    CCMouseButtonUnknown = -1
} CCMouseButton;

typedef enum {
    CCMouseEventTypeMove,
    CCMouseEventTypeButton,
    CCMouseEventTypeScroll,
    CCMouseEventTypeDrop,
    CCMouseEventTypeEntered
} CCMouseEventType;

typedef struct {
    uint64_t timestamp;
    _Bool down : 1;
} CC_PACKED CCMouseButtonState;

typedef struct {
    CCMouseButton button : 4;
    CCKeyboardModifier flags : 4;
    CCMouseButtonState state;
} CC_PACKED CCMouseButtonEvent;

_Static_assert(sizeof(CCMouseButtonEvent) <= 16, "Exceeds 16 bytes");

typedef struct {
    CCVector2D delta;
} CCMouseMoveEvent, CCMouseScrollEvent;

typedef struct {
    CCCollection(CCString) files;
} CCMouseDropItemsEvent;

typedef struct {
    CCMouseEventType type;
    uint64_t timestamp;
    CCVector2D position;
    union {
        CCMouseMoveEvent move;
        CCMouseButtonEvent button;
        CCMouseScrollEvent scroll;
        CCMouseDropItemsEvent drop;
        _Bool entered;
    };
} CCMouseEvent;

/*!
 * @brief Register a mouse drop event.
 * @param Files The list of files dropped.
 */
void CCMouseDropInput(CCCollection(CCString) Files);

/*!
 * @brief Register a mouse scroll event.
 * @param ScrollDelta The scroll delta.
 */
void CCMouseScrollInput(CCVector2D ScrollDelta);

/*!
 * @brief Register a mouse button event.
 * @param Button The mouse button the event belongs to.
 * @param Action The action of the event.
 * @param Mods The modifiers applied.
 */
void CCMouseButtonInput(CCMouseButton Button, CCKeyboardAction Action, CCKeyboardModifier Mods);

/*!
 * @brief Register a mouse move event.
 * @param Position The position of the mouse. Note: This should be in the coordinate space where
 *        0,0 = bottom left corner of the window.
 */
void CCMousePositionInput(CCVector2D Position);

/*!
 * @brief Register a mouse has entered/exited the window.
 * @param Entered TRUE if the mouse has entered the window, FALSE if it has exited.
 */
void CCMouseEnterInput(_Bool Entered);

/*!
 * @brief Get the mouse button state for the given button.
 * @param Button The button to lookup.
 * @param Modifier The modifier to match. Ignored if @b IgnoreModifier is TRUE.
 * @param IgnoreModifier Whether the modifier should be matched or not.
 * @return The mouse button state that matches the given button, or empty state if there is none available.
 */
CCMouseButtonState CCMouseButtonGetStateForButton(CCMouseButton Button, CCKeyboardModifier Modifier, _Bool IgnoreModifier);


#pragma mark - Template Types

#define CC_TYPE_CCMouseEvent(...) CCMouseEvent
#define CC_TYPE_0_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_1_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_2_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_3_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_4_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_5_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_6_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_7_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_8_CCMouseEvent CC_TYPE_CCMouseEvent,
#define CC_TYPE_9_CCMouseEvent CC_TYPE_CCMouseEvent,

#define CC_PRESERVE_CC_TYPE_CCMouseEvent CC_TYPE_CCMouseEvent

#define CC_TYPE_DECL_CCMouseEvent(...) CCMouseEvent, __VA_ARGS__
#define CC_TYPE_DECL_0_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_1_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_2_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_3_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_4_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_5_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_6_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_7_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_8_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,
#define CC_TYPE_DECL_9_CCMouseEvent CC_TYPE_DECL_CCMouseEvent,

#define CC_MANGLE_TYPE_0_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_1_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_2_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_3_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_4_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_5_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_6_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_7_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_8_CCMouseEvent CCMouseEvent
#define CC_MANGLE_TYPE_9_CCMouseEvent CCMouseEvent


#define CC_TYPE_CCMouseButton(...) CCMouseButton
#define CC_TYPE_0_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_1_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_2_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_3_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_4_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_5_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_6_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_7_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_8_CCMouseButton CC_TYPE_CCMouseButton,
#define CC_TYPE_9_CCMouseButton CC_TYPE_CCMouseButton,

#define CC_PRESERVE_CC_TYPE_CCMouseButton CC_TYPE_CCMouseButton

#define CC_TYPE_DECL_CCMouseButton(...) CCMouseButton, __VA_ARGS__
#define CC_TYPE_DECL_0_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_1_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_2_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_3_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_4_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_5_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_6_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_7_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_8_CCMouseButton CC_TYPE_DECL_CCMouseButton,
#define CC_TYPE_DECL_9_CCMouseButton CC_TYPE_DECL_CCMouseButton,

#define CC_MANGLE_TYPE_0_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_1_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_2_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_3_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_4_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_5_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_6_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_7_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_8_CCMouseButton CCMouseButton
#define CC_MANGLE_TYPE_9_CCMouseButton CCMouseButton


#pragma mark - Reflection

extern const CCReflectInteger CC_REFLECT(CCMouseButton);


#pragma mark - Events

#ifndef CC_MOUSE_EVENT_MAX
#define CC_MOUSE_EVENT_MAX 1024
#endif

#define Titem CCMouseEvent
#define Tmax CC_MOUSE_EVENT_MAX
#include <CommonC/ConcurrentCircularBuffer.h>

extern CCConcurrentCircularBuffer(CCMouseEvent, CC_MOUSE_EVENT_MAX) CCMouseInputEventBuffer;

#endif
