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

#ifndef CommonGameKit_Controller_h
#define CommonGameKit_Controller_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/Component.h>

typedef enum {
    CCControllerHatCenter,
    CCControllerHatUp = (1 << 0),
    CCControllerHatRight = (1 << 1),
    CCControllerHatDown = (1 << 2),
    CCControllerHatLeft = (1 << 3),
    CCControllerHatRightUp = CCControllerHatRight | CCControllerHatUp,
    CCControllerHatRightDown = CCControllerHatRight | CCControllerHatDown,
    CCControllerHatLeftUp = CCControllerHatLeft | CCControllerHatUp,
    CCControllerHatLeftDown = CCControllerHatLeft | CCControllerHatDown
} CCControllerHat;

typedef enum {
    CCControllerEventTypeConnection,
    CCControllerEventTypeAxis,
    CCControllerEventTypeButton,
    CCControllerEventTypeHat
} CCControllerEventType;

typedef struct {
    int32_t index;
    _Bool flip;
    _Bool oneSided;
} CCControllerAxisMap;

typedef struct {
    CCString name;
    uint64_t timestamp;
    CCArray(float) axes;
    CCArray(_Bool) buttons;
    CCArray(CCControllerHat) hats;
    _Bool connected;
} CCController;

typedef struct {
    size_t index;
    float value;
} CCControllerAxisEvent;

typedef struct {
    size_t index;
    _Bool down;
} CCControllerButtonEvent;

typedef struct {
    size_t index;
    CCControllerHat position;
} CCControllerHatEvent;

typedef struct {
    CCControllerEventType type;
    uint64_t timestamp;
    size_t index;
    union {
        CCControllerAxisEvent axis;
        CCControllerButtonEvent button;
        CCControllerHatEvent hat;
        _Bool connected;
    };
} CCControllerEvent;

/*!
 * @brief A callback to get the device name for the controller.
 * @param Index The index to identify the controller by.
 * @return The name of the device. Ownership of this string should be returned to the caller.
 */
typedef CC_NEW CCString (*CCControllerGetNameCallback)(size_t Index);

/*!
 * @brief A callback to update the axes of a controller.
 * @param Index The index to identify the controller by.
 * @param Axes The list of controller axes. Axes must be of type @b float. The array will either be
 *        empty or populated with the previous axes' values.
 */
typedef void (*CCControllerUpdateAxesCallback)(size_t Index, CCArray(float) Axes);

/*!
 * @brief A callback to update the buttons of a controller.
 * @param Index The index to identify the controller by.
 * @param Buttons The list of controller buttons. Buttons must be of type @b _Bool, where TRUE represents
 *        a down state and FALSE represents an up state. The array will either be empty or populated with
 *        the previous buttons' values.
 */
typedef void (*CCControllerUpdateButtonsCallback)(size_t Index, CCArray(_Bool) Buttons);

/*!
 * @brief A callback to update the hats of a controller.
 * @param Index The index to identify the controller by.
 * @param Hats The list of controller hats. Hats must be of type @b CCControllerHat. The array will either
 *        be empty or populated with the previous hats' values.
 */
typedef void (*CCControllerUpdateHatsCallback)(size_t Index, CCArray(CCControllerHat) Hats);

/*!
 * @brief How much movement of the analog input is needed to trigger an event.
 * @description The value should be between 0.0 to 1.0, ranging from the smallest amount of movement to the
 *              most before triggering an event.
 */
extern float CCControllerAxisEventSensitivity;

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
 * @param Hats The callback to retrieve the hats' state for the controller.
 */
void CCControllerSetup(size_t Count, CCControllerGetNameCallback Name, CCControllerUpdateAxesCallback Axes, CCControllerUpdateButtonsCallback Buttons, CCControllerUpdateHatsCallback Hats);

/*!
 * @brief Indicate that a controller has been connected/disconnected.
 * @param Index The index to identify the controller by.
 * @param Connected Whether the controller was connected (TRUE), or disconnected (FALSE).
 */
void CCControllerConnect(size_t Index, _Bool Connected);

/*!
 * @brief Update the state of the controllers with non-zero based events.
 * @description The more frequently this is polled, the more accurate the current state will be. Events are
 *              added for any non-zero/non-neutral state.
 */
void CCControllerUpdateState(void);

/*!
 * @brief Update the state of the controllers with diff based events.
 * @description The more frequently this is polled, the more accurate the current state will be. Events are
 *              added whenever they differ from the previous state.
 */
void CCControllerDiffUpdateState(void);

/*!
 * @brief Sync the current controller state with the latest recorded state.
 * @note This should only be called from one thread before any other thread checks the controller state.
 */
void CCControllerSync(void);

/*!
 * @brief Clears the local controller cache.
 * @note After a sync has been performed this must be called by any threads that will access controller state.
 */
void CCControllerClearCache(void);

/*!
 * @brief Get the controller state.
 * @note This should only be used after another thread has synced the controller state, after which the controller
 *       state can be used up until a thread calls @b CCControllerSync again.
 *
 * @param Index The connection index for the controller.
 * @return The controller state at that connection, or NULL if there is none.
 */
const CCController *CCControllerGetController(size_t Index);

/*!
 * @brief Check whether the controller has the provided device name.
 * @param State The controller state.
 * @param Device The device name to check.
 * @return Whether the controller does have the device name (TRUE), or not (FALSE).
 */
_Bool CCControllerIsDevice(const CCController *State, CCString Device);

/*!
 * @brief Get the controller joystick state for 1D movement.
 * @param State The controller state.
 * @param Resolution The precision of the analog input. This is useful for smoothing out the values, so smaller
 *                   movements end up being ignored.

 * @param Deadzone The deadzone region of the joystick. The deadzone is the region of the joystick that will be
 *                 mapped to 0. This is useful to alleviate any noise due to the analog input.
 *
 * @param X How the x-axis of the joystick should be mapped.
 * @return The joystick state.
 */
