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
#include "InputSystem.h"
#include "InputMapControllerComponent.h"
#include "InputMapControllerAxesComponent.h"
#include "InputMapControllerButtonComponent.h"

typedef struct {
    CCString name;
    double timestamp;
    CCOrderedCollection(float) axes;
    CCOrderedCollection(_Bool) buttons;
    _Bool connected;
    atomic_flag lock;
} CCController;

static CCController *Controller = NULL;

static size_t ControllerCount = 0;
static CCControllerGetNameCallback ControllerGetName = NULL;
static CCControllerUpdateAxesCallback ControllerUpdateAxes = NULL;
static CCControllerUpdateButtonsCallback ControllerUpdateButtons = NULL;

void CCControllerSetup(size_t Count, CCControllerGetNameCallback Name, CCControllerUpdateAxesCallback Axes, CCControllerUpdateButtonsCallback Buttons)
{
    CCAssertLog(Name, "Name must not be null");
    CCAssertLog(Axes, "Name must not be null");
    CCAssertLog(Buttons, "Name must not be null");
    
    ControllerGetName = Name;
    ControllerUpdateAxes = Axes;
    ControllerUpdateButtons = Buttons;
    
    CC_SAFE_Malloc(Controller, sizeof(CCController) * Count,
                   CC_LOG_ERROR("Failed to create controllers, due to allocation failure (%zu)", sizeof(CCController) * Count);
                   return;
                   );
    
    ControllerCount = Count;
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        Controller[Loop] = (CCController){
            .name = 0,
            .timestamp = 0.0,
            .axes = NULL,
            .buttons = NULL,
            .connected = FALSE,
            .lock = (atomic_flag)ATOMIC_FLAG_INIT
        };
    }
}

void CCControllerConnect(size_t Index, _Bool Connected)
{
    while (!atomic_flag_test_and_set(&Controller[Index].lock)) CC_SPIN_WAIT();
    
    if ((Controller[Index].connected = Connected))
    {
        Controller[Index].name = ControllerGetName(Index);
        Controller[Index].axes = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(float), NULL);
        Controller[Index].buttons = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(_Bool), NULL);
    }
    
    else
    {
        if (Controller[Index].name)
        {
            CCStringDestroy(Controller[Index].name);
            Controller[Index].name = 0;
        }
        
        if (Controller[Index].axes)
        {
            CCCollectionDestroy(Controller[Index].axes);
            Controller[Index].axes = NULL;
        }
        
        if (Controller[Index].buttons)
        {
            CCCollectionDestroy(Controller[Index].buttons);
            Controller[Index].buttons = NULL;
        }
    }
    
    atomic_flag_clear(&Controller[Index].lock);
}

void CCControllerUpdateState(void)
{
    for (int Loop = 0; Loop < ControllerCount; Loop++)
    {
        while (!atomic_flag_test_and_set(&Controller[Loop].lock)) CC_SPIN_WAIT();
        if (Controller[Loop].connected)
        {
            Controller[Loop].timestamp = CCTimestamp();
            ControllerUpdateAxes(Loop, Controller[Loop].axes);
            ControllerUpdateButtons(Loop, Controller[Loop].buttons);
        }
        atomic_flag_clear(&Controller[Loop].lock);
    }
    
    /*
     TODO: handle callbacks
     Should probably handle it by checking for large difference in state, or if a minimum amount of time has elapsed.
     */
}

static float CCFloatPrecision(float Value, uint64_t Bits)
{
    const uint64_t NewValueMax = CCBitSet(Bits);
    Value *= (NewValueMax / 2) + (Value < 0 ? 1 : 0);
    
    Value = (float)((((uint64_t)Value & NewValueMax) + (NewValueMax / 2) + 1) & NewValueMax);
    Value -= (NewValueMax / 2) + 1;
    
    return fmaxf(Value / (NewValueMax / 2), -1.0f);
}

