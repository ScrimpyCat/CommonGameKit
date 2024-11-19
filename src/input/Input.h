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
    CCInputMapType type;
    union {
        struct {
            struct CCInputMap *inputMaps;
            size_t count;
            _Bool wantsAllActive;
        } group;
        struct {
            CCKeyboardKeycode keycode;
            CCChar character;
            CCKeyboardModifier flags;
            float ramp;
            _Bool isKeycode, ignoreModifier, repeats;
        } keyboard;
        union {
            struct {
                CCMouseButton code;
                CCKeyboardModifier flags;
                float ramp;
                _Bool ignoreModifier;
            } button;
        } mouse;
        struct {
            CCString device;
            int8_t connection;
            union {
                struct {
                    float deadzone;
                    uint8_t resolution;
                    CCControllerAxisMap x, y, z, w;
                } axis;
                struct {
                    int32_t index;
                    float ramp;
                } button, hat;
            };
        } controller;
        union {
            struct {
                struct CCInputMap *inputMaps;
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