float CCControllerGetStateForJoystick1D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X);

/*!
 * @brief Get the controller joystick state for 1D movement.
 * @param State The controller state.
 * @param Resolution The precision of the analog input. This is useful for smoothing out the values, so smaller
 *                   movements end up being ignored.

 * @param Deadzone The deadzone region of the joystick. The deadzone is the region of the joystick that will be
 *                 mapped to 0. This is useful to alleviate any noise due to the analog input.
 *
 * @param X How the x-axis of the joystick should be mapped.
 * @param Y How the y-axis of the joystick should be mapped.
 * @return The joystick state.
 */
CCVector2D CCControllerGetStateForJoystick2D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y);

/*!
 * @brief Get the controller joystick state for 1D movement.
 * @param State The controller state.
 * @param Resolution The precision of the analog input. This is useful for smoothing out the values, so smaller
 *                   movements end up being ignored.

 * @param Deadzone The deadzone region of the joystick. The deadzone is the region of the joystick that will be
 *                 mapped to 0. This is useful to alleviate any noise due to the analog input.
 *
 * @param X How the x-axis of the joystick should be mapped.
 * @param Y How the y-axis of the joystick should be mapped.
 * @param Z How the z-axis of the joystick should be mapped.
 * @return The joystick state.
 */
CCVector3D CCControllerGetStateForJoystick3D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y, CCControllerAxisMap Z);

/*!
 * @brief Get the controller joystick state for 1D movement.
 * @param State The controller state.
 * @param Resolution The precision of the analog input. This is useful for smoothing out the values, so smaller
 *                   movements end up being ignored.

 * @param Deadzone The deadzone region of the joystick. The deadzone is the region of the joystick that will be
 *                 mapped to 0. This is useful to alleviate any noise due to the analog input.
 *
 * @param X How the x-axis of the joystick should be mapped.
 * @param Y How the y-axis of the joystick should be mapped.
 * @param Z How the z-axis of the joystick should be mapped.
 * @param W How the w-axis of the joystick should be mapped.
 * @return The joystick state.
 */
CCVector4D CCControllerGetStateForJoystick4D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y, CCControllerAxisMap Z, CCControllerAxisMap W);

/*!
 * @brief Get the controller button state.
 * @param State The controller state.
 * @param Button The button to get the state of. Use -1 to reference no button.
 * @return Whether the button is active (TRUE), or inactive (FALSE). If button does not exist FALSE is returned.
 */
_Bool CCControllerGetStateForButton(const CCController *State, int32_t Button);

/*!
 * @brief Get the controller hat state.
 * @param State The controller state.
 * @param Hat The hat to get the state of. Use -1 to reference no hat.
 * @return The state of the controller hat. If button does not exist @b CCControllerHatCenter is returned.
 */
CCControllerHat CCControllerGetStateForHat(const CCController *State, int32_t Hat);


#pragma mark - Template Types

#define CC_TYPE_CCControllerAxisMap(...) CCControllerAxisMap
#define CC_TYPE_0_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_1_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_2_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_3_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_4_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_5_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_6_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_7_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_8_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,
#define CC_TYPE_9_CCControllerAxisMap CC_TYPE_CCControllerAxisMap,

#define CC_PRESERVE_CC_TYPE_CCControllerAxisMap CC_TYPE_CCControllerAxisMap

#define CC_TYPE_DECL_CCControllerAxisMap(...) CCControllerAxisMap, __VA_ARGS__
#define CC_TYPE_DECL_0_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_1_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_2_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_3_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_4_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_5_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_6_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_7_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_8_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,
#define CC_TYPE_DECL_9_CCControllerAxisMap CC_TYPE_DECL_CCControllerAxisMap,

#define CC_MANGLE_TYPE_0_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_1_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_2_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_3_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_4_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_5_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_6_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_7_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_8_CCControllerAxisMap CCControllerAxisMap
#define CC_MANGLE_TYPE_9_CCControllerAxisMap CCControllerAxisMap


#define CC_TYPE_CCControllerEvent(...) CCControllerEvent
#define CC_TYPE_0_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_1_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_2_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_3_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_4_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_5_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_6_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_7_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_8_CCControllerEvent CC_TYPE_CCControllerEvent,
#define CC_TYPE_9_CCControllerEvent CC_TYPE_CCControllerEvent,

#define CC_PRESERVE_CC_TYPE_CCControllerEvent CC_TYPE_CCControllerEvent

#define CC_TYPE_DECL_CCControllerEvent(...) CCControllerEvent, __VA_ARGS__
#define CC_TYPE_DECL_0_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_1_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_2_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_3_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_4_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_5_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_6_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_7_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_8_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,
#define CC_TYPE_DECL_9_CCControllerEvent CC_TYPE_DECL_CCControllerEvent,

#define CC_MANGLE_TYPE_0_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_1_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_2_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_3_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_4_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_5_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_6_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_7_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_8_CCControllerEvent CCControllerEvent
#define CC_MANGLE_TYPE_9_CCControllerEvent CCControllerEvent


#pragma mark - Reflection

extern const CCReflectStruct3 CC_REFLECT(CCControllerAxisMap);


#pragma mark - Events

#ifndef CC_CONTROLLER_EVENT_MAX
#define CC_CONTROLLER_EVENT_MAX 1024
#endif

#define Titem CCControllerEvent
#define Tmax CC_CONTROLLER_EVENT_MAX
#include <CommonC/ConcurrentCircularBuffer.h>

extern CCConcurrentCircularBuffer(CCControllerEvent, CC_CONTROLLER_EVENT_MAX) CCControllerInputEventBuffer;


#endif