CCControllerState CCControllerGetStateForComponent(CCComponent Component)
{
    CCAssertLog(CCComponentGetID(Component) == CC_INPUT_MAP_CONTROLLER_COMPONENT_ID, "Must be a input map controller component");
    
    const int8_t Connection = CCInputMapControllerComponentGetConnection(Component);
    const CCString Device = CCInputMapControllerComponentGetDevice(Component);
    
    switch (CCComponentGetID(Component) & CCInputMapTypeMask)
    {
        case CCInputMapTypeControllerAxes:
        {
            const int32_t x = CCInputMapControllerAxesComponentGetX(Component), y = CCInputMapControllerAxesComponentGetY(Component), z = CCInputMapControllerAxesComponentGetZ(Component);
            
            CCVector3D Position = CCVector3DZero;
            
            while (!atomic_flag_test_and_set(&Controller[Connection].lock)) CC_SPIN_WAIT();
            if (Controller[Connection].connected)
            {
                if ((!Device) || (CCStringEqual(Device, Controller[Connection].name)))
                {
                    const size_t Count = CCCollectionGetCount(Controller[Connection].axes);
                    if ((x >= 0) && (x < Count)) Position.x = *(float*)CCOrderedCollectionGetElementAtIndex(Controller[Connection].axes, x);
                    if ((y >= 0) && (x < Count)) Position.y = *(float*)CCOrderedCollectionGetElementAtIndex(Controller[Connection].axes, y);
                    if ((z >= 0) && (x < Count)) Position.z = *(float*)CCOrderedCollectionGetElementAtIndex(Controller[Connection].axes, z);
                }
            }
            atomic_flag_clear(&Controller[Connection].lock);
            
            if (CCInputMapControllerAxesComponentGetFlipX(Component)) Position.x = -Position.x;
            if (CCInputMapControllerAxesComponentGetFlipY(Component)) Position.y = -Position.y;
            if (CCInputMapControllerAxesComponentGetFlipZ(Component)) Position.z = -Position.z;
            
            const uint8_t Resolution = CCInputMapControllerAxesComponentGetResolution(Component);
            if (Resolution)
            {
                Position.x = CCFloatPrecision(Position.x, Resolution);
                Position.y = CCFloatPrecision(Position.y, Resolution);
                Position.z = CCFloatPrecision(Position.z, Resolution);
            }
            
            if ((Position.x > 0.0f) || (Position.y > 0.0f) || (Position.z > 0.0f))
            {
                const float Deadzone = CCInputMapControllerAxesComponentGetDeadzone(Component), Length = CCVector3Length(Position);
                if (Length < Deadzone) Position = CCVector3DZero;
                else Position = CCVector3MulScalar(CCVector3Normalize(Position), (Length - Deadzone) / (1 - Deadzone));
                
                Position = CCVector3Clamp(Position, CCVector3DFill(-1.0f), CCVector3DFill(1.0f));
            }
            
            if (CCInputMapControllerAxesComponentGetOneSidedX(Component)) Position.x = (Position.x + 1.0f) / 2.0f;
            if (CCInputMapControllerAxesComponentGetOneSidedY(Component)) Position.y = (Position.y + 1.0f) / 2.0f;
            if (CCInputMapControllerAxesComponentGetOneSidedZ(Component)) Position.z = (Position.z + 1.0f) / 2.0f;
            
            
            return (CCControllerState){ .axes = Position };
        }
            
        case CCInputMapTypeControllerButton:
        {
            const int32_t Button = CCInputMapControllerButtonComponentGetButton(Component);
            
            _Bool Active = FALSE;
            double Timestamp = -INFINITY;
            
            while (!atomic_flag_test_and_set(&Controller[Connection].lock)) CC_SPIN_WAIT();
            if (Controller[Connection].connected)
            {
                if ((!Device) || (CCStringEqual(Device, Controller[Connection].name)))
                {
                    const size_t Count = CCCollectionGetCount(Controller[Connection].buttons);
                    if ((Button >= 0) && (Button < Count)) Active = *(_Bool*)CCOrderedCollectionGetElementAtIndex(Controller[Connection].buttons, Button);
                    
                    Timestamp = Controller[Connection].timestamp;
                }
            }
            atomic_flag_clear(&Controller[Connection].lock);
            
            
            return (CCControllerState){ .button = { .timestamp = Timestamp, .down = Active } };
        }
            
        default:
            break;
    }
    
    return (CCControllerState){ .axes = CCVector3DZero };
}
