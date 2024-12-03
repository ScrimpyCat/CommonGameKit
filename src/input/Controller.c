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

#define CC_QUICK_COMPILE
#include "Controller.h"
#include "Window.h"
#include "Callbacks.h"
#include <stdatomic.h>

const CCReflectStruct3 CC_REFLECT(CCControllerAxisMap) = CC_REFLECT_STRUCT(CCControllerAxisMap,
    (index, &CC_REFLECT(int32_t)),
    (flip, &CC_REFLECT(_Bool)),
    (oneSided, &CC_REFLECT(_Bool))
);

#ifndef CC_CONTROLLER_MAX
#define CC_CONTROLLER_MAX 16
#endif

static CCController Controller[CC_CONTROLLER_MAX];

static size_t ControllerCount = 0;


#define CC_TYPE_CCController(...) CCController
#define CC_TYPE_0_CCController CC_TYPE_CCController,
#define CC_TYPE_1_CCController CC_TYPE_CCController,
#define CC_TYPE_2_CCController CC_TYPE_CCController,
#define CC_TYPE_3_CCController CC_TYPE_CCController,
#define CC_TYPE_4_CCController CC_TYPE_CCController,
#define CC_TYPE_5_CCController CC_TYPE_CCController,
#define CC_TYPE_6_CCController CC_TYPE_CCController,
#define CC_TYPE_7_CCController CC_TYPE_CCController,
#define CC_TYPE_8_CCController CC_TYPE_CCController,
#define CC_TYPE_9_CCController CC_TYPE_CCController,

#define CC_PRESERVE_CC_TYPE_CCController CC_TYPE_CCController

#define CC_TYPE_DECL_CCController(...) CCController, __VA_ARGS__
#define CC_TYPE_DECL_0_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_1_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_2_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_3_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_4_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_5_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_6_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_7_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_8_CCController CC_TYPE_DECL_CCController,
#define CC_TYPE_DECL_9_CCController CC_TYPE_DECL_CCController,

#define CC_MANGLE_TYPE_0_CCController CCController
#define CC_MANGLE_TYPE_1_CCController CCController
#define CC_MANGLE_TYPE_2_CCController CCController
#define CC_MANGLE_TYPE_3_CCController CCController
#define CC_MANGLE_TYPE_4_CCController CCController
#define CC_MANGLE_TYPE_5_CCController CCController
#define CC_MANGLE_TYPE_6_CCController CCController
#define CC_MANGLE_TYPE_7_CCController CCController
#define CC_MANGLE_TYPE_8_CCController CCController
#define CC_MANGLE_TYPE_9_CCController CCController

#define Titem CCController
#include <CommonC/ConcurrentSwapBuffer.h>

#define CC_CONTROLLER_BUFFER_INIT(...) CC_CONCURRENT_SWAP_BUFFER_INIT
#define CC_CONTROLLERS_INIT(count) { CC_REPEAT(0, count, CC_CONTROLLER_BUFFER_INIT) }

static CCConcurrentSwapBuffer(CCController) ControllerSwap[CC_CONTROLLER_MAX] = CC_CONTROLLERS_INIT(CC_CONTROLLER_MAX);

CCConcurrentCircularBuffer(CCControllerEvent, CC_CONTROLLER_EVENT_MAX) CCControllerInputEventBuffer = CC_CONCURRENT_CIRCULAR_BUFFER_INIT;

static CCControllerGetNameCallback ControllerGetName = NULL;
static CCControllerUpdateAxesCallback ControllerUpdateAxes = NULL;
static CCControllerUpdateButtonsCallback ControllerUpdateButtons = NULL;
static CCControllerUpdateHatsCallback ControllerUpdateHats = NULL;

void CCControllerSetup(size_t Count, CCControllerGetNameCallback Name, CCControllerUpdateAxesCallback Axes, CCControllerUpdateButtonsCallback Buttons, CCControllerUpdateHatsCallback Hats)
{
    if (Count > CC_CONTROLLER_MAX)
    {
        CC_LOG_WARNING("Controller count exceeds maximum number of supported controllers. Consider increasing the limit.");
        
        Count = CC_CONTROLLER_MAX;
    }
    
    CCAssertLog(Name, "Name must not be null");
    CCAssertLog(Axes, "Axes must not be null");
    CCAssertLog(Buttons, "Buttons must not be null");
    CCAssertLog(Hats, "Hats must not be null");
    
    ControllerGetName = Name;
    ControllerUpdateAxes = Axes;
    ControllerUpdateButtons = Buttons;
    ControllerUpdateHats = Hats;
    
    ControllerCount = Count;
}

