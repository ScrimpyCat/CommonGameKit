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

#ifndef CommonGameKit_InputSystem_h
#define CommonGameKit_InputSystem_h

#include "ComponentSystem.h"
#include "Entity.h"
#include "Keyboard.h"
#include "Mouse.h"

//Note: Currently just a demo system

#define CC_INPUT_SYSTEM_ID 21
#define CC_INPUT_COMPONENT_FLAG 0x2000000

typedef enum {
    CCInputMapTypeMask = 0xff,
    CCInputMapTypeNone = 1,
    CCInputMapTypeKeyboard,
    CCInputMapTypeMousePosition,
    CCInputMapTypeMouseButton,
    CCInputMapTypeMouseScroll,
    CCInputMapTypeMouseDrop,
    CCInputMapTypeController,
    CCInputMapTypeControllerAxes,
    CCInputMapTypeControllerButton,
    CCInputMapTypeGroup
} CCInputMapType;

typedef enum {
    CCInputStateInactive,
    CCInputStateActive
} CCInputState;

typedef void (*CCInputMapKeyboardCallback)(CCComponent Component, CCKeyboardMap State);
typedef void (*CCInputMapMouseCallback)(CCComponent Component, CCMouseEvent Event, CCMouseMap State);

void CCInputSystemRegister(void);
void CCInputSystemDeregister(void);

/*!
 * @brief Set whether the window is in focus.
 * @param Focus TRUE if the window has become focused, or FALSE if it is no longer focused.
 */
void CCInputSystemWindowFocus(_Bool Focus);

/*!
 * @brief Get the input components registered by the system for a certain type.
 * @param InputType The type of components to retrieve.
 * @return The list of components. Note: This collection must be destroyed.
 */
CCCollection CCInputSystemGetComponents(CCInputMapType InputType);

/*!
 * @brief Gets the state of the action belonging to the entity.
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return CCInputStateActive if the action is active, or CCInputStateInactive if it is not or does not exist.
 */
CCInputState CCInputSystemGetStateForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action (0.0 - 1.0), where 0.0 is no pressure and 1.0
 *         is the maximum amount of pressure allowed.
 */
float CCInputSystemGetPressureForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the 2 axis pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 *             And for inputs without 2 axis, they can be simulated by placing them inside an input group
 *             component. If the group contains 2 components then they'll be assigned in order (0:x, 1:y),
 *             while if it contains 4 components then they'll be assigned (0:-x to 2:+x, 1:-y to 3:+y).
 *
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action it wil either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCVector2D CCInputSystemGetPressure2ForAction(CCEntity Entity, const char *Action);

/*!
 * @brief Gets the 3 axis pressure of the action belonging to the entity.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 *             And for inputs without 3 axis, they can be simulated by placing them inside an input group
 *             component. If the group contains 3 components then they'll be assigned in order (0:x, 1:y, 2:z),
 *             while if it contains 6 components then they'll be assigned (0:-x to 3:+x, 1:-y to 4:+y, 2:-z to 5:+z).
 *
 * @param Entity The entity that has the component attached to it.
 * @param Action The action to look for.
 * @return The amount of pressure currently applied to the action it wil either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCVector3D CCInputSystemGetPressure3ForAction(CCEntity Entity, const char *Action);

#endif
