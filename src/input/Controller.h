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

#ifndef Blob_Game_Controller_h
#define Blob_Game_Controller_h

#include "GLSetup.h"
#include "Component.h"
#include <CommonC/Common.h>

typedef enum {
    CCControllerEventAxes,
    CCControllerEventButton
} CCControllerEvent;

typedef struct {
    double timestamp;
    _Bool down;
} CCControllerButtonState;

typedef struct {
    CCVector3D position;
} CCControllerAxesState;

typedef union {
    CCControllerAxesState axes;
    CCControllerButtonState button;
} CCControllerState;

/*!
 * @brief A callback to get the device name for the controller.
 * @param Index The index to identify the controller by.
 * @return The name of the device. Ownership of this string should be returned to the caller.
 */
typedef CC_NEW CCString (*CCControllerGetNameCallback)(size_t Index);

/*!
 * @brief A callback to update the axes of a controller.
 * @param Index The index to identify the controller by.
 * @param Axes The list of controller axes. Axes must be of type @b float. The collection will either be
 *        empty or populated with the previous axes' values.
 */
typedef void (*CCControllerUpdateAxesCallback)(size_t Index, CCOrderedCollection Axes);

/*!
 * @brief A callback to update the buttons of a controller.
 * @param Index The index to identify the controller by.
 * @param Buttons The list of controller buttons. Buttons must be of type @b _Bool, where TRUE represents
 *        a down state and FALSE represents an up state. The collection will either be empty or populated
 *        with the previous buttons' values.
 */
typedef void (*CCControllerUpdateButtonsCallback)(size_t Index, CCOrderedCollection Buttons);

/*!
 * @brief Setup the controller manager.
 * @warning This must be called before any other controller related function.
 * @param Count The maximum number of controllers that may be connected at any one time. Controllers
 *        should be identifiable using a unique index from 0 to (Count - 1). Once a controller has
 *        been disconnected, it's unique identifying index can be reused by a new controller
 *        connection.
 *
 * @param Name The callback to retrieve the name for the controller.
 * @param Axes The callback to retrieve the axes' state for the controller.
 * @param Buttons The callback to retrieve the buttons' state for the controller.
 */
void CCControllerSetup(size_t Count, CCControllerGetNameCallback Name, CCControllerUpdateAxesCallback Axes, CCControllerUpdateButtonsCallback Buttons);

/*!
 * @brief Indicate that a controller has been connected/disconnected.
 * @param Index The index to identify the controller by.
 * @param Connected Whether the controller was connected (TRUE), or disconnected (FALSE).
 */
void CCControllerConnect(size_t Index, _Bool Connected);

/*!
 * @brief Update the state of the controllers.
 * @description The more frequently this is polled, the more accurate the current state will be.
 */
void CCControllerUpdateState(void);

/*!
 * @brief Get the controller state for the given component.
 * @param Component The input map controller component.
 * @return The controller state for the given component, or empty state if there is none available.
 */
CCControllerState CCControllerGetStateForComponent(CCComponent Component);

#endif