void CCControllerConnect(size_t Index, _Bool Connected)
{
    if (Index >= CC_CONTROLLER_MAX) return;
    
    if (Controller[Index].connected != Connected)
    {
        const uint64_t Timestamp = CCTimestamp();
        
        Controller[Index].timestamp = Timestamp;
        
        CCController *State = CCConcurrentSwapBufferGetFuture(&ControllerSwap[Index]);
        
        State->connected = Connected;
        State->timestamp = Timestamp;
        
        if (State->name)
        {
            CCStringDestroy(State->name);
            State->name = 0;
        }
        
        if (State->axes) CCArrayRemoveAllElements(State->axes);
        if (State->buttons) CCArrayRemoveAllElements(State->buttons);
        if (State->hats) CCArrayRemoveAllElements(State->hats);
        
        if ((Controller[Index].connected = Connected))
        {
            Controller[Index].name = ControllerGetName(Index);
            Controller[Index].axes = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(float), 16);
            Controller[Index].buttons = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(_Bool), 16);
            Controller[Index].hats = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCControllerHat), 16);
            
            State->name = CCStringCopy(Controller[Index].name);
            
            if (!State->axes) State->axes = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(float), 16);
            if (!State->buttons) State->buttons = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(_Bool), 16);
            if (!State->hats) State->hats = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCControllerHat), 16);
        }
        
        else
        {
            CCStringDestroy(Controller[Index].name);
            Controller[Index].name = 0;
            
            CCArrayDestroy(Controller[Index].axes);
            Controller[Index].axes = NULL;
            
            CCArrayDestroy(Controller[Index].buttons);
            Controller[Index].buttons = NULL;
            
            CCArrayDestroy(Controller[Index].hats);
            Controller[Index].hats = NULL;
        }
        
        CCConcurrentSwapBufferApply(&ControllerSwap[Index]);
        
        const CCControllerEvent Event = { .type = CCControllerEventTypeConnection, .timestamp = Timestamp, .connected = Connected };
        CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
    }
}

float CCControllerJoystickEventSensitivity = 0.05f;

