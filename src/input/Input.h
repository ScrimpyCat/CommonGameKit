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

#ifndef CommonGameKit_Input_h
#define CommonGameKit_Input_h

#include <CommonGameKit/Keyboard.h>
#include <CommonGameKit/Mouse.h>
#include <CommonGameKit/Controller.h>

typedef enum {
    CCInputMapTypeNone,
    CCInputMapTypeGroup,
    CCInputMapTypeKeyboard,
    CCInputMapTypeMousePosition,
    CCInputMapTypeMouseButton,
    CCInputMapTypeMouseScroll,
    CCInputMapTypeMouseDrop,
    CCInputMapTypeControllerAxis,
    CCInputMapTypeControllerButton,
    CCInputMapTypeControllerHat,
    CCInputMapTypeVirtualJoystick,
    CCInputMapTypeCount
} CCInputMapType;

typedef struct CCInputMap {
    /// The type of input to map.
    CCInputMapType type;
    union {
        struct {
            /// The input maps that are in the group.
            struct CCInputMap *inputMaps;
            /// The number of input maps in the group.
            size_t count;
            /// Whether all input maps in the group need to be active for the group to be considered active.
            _Bool wantsAllActive;
        } group;
        struct {
            /// The keyboard keycode to match to apply this mapping.
            CCKeyboardKeycode keycode;
            /// The keyboard character to match to apply this mapping. If 0 then it does not require matching a character.
            CCChar character;
            /// The keyboard modifiers to match to apply this mapping.
            CCKeyboardModifier flags;
            /// The ramping affects the speed of the build up/falloff per second.
            float ramp;
            /// Whether to use a keycode match or fallback to character match.
            _Bool isKeycode;
            /// Whether to ignore the modifier flag when matching.
            _Bool ignoreModifier;
            /// Whether to match repeats. Repeated key presses are when a key is being held down
            _Bool repeats;
        } keyboard;
        union {
            struct {
                /// The mouse button to match to apply this mapping.
                CCMouseButton code;
                /// The keyboard modifiers to match to apply this mapping.
                CCKeyboardModifier flags;
                /// The ramping affects the speed of the build up/falloff per second.
                float ramp;
                /// Whether to ignore the modifier flag when matching.
                _Bool ignoreModifier;
            } button;
        } mouse;
        struct {
            /// The device name is the assigned name of the controller this component should match with. If null then it maps to any controller at the specified connection.
            CCString device;
            ///  The connection is the fixed index of the controller this mapping applies to.
            int8_t connection;
            union {
                struct {
                    /// The deadzone is the region of the control stick that will be mapped to 0. This is useful to alleviate any noise due to the analog input.
                    float deadzone;
                    /// The resolution is the precision of the analog input. This is useful to smooth out the values, so smaller movements end up being ignored.
                    uint8_t resolution;
                    /// The controller axis mappings.
                    CCControllerAxisMap x, y, z, w;
                } axis;
                struct {
                    /// The ID of the input from the controller to map. A negative index is unmapped/ignored.
                    int32_t index;
                    /// The ramping affects the speed of the build up/falloff per second.
                    float ramp;
                } button, hat;
            };
        } controller;
        union {
            struct {
                /// The input maps that form the virtual joystick. The inputs are mapped accordingly: [+x, -x, +y, -y, +z, -z, +w, -w]
                struct CCInputMap *inputMaps;
                /// The number of input maps that make up the virtual joystick.
                size_t count;
            } joystick;
        } virtual;
    };
} CCInputMap;

typedef struct {
    uint64_t timestamp;
    _Bool active;
} CCInputState;

typedef struct {
    uint64_t timestamp;
    float pressure;
} CCInputPressure1D;

typedef struct {
    uint64_t timestamp;
    CCVector2D pressure;
} CCInputPressure2D;

typedef struct {
    uint64_t timestamp;
    CCVector3D pressure;
} CCInputPressure3D;

typedef struct {
    uint64_t timestamp;
    CCVector4D pressure;
} CCInputPressure4D;


/*!
 * @brief The input map destructor.
 * @description This destroys the data in the input map, but not the pointer itself.
 * @param InputMap The input map to destroy the contents of.
 */
void CCInputMapDestructor(CCInputMap *InputMap);

