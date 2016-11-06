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

#include <CommonGameKit/Component.h>
#include <CommonC/Common.h>
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
    CCMouseEventDrop
} CCMouseEvent;

typedef struct {
    double timestamp;
    _Bool down;
} CCMouseButtonState;

typedef struct {
    CCMouseButton button;
    CCKeyboardModifier flags;
    CCMouseButtonState state;
} CCMouseButtonMap;

typedef struct {
    CCVector2D delta;
} CCMouseMoveMap, CCMouseScrollMap;

typedef struct {
    CCCollection files;
} CCMouseDropItemsMap;

typedef struct {
    CCVector2D position;
    union {
        CCMouseMoveMap move;
        CCMouseButtonMap button;
        CCMouseScrollMap scroll;
        CCMouseDropItemsMap drop;
    };
} CCMouseMap;

/*!
 * @brief Register a mouse drop event.
 * @param Files The list of files dropped.
 */
void CCMouseDropInput(CCCollection Files);

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
 * @brief Get the mouse button state for the given component.
 * @param Component The input map mouse button component.
 * @return The mouse button state for the given component, or empty state if there is none available.
 */
CCMouseButtonState CCMouseButtonGetStateForComponent(CCComponent Component);

#endif