static void CCControllerUpdateAxes(size_t Index, CCController *State)
{
    ControllerUpdateAxes(Index, State->axes);
    
    const size_t Count = CCArrayGetCount(State->axes);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        const float *NewValue = CCArrayGetElementAtIndex(State->axes, Loop2);
        
        if (!CCFloatEqualAbsolute(0.0f, *NewValue, CCControllerJoystickEventSensitivity))
        {
            const CCControllerEvent Event = { .type = CCControllerEventTypeAxis, .timestamp = State->timestamp, .axis = { .index = Index, .value = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
        }
    }
}

static void CCControllerDiffUpdateAxes(size_t Index, CCController *State)
{
    ControllerUpdateAxes(Index, State->axes);
    
    const size_t OldCount = CCArrayGetCount(Controller[Index].axes);
    const size_t NewCount = CCArrayGetCount(State->axes);
    const size_t Count = CCMin(OldCount, NewCount);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        float *OldValue = CCArrayGetElementAtIndex(Controller[Index].axes, Loop2);
        const float *NewValue = CCArrayGetElementAtIndex(State->axes, Loop2);
        
        if (!CCFloatEqualAbsolute(*OldValue, *NewValue, CCControllerJoystickEventSensitivity))
        {
            *OldValue = *NewValue;
            
            const CCControllerEvent Event = { .type = CCControllerEventTypeAxis, .timestamp = State->timestamp, .axis = { .index = Index, .value = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
        }
    }
    
    if (OldCount < NewCount)
    {
        for (size_t Loop2 = OldCount; Loop2 < NewCount; Loop2++)
        {
            const float *NewValue = CCArrayGetElementAtIndex(State->axes, Loop2);
            
            if (!CCFloatEqualAbsolute(0.0f, *NewValue, CCControllerJoystickEventSensitivity))
            {
                const CCControllerEvent Event = { .type = CCControllerEventTypeAxis, .timestamp = State->timestamp, .axis = { .index = Index, .value = *NewValue } };
                CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
            }
            
            CCArrayAppendElement(Controller[Index].axes, NewValue);
        }
    }
    
    else CCArrayRemoveElementsAtIndex(Controller[Index].axes, NewCount, OldCount - NewCount);
}

static void CCControllerUpdateButtons(size_t Index, CCController *State)
{
    ControllerUpdateButtons(Index, State->buttons);
    
    const size_t Count = CCArrayGetCount(State->buttons);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        const _Bool *NewValue = CCArrayGetElementAtIndex(State->buttons, Loop2);
        
        if (*NewValue)
        {
            const CCControllerEvent Event = { .type = CCControllerEventTypeButton, .timestamp = State->timestamp, .button = { .index = Index, .down = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
        }
    }
}

static void CCControllerDiffUpdateButtons(size_t Index, CCController *State)
{
    ControllerUpdateButtons(Index, State->buttons);
    
    const size_t OldCount = CCArrayGetCount(Controller[Index].buttons);
    const size_t NewCount = CCArrayGetCount(State->buttons);
    const size_t Count = CCMin(OldCount, NewCount);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        _Bool *OldValue = CCArrayGetElementAtIndex(Controller[Index].buttons, Loop2);
        const _Bool *NewValue = CCArrayGetElementAtIndex(State->buttons, Loop2);
        
        if (*OldValue != *NewValue)
        {
            *OldValue = *NewValue;
            
            const CCControllerEvent Event = { .type = CCControllerEventTypeButton, .timestamp = State->timestamp, .button = { .index = Index, .down = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
        }
    }
    
    if (OldCount < NewCount)
    {
        for (size_t Loop2 = OldCount; Loop2 < NewCount; Loop2++)
        {
            const _Bool *NewValue = CCArrayGetElementAtIndex(State->buttons, Loop2);
            
            if (*NewValue)
            {
                const CCControllerEvent Event = { .type = CCControllerEventTypeButton, .timestamp = State->timestamp, .button = { .index = Index, .down = *NewValue } };
                CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
            }
            
            CCArrayAppendElement(Controller[Index].buttons, NewValue);
        }
    }
    
    else CCArrayRemoveElementsAtIndex(Controller[Index].buttons, NewCount, OldCount - NewCount);
}

static void CCControllerUpdateHats(size_t Index, CCController *State)
{
    ControllerUpdateHats(Index, State->hats);
    
    const size_t Count = CCArrayGetCount(State->hats);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        const CCControllerHat *NewValue = CCArrayGetElementAtIndex(State->hats, Loop2);
        
        if (*NewValue)
        {
            const CCControllerEvent Event = { .type = CCControllerEventTypeHat, .timestamp = State->timestamp, .hat = { .index = Index, .position = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
        }
    }
}

static void CCControllerDiffUpdateHats(size_t Index, CCController *State)
{
    ControllerUpdateHats(Index, State->hats);
    
    const size_t OldCount = CCArrayGetCount(Controller[Index].hats);
    const size_t NewCount = CCArrayGetCount(State->hats);
    const size_t Count = CCMin(OldCount, NewCount);
    
    for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
    {
        CCControllerHat *OldValue = CCArrayGetElementAtIndex(Controller[Index].hats, Loop2);
        const CCControllerHat *NewValue = CCArrayGetElementAtIndex(State->hats, Loop2);
        
        if (*OldValue != *NewValue)
        {
            *OldValue = *NewValue;
            
            const CCControllerEvent Event = { .type = CCControllerEventTypeHat, .timestamp = State->timestamp, .hat = { .index = Index, .position = *NewValue } };
            CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);

        }
    }
    
    if (OldCount < NewCount)
    {
        for (size_t Loop2 = OldCount; Loop2 < NewCount; Loop2++)
        {
            const CCControllerHat *NewValue = CCArrayGetElementAtIndex(State->hats, Loop2);
            
            if (*NewValue != CCControllerHatCenter)
            {
                const CCControllerEvent Event = { .type = CCControllerEventTypeHat, .timestamp = State->timestamp, .hat = { .index = Index, .position = *NewValue } };
                CCConcurrentCircularBufferAddItem(&CCControllerInputEventBuffer, &Event);
            }
            
            CCArrayAppendElement(Controller[Index].hats, NewValue);
        }
    }
    
    else CCArrayRemoveElementsAtIndex(Controller[Index].hats, NewCount, OldCount - NewCount);
}

void CCControllerUpdateState(void)
{
    for (size_t Loop = 0; Loop < ControllerCount; Loop++)
    {
        if (Controller[Loop].connected)
        {
            const uint64_t Timestamp = CCTimestamp();
            
            Controller[Loop].timestamp = Timestamp;
            
            CCController *State = CCConcurrentSwapBufferGetFuture(&ControllerSwap[Loop]);
            
            State->timestamp = Timestamp;
            
            CCControllerUpdateAxes(Loop, State);
            CCControllerUpdateButtons(Loop, State);
            CCControllerUpdateHats(Loop, State);
            
            CCConcurrentSwapBufferApply(&ControllerSwap[Loop]);
        };
    }
}

void CCControllerDiffUpdateState(void)
{
    for (size_t Loop = 0; Loop < ControllerCount; Loop++)
    {
        if (Controller[Loop].connected)
        {
            const uint64_t Timestamp = CCTimestamp();
            
            Controller[Loop].timestamp = Timestamp;
            
            CCController *State = CCConcurrentSwapBufferGetFuture(&ControllerSwap[Loop]);
            
            State->timestamp = Timestamp;
            
            CCControllerDiffUpdateAxes(Loop, State);
            CCControllerDiffUpdateButtons(Loop, State);
            CCControllerDiffUpdateHats(Loop, State);
            
            CCConcurrentSwapBufferApply(&ControllerSwap[Loop]);
        };
    }
}

_Atomic(const CCController *) ControllerSnapshot[CC_CONTROLLER_MAX];

void CCControllerSync(void)
{
    for (size_t Loop = 0; Loop < ControllerCount; Loop++)
    {
        const CCController *State = CCConcurrentSwapBufferRead(&ControllerSwap[Loop]);
        
        if (State->connected)
        {
            atomic_store_explicit(&ControllerSnapshot[Loop], State, memory_order_release);
        }
        
        else atomic_store_explicit(&ControllerSnapshot[Loop], NULL, memory_order_relaxed);
    }
}

static _Thread_local struct {
    const CCController *state;
    _Bool cached;
} ControllerCache[CC_CONTROLLER_MAX];

void CCControllerClearCache(void)
{
    memset(ControllerCache, 0, sizeof(ControllerCache));
}

const CCController *CCControllerGetController(size_t Index)
{
    if (Index < ControllerCount)
    {
        if (ControllerCache[Index].cached) return ControllerCache[Index].state;
        
        ControllerCache[Index].cached = TRUE;
        
        return ControllerCache[Index].state = atomic_load_explicit(&ControllerSnapshot[Index], memory_order_consume);
    }
    
    return NULL;
}

_Bool CCControllerIsDevice(const CCController *State, CCString Device)
{
    return (State) && (State->connected) && ((!Device) || (CCStringEqual(Device, State->name)));
}

static inline float CCFloatPrecision(float Value, uint8_t N)
{
    return roundf(Value * N) / N;
}

float CCControllerGetStateForJoystick1D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X)
{
    float Position = 0.0f;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->axes);
        if ((X.index >= 0) && (X.index < Count)) Position = *(float*)CCArrayGetElementAtIndex(State->axes, X.index);
        
        if (X.flip) Position = -Position;
        
        if (Resolution)
        {
            Position = CCFloatPrecision(Position, Resolution);
        }
        
        if (Position > 0.0f)
        {
            const float Length = Position;
            if (Length < Deadzone) Position = 0.0f;
            else Position = Position * ((Length - Deadzone) / (1 - Deadzone));
            
            Position = CCClamp(Position, -1.0f, 1.0f);
        }
        
        if (X.oneSided) Position = (Position + 1.0f) / 2.0f;
    }
    
    return Position;
}

CCVector2D CCControllerGetStateForJoystick2D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y)
{
    CCVector2D Position = CCVector2DZero;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->axes);
        if ((X.index >= 0) && (X.index < Count)) Position.x = *(float*)CCArrayGetElementAtIndex(State->axes, X.index);
        if ((Y.index >= 0) && (Y.index < Count)) Position.y = *(float*)CCArrayGetElementAtIndex(State->axes, Y.index);
        
        if (X.flip) Position.x = -Position.x;
        if (Y.flip) Position.y = -Position.y;
        
        if (Resolution)
        {
            Position.x = CCFloatPrecision(Position.x, Resolution);
            Position.y = CCFloatPrecision(Position.y, Resolution);
        }
        
        if ((Position.x > 0.0f) || (Position.y > 0.0f))
        {
            const float Length = CCVector2Length(Position);
            if (Length < Deadzone) Position = CCVector2DZero;
            else Position = CCVector2MulScalar(CCVector2Normalize(Position), (Length - Deadzone) / (1 - Deadzone));
            
            Position = CCVector2Clamp(Position, CCVector2DFill(-1.0f), CCVector2DFill(1.0f));
        }
        
        if (X.oneSided) Position.x = (Position.x + 1.0f) / 2.0f;
        if (Y.oneSided) Position.y = (Position.y + 1.0f) / 2.0f;
    }
    
    return Position;
}