/*!
 * @brief Sync the input with the input thread.
 * @description This should be called by any thread before it makes any input queries.
 */
void CCInputSync(void);

/*!
 * @brief Set whether the window is in focus.
 * @param Focus TRUE if the window has become focused, or FALSE if it is no longer focused.
 */
void CCInputWindowFocus(_Bool Focus);

/*!
 * @brief Gets the state of an input map.
 * @param InputMap The input map to check the state of.
 * @return The input state. If the input does not exist then it will treat it as if it was inactive.
 */
CCInputState CCInputGetState(const CCInputMap *InputMap);

/*!
 * @brief Gets the 1D pressure of the input map.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param InputMap The input map to get the pressure of.
 * @return The amount of pressure currently applied to the input. It will either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCInputPressure1D CCInputGetPressure1D(const CCInputMap *InputMap);

/*!
 * @brief Gets the 2D pressure of the input map.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param InputMap The input map to get the pressure of.
 * @return The amount of pressure currently applied to the input. It will either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCInputPressure2D CCInputGetPressure2D(const CCInputMap *InputMap);

/*!
 * @brief Gets the 3D pressure of the input map.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param InputMap The input map to get the pressure of.
 * @return The amount of pressure currently applied to the input. It will either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCInputPressure3D CCInputGetPressure3D(const CCInputMap *InputMap);

/*!
 * @brief Gets the 4D pressure of the input map.
 * @discussion For binary inputs, the pressure will be calculated by ramping up relative to the timestamp.
 * @param InputMap The input map to get the pressure of.
 * @return The amount of pressure currently applied to the input. It will either be in the form of (0.0 - 1.0),
 *         or (-1.0 - +1.0), where 0.0 is no pressure and -/+1.0 is the maximum amount of pressure allowed.
 */
CCInputPressure4D CCInputGetPressure4D(const CCInputMap *InputMap);


#pragma mark - Template Types

#define CC_TYPE_CCInputMapType(...) CCInputMapType
#define CC_TYPE_0_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_1_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_2_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_3_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_4_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_5_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_6_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_7_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_8_CCInputMapType CC_TYPE_CCInputMapType,
#define CC_TYPE_9_CCInputMapType CC_TYPE_CCInputMapType,

#define CC_PRESERVE_CC_TYPE_CCInputMapType CC_TYPE_CCInputMapType

#define CC_TYPE_DECL_CCInputMapType(...) CCInputMapType, __VA_ARGS__
#define CC_TYPE_DECL_0_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_1_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_2_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_3_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_4_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_5_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_6_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_7_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_8_CCInputMapType CC_TYPE_DECL_CCInputMapType,
#define CC_TYPE_DECL_9_CCInputMapType CC_TYPE_DECL_CCInputMapType,

#define CC_MANGLE_TYPE_0_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_1_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_2_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_3_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_4_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_5_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_6_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_7_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_8_CCInputMapType CCInputMapType
#define CC_MANGLE_TYPE_9_CCInputMapType CCInputMapType


#define CC_TYPE_CCInputMap(...) CCInputMap
#define CC_TYPE_0_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_1_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_2_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_3_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_4_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_5_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_6_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_7_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_8_CCInputMap CC_TYPE_CCInputMap,
#define CC_TYPE_9_CCInputMap CC_TYPE_CCInputMap,

#define CC_PRESERVE_CC_TYPE_CCInputMap CC_TYPE_CCInputMap

#define CC_TYPE_DECL_CCInputMap(...) CCInputMap, __VA_ARGS__
#define CC_TYPE_DECL_0_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_1_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_2_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_3_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_4_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_5_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_6_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_7_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_8_CCInputMap CC_TYPE_DECL_CCInputMap,
#define CC_TYPE_DECL_9_CCInputMap CC_TYPE_DECL_CCInputMap,

#define CC_MANGLE_TYPE_0_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_1_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_2_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_3_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_4_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_5_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_6_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_7_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_8_CCInputMap CCInputMap
#define CC_MANGLE_TYPE_9_CCInputMap CCInputMap


#pragma mark - Reflection

extern const CCReflectOpaque CC_REFLECT(CCInputMap);

extern const CCReflectInteger CC_REFLECT(CCInputMapType);

#endif
