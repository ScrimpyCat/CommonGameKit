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
    CCMouseEventMove,
    CCMouseEventButton,
    CCMouseEventScroll,
    CCMouseEventDrop,
    CCMouseEventEntered
} CCMouseEvent;

typedef struct {
    uint64_t timestamp;
    _Bool down : 1;
} CC_PACKED CCMouseButtonState;

typedef struct {
    CCMouseButton button : 4;
    CCKeyboardModifier flags : 4;
    CCMouseButtonState state;
} CC_PACKED CCMouseButtonMap;

_Static_assert(sizeof(CCMouseButtonMap) <= 16, "Exceeds 16 bytes");

typedef struct {
    CCVector2D delta;
} CCMouseMoveMap, CCMouseScrollMap;

typedef struct {
    CCCollection(CCString) files;
} CCMouseDropItemsMap;

typedef struct {
    CCMouseEvent event;
    uint64_t timestamp;
    CCVector2D position;
    union {
        CCMouseMoveMap move;
        CCMouseButtonMap button;
        CCMouseScrollMap scroll;
        CCMouseDropItemsMap drop;
        _Bool entered;
    };
} CCMouseMap;

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


#pragma mark - Events

#define CC_TYPE_CCMouseMap(...) CCMouseMap
#define CC_TYPE_0_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_1_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_2_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_3_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_4_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_5_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_6_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_7_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_8_CCMouseMap CC_TYPE_CCMouseMap,
#define CC_TYPE_9_CCMouseMap CC_TYPE_CCMouseMap,

#define CC_PRESERVE_CC_TYPE_CCMouseMap CC_TYPE_CCMouseMap

#define CC_TYPE_DECL_CCMouseMap(...) CCMouseMap, __VA_ARGS__
#define CC_TYPE_DECL_0_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_1_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_2_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_3_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_4_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_5_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_6_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_7_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_8_CCMouseMap CC_TYPE_DECL_CCMouseMap,
#define CC_TYPE_DECL_9_CCMouseMap CC_TYPE_DECL_CCMouseMap,

#define CC_MANGLE_TYPE_0_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_1_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_2_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_3_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_4_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_5_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_6_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_7_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_8_CCMouseMap CCMouseMap
#define CC_MANGLE_TYPE_9_CCMouseMap CCMouseMap

#ifndef CC_MOUSE_EVENT_MAX
#define CC_MOUSE_EVENT_MAX 1024
#endif

#define Titem CCMouseMap
#define Tmax CC_MOUSE_EVENT_MAX
#include <CommonC/ConcurrentCircularBuffer.h>

extern CCConcurrentCircularBuffer(CCMouseMap, CC_MOUSE_EVENT_MAX) CCMouseInputEventBuffer;

#endif