CCVector3D CCControllerGetStateForJoystick3D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y, CCControllerAxisMap Z)
{
    CCVector3D Position = CCVector3DZero;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->axes);
        if ((X.index >= 0) && (X.index < Count)) Position.x = *(float*)CCArrayGetElementAtIndex(State->axes, X.index);
        if ((Y.index >= 0) && (Y.index < Count)) Position.y = *(float*)CCArrayGetElementAtIndex(State->axes, Y.index);
        if ((Z.index >= 0) && (Z.index < Count)) Position.z = *(float*)CCArrayGetElementAtIndex(State->axes, Z.index);
        
        if (X.flip) Position.x = -Position.x;
        if (Y.flip) Position.y = -Position.y;
        if (Z.flip) Position.z = -Position.z;
        
        if (Resolution)
        {
            Position.x = CCFloatPrecision(Position.x, Resolution);
            Position.y = CCFloatPrecision(Position.y, Resolution);
            Position.z = CCFloatPrecision(Position.z, Resolution);
        }
        
        if ((Position.x > 0.0f) || (Position.y > 0.0f) || (Position.z > 0.0f))
        {
            const float Length = CCVector3Length(Position);
            if (Length < Deadzone) Position = CCVector3DZero;
            else Position = CCVector3MulScalar(CCVector3Normalize(Position), (Length - Deadzone) / (1 - Deadzone));
            
            Position = CCVector3Clamp(Position, CCVector3DFill(-1.0f), CCVector3DFill(1.0f));
        }
        
        if (X.oneSided) Position.x = (Position.x + 1.0f) / 2.0f;
        if (Y.oneSided) Position.y = (Position.y + 1.0f) / 2.0f;
        if (Z.oneSided) Position.z = (Position.z + 1.0f) / 2.0f;
    }
    
    return Position;
}

