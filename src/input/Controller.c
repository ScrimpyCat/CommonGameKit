//
//  Controller.c
//  Blob Game
//
//  Created by Stefan Johnson on 12/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "Controller.h"
#include "Window.h"
#include "stdatomic.h"
#include "InputSystem.h"
#include "InputMapControllerComponent.h"
#include "InputMapControllerAxesComponent.h"
#include "InputMapControllerButtonComponent.h"

static struct {
    const char *name;
    double timestamp;
    CCOrderedCollection axes;
    CCOrderedCollection buttons;
    _Bool connected;
    atomic_flag lock;
} Controller[GLFW_JOYSTICK_LAST + 1];

void CCControllerSetup(void)
{
    for (size_t Loop = 0; Loop < sizeof(Controller) / sizeof(typeof(*Controller)); Loop++)
    {
        Controller[Loop].lock = (atomic_flag)ATOMIC_FLAG_INIT;
    }
}

void CCControllerUpdateState(void)
{
    for (int Loop = 0; Loop < sizeof(Controller) / sizeof(typeof(*Controller)); Loop++)
    {
        while (!atomic_flag_test_and_set(&Controller[Loop].lock));
        if ((Controller[Loop].connected = glfwJoystickPresent(Loop)))
        {
            Controller[Loop].name = glfwGetJoystickName(Loop);
            Controller[Loop].timestamp = glfwGetTime();
            
            if (!Controller[Loop].axes) Controller[Loop].axes = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(float), NULL);
            if (!Controller[Loop].buttons) Controller[Loop].buttons = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(unsigned char), NULL);
            
            int Count;
            const float *Axes = glfwGetJoystickAxes(Loop, &Count);
            
            if (Count == CCCollectionGetCount(Controller[Loop].axes)) for (int Loop2 = 0; Loop2 < Count; Loop2++) CCOrderedCollectionReplaceElementAtIndex(Controller[Loop].axes, &Axes[Loop2], Loop2);
            else
            {
                CCCollectionRemoveAllElements(Controller[Loop].axes);
                for (int Loop2 = 0; Loop2 < Count; Loop2++) CCOrderedCollectionAppendElement(Controller[Loop].axes, &Axes[Loop2]);
            }
            
            
            const unsigned char *Buttons = glfwGetJoystickButtons(Loop, &Count);
            
            if (Count == CCCollectionGetCount(Controller[Loop].buttons)) for (int Loop2 = 0; Loop2 < Count; Loop2++) CCOrderedCollectionReplaceElementAtIndex(Controller[Loop].buttons, &Buttons[Loop2], Loop2);
            else
            {
                CCCollectionRemoveAllElements(Controller[Loop].buttons);
                for (int Loop2 = 0; Loop2 < Count; Loop2++) CCOrderedCollectionAppendElement(Controller[Loop].buttons, &Buttons[Loop2]);
            }
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
    const int8_t Connection = CCInputMapControllerComponentGetConnection(Component);
    const char *Device = CCInputMapControllerComponentGetDevice(Component);
    
    switch (CCComponentGetID(Component) & CCInputMapTypeMask)
    {
        case CCInputMapTypeControllerAxes:
        {
            const int32_t x = CCInputMapControllerAxesComponentGetX(Component), y = CCInputMapControllerAxesComponentGetY(Component), z = CCInputMapControllerAxesComponentGetZ(Component);
            
            CCVector3D Position = CCVector3DZero;
            
            while (!atomic_flag_test_and_set(&Controller[Connection].lock));
            if (Controller[Connection].connected)
            {
                if ((!Device) || (!strcmp(Device, Controller[Connection].name)))
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
            
            while (!atomic_flag_test_and_set(&Controller[Connection].lock));
            if (Controller[Connection].connected)
            {
                if ((!Device) || (!strcmp(Device, Controller[Connection].name)))
                {
                    const size_t Count = CCCollectionGetCount(Controller[Connection].buttons);
                    if ((Button >= 0) && (Button < Count)) Active = *(unsigned char*)CCOrderedCollectionGetElementAtIndex(Controller[Connection].buttons, Button);
                    
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