CCVector4D CCControllerGetStateForJoystick4D(const CCController *State, uint8_t Resolution, float Deadzone, CCControllerAxisMap X, CCControllerAxisMap Y, CCControllerAxisMap Z, CCControllerAxisMap W)
{
    CCVector4D Position = CCVector4DZero;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->axes);
        if ((X.index >= 0) && (X.index < Count)) Position.x = *(float*)CCArrayGetElementAtIndex(State->axes, X.index);
        if ((Y.index >= 0) && (Y.index < Count)) Position.y = *(float*)CCArrayGetElementAtIndex(State->axes, Y.index);
        if ((Z.index >= 0) && (Z.index < Count)) Position.z = *(float*)CCArrayGetElementAtIndex(State->axes, Z.index);
        if ((W.index >= 0) && (W.index < Count)) Position.w = *(float*)CCArrayGetElementAtIndex(State->axes, W.index);
        
        if (X.flip) Position.x = -Position.x;
        if (Y.flip) Position.y = -Position.y;
        if (Z.flip) Position.z = -Position.z;
        if (W.flip) Position.w = -Position.w;
        
        if (Resolution)
        {
            Position.x = CCFloatPrecision(Position.x, Resolution);
            Position.y = CCFloatPrecision(Position.y, Resolution);
            Position.z = CCFloatPrecision(Position.z, Resolution);
            Position.w = CCFloatPrecision(Position.w, Resolution);
        }
        
        if ((Position.x > 0.0f) || (Position.y > 0.0f) || (Position.z > 0.0f) || (Position.w > 0.0f))
        {
            const float Length = CCVector4Length(Position);
            if (Length < Deadzone) Position = CCVector4DZero;
            else Position = CCVector4MulScalar(CCVector4Normalize(Position), (Length - Deadzone) / (1 - Deadzone));
            
            Position = CCVector4Clamp(Position, CCVector4DFill(-1.0f), CCVector4DFill(1.0f));
        }
        
        if (X.oneSided) Position.x = (Position.x + 1.0f) / 2.0f;
        if (Y.oneSided) Position.y = (Position.y + 1.0f) / 2.0f;
        if (Z.oneSided) Position.z = (Position.z + 1.0f) / 2.0f;
        if (W.oneSided) Position.w = (Position.w + 1.0f) / 2.0f;
    }
    
    return Position;
}

_Bool CCControllerGetStateForButton(const CCController *State, int32_t Button)
{
    _Bool Active = FALSE;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->buttons);
        if ((Button >= 0) && (Button < Count)) Active = *(_Bool*)CCArrayGetElementAtIndex(State->buttons, Button);
    }
    
    return Active;
}

CCControllerHat CCControllerGetStateForHat(const CCController *State, int32_t Hat)
{
    CCControllerHat HatState = CCControllerHatCenter;
    
    if ((State) && (State->connected))
    {
        const size_t Count = CCArrayGetCount(State->hats);
        if ((Hat >= 0) && (Hat < Count)) HatState = *(CCControllerHat*)CCArrayGetElementAtIndex(State->hats, Hat);
    }
    
    return HatState;
}
