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

#include "Input.h"
#include "Callbacks.h"


void CCInputMapDestructor(CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
            for (size_t Loop = 0, Count = InputMap->group.count; Loop < Count; Loop++)
            {
                CCInputMapDestructor(&InputMap->group.inputMaps[Loop]);
            }
            
            CC_SAFE_Free(InputMap->group.inputMaps);
            break;
            
        case CCInputMapTypeVirtualJoystick:
            for (size_t Loop = 0, Count = InputMap->virtual.joystick.count; Loop < Count; Loop++)
            {
                CCInputMapDestructor(&InputMap->virtual.joystick.inputMaps[Loop]);
            }
            
            CC_SAFE_Free(InputMap->virtual.joystick.inputMaps);
            break;
            
        default:
            break;
    }
}

const CCReflectInteger CC_REFLECT(CCInputMapType) = CC_REFLECT_SIGNED_INTEGER(CCInputMapType, CCReflectEndianNative);

static void CCInputMapMapper(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent);
static void CCInputMapUnmapper(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator);

const CCReflectOpaque CC_REFLECT(CCInputMap) = CC_REFLECT_OPAQUE(sizeof(CCInputMap), sizeof(CCReflectOpaque), CCInputMapMapper, CCInputMapUnmapper);

static const CCReflectStruct1 CCInputMapReflectBase = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType))
);

static const CCReflectStruct4 CCInputMapReflectGroupEmpty = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (group.inputMaps, &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT_ARRAY(&CC_REFLECT(CCInputMap), 0), CCReflectOwnershipRetain)),
    (group.count, &CC_REFLECT(size_t)),
    (group.wantsAllActive, &CC_REFLECT(_Bool))
);

static const CCReflectStruct8 CCInputMapReflectKeyboard = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (keyboard.keycode, &CC_REFLECT(CCKeyboardKeycode)),
    (keyboard.character, &CC_REFLECT(CCChar)),
    (keyboard.flags, &CC_REFLECT(CCKeyboardModifier)),
    (keyboard.ramp, &CC_REFLECT(float)),
    (keyboard.isKeycode, &CC_REFLECT(_Bool)),
    (keyboard.ignoreModifier, &CC_REFLECT(_Bool)),
    (keyboard.repeats, &CC_REFLECT(_Bool))
);

static const CCReflectStruct5 CCInputMapReflectMouseButton = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (mouse.button.code, &CC_REFLECT(CCMouseButton)),
    (mouse.button.flags, &CC_REFLECT(CCKeyboardModifier)),
    (mouse.button.ramp, &CC_REFLECT(float)),
    (mouse.button.ignoreModifier, &CC_REFLECT(_Bool))
);

static const CCReflectStruct9 CCInputMapReflectControllerAxis = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (controller.device, &CC_REFLECT(CCString)),
    (controller.connection, &CC_REFLECT(int8_t)),
    (controller.axis.deadzone, &CC_REFLECT(float)),
    (controller.axis.resolution, &CC_REFLECT(uint8_t)),
    (controller.axis.x, &CC_REFLECT(CCControllerAxisMap)),
    (controller.axis.y, &CC_REFLECT(CCControllerAxisMap)),
    (controller.axis.z, &CC_REFLECT(CCControllerAxisMap)),
    (controller.axis.w, &CC_REFLECT(CCControllerAxisMap))
);

static const CCReflectStruct5 CCInputMapReflectControllerButton = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (controller.device, &CC_REFLECT(CCString)),
    (controller.connection, &CC_REFLECT(int8_t)),
    (controller.button.index, &CC_REFLECT(int32_t)),
    (controller.button.ramp, &CC_REFLECT(float))
);

static const CCReflectStruct5 CCInputMapReflectControllerHat = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (controller.device, &CC_REFLECT(CCString)),
    (controller.connection, &CC_REFLECT(int8_t)),
    (controller.hat.index, &CC_REFLECT(int32_t)),
    (controller.hat.ramp, &CC_REFLECT(float))
);

static const CCReflectStruct3 CCInputMapReflectVirtualJoystickEmpty = CC_REFLECT_STRUCT(CCInputMap,
    (type, &CC_REFLECT(CCInputMapType)),
    (virtual.joystick.inputMaps, &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT_ARRAY(&CC_REFLECT(CCInputMap), 0), CCReflectOwnershipRetain)),
    (virtual.joystick.count, &CC_REFLECT(size_t))
);

static void CCInputMapMapper(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent)
{
    switch (((const CCInputMap*)Data)->type)
    {
        case CCInputMapTypeGroup:
        {
            CCReflectStruct4 GroupType = CCInputMapReflectGroupEmpty;
            GroupType.fields[1].type = &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT_ARRAY(&CC_REFLECT(CCInputMap), ((const CCInputMap*)Data)->group.count), CCReflectOwnershipRetain);
            
            Handler(&GroupType, Data, Args);
            break;
        }
            
        case CCInputMapTypeKeyboard:
            Handler(&CCInputMapReflectKeyboard, Data, Args);
            break;
            
        case CCInputMapTypeMouseButton:
            Handler(&CCInputMapReflectMouseButton, Data, Args);
            break;
            
        case CCInputMapTypeControllerAxis:
            Handler(&CCInputMapReflectControllerAxis, Data, Args);
            break;
            
        case CCInputMapTypeControllerButton:
            Handler(&CCInputMapReflectControllerButton, Data, Args);
            break;
            
        case CCInputMapTypeControllerHat:
            Handler(&CCInputMapReflectControllerHat, Data, Args);
            break;
            
        case CCInputMapTypeVirtualJoystick:
        {
            CCReflectStruct3 VirtualJoystickType = CCInputMapReflectVirtualJoystickEmpty;
            VirtualJoystickType.fields[1].type = &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT_ARRAY(&CC_REFLECT(CCInputMap), ((const CCInputMap*)Data)->virtual.joystick.count), CCReflectOwnershipRetain);
            
            Handler(&VirtualJoystickType, Data, Args);
            break;
        }
            
        default:
            Handler(&CCInputMapReflectBase, Data, Args);
            break;
    }
}

static void CCInputMapUnmapper(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator)
{
    Handler(MappedType, Data, Args);
}

void CCInputSync(void)
{
    CCControllerClearCache();
}

void CCInputWindowFocus(_Bool Focus)
{
    if (!Focus)
    {
        CCKeyboardStateReset();
    }
}

static CCInputState CCInputMapGetGroupState(const CCInputMap *InputGroupMap);
static CCInputPressure4D CCInputGetVirtualJoystickState(const CCInputMap *InputVirtualMap);

CCInputState CCInputGetState(const CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
            return CCInputMapGetGroupState(InputMap);
            
        case CCInputMapTypeKeyboard:
        {
            CCKeyboardState KeyState;
            
            if (InputMap->keyboard.isKeycode) KeyState = CCKeyboardGetStateForKeycode(InputMap->keyboard.keycode, InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
            else KeyState = CCKeyboardGetStateForCharacter(InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
            
            return (CCInputState){ .timestamp = KeyState.timestamp, .active = KeyState.down };
        }
            
        case CCInputMapTypeMouseButton:
        {
            CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForButton(InputMap->mouse.button.code, InputMap->mouse.button.flags, InputMap->mouse.button.ignoreModifier);
            
            return (CCInputState){ .timestamp = MouseButtonState.timestamp, .active = MouseButtonState.down };
        }
            
        case CCInputMapTypeControllerAxis:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCVector4D Joystick = CCControllerGetStateForJoystick4D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x, InputMap->controller.axis.y, InputMap->controller.axis.z, InputMap->controller.axis.w);
                
                return (CCInputState){ .timestamp = Controller->timestamp, .active = CCVector4Equal(Joystick, CCVector4DZero) };
            }
        }
            
        case CCInputMapTypeControllerButton:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device)) return (CCInputState){ .timestamp = Controller->timestamp, .active = CCControllerGetStateForButton(Controller, InputMap->controller.button.index) };
        }
            
        case CCInputMapTypeControllerHat:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device)) return (CCInputState){ .timestamp = Controller->timestamp, .active = CCControllerGetStateForHat(Controller, InputMap->controller.hat.index) != CCControllerHatCenter };
        }
            
        default:
            break;
    }
    
    return (CCInputState){ .timestamp = CCTimestamp(), .active = FALSE };
}

static CCInputPressure1D CCInputPressureForBinaryInput(_Bool Active, uint64_t Timestamp, float Ramp, uint64_t CurrentTimestamp)
{
    Ramp *= (double)(CurrentTimestamp - Timestamp) / 1000000000.0;
    Ramp = Ramp == 0.0f ? 1.0f : Ramp;
    return (CCInputPressure1D){ .timestamp = CurrentTimestamp, .pressure = CCClamp((Active ? 0.0f + Ramp : 1.0f - Ramp), 0.0f, 1.0f) };
}

CCInputPressure1D CCInputGetPressure1D(const CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
        {
            if (InputMap->group.wantsAllActive)
            {
                CCInputState GroupState = CCInputMapGetGroupState(InputMap);
                
                return CCInputPressureForBinaryInput(GroupState.active, GroupState.timestamp, 0.0f, CCTimestamp());
            }
            
            else
            {
                uint64_t Timestamp = 0;
                float Pressure = 0.0f;
                
                for (size_t Loop = 0, Count = InputMap->group.count; Loop < Count; Loop++)
                {
                    const CCInputPressure1D State = CCInputGetPressure1D(&InputMap->group.inputMaps[Loop]);
                    
                    Pressure += State.pressure;
                    
                    if (State.timestamp > Timestamp) Timestamp = State.timestamp;
                }
                
                return (CCInputPressure1D){ .timestamp = Timestamp, .pressure = CCClamp(Pressure, -1.0f, 1.0f) };
            }
        }
            
        case CCInputMapTypeKeyboard:
        {
            CCKeyboardState KeyState;
            
            if (InputMap->keyboard.isKeycode) KeyState = CCKeyboardGetStateForKeycode(InputMap->keyboard.keycode, InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
            else KeyState = CCKeyboardGetStateForCharacter(InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
            
            return CCInputPressureForBinaryInput(KeyState.down, KeyState.timestamp, InputMap->keyboard.ramp, CCTimestamp());
        }
            
        case CCInputMapTypeMouseButton:
        {
            const CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForButton(InputMap->mouse.button.code, InputMap->mouse.button.flags, InputMap->mouse.button.ignoreModifier);
            
            return CCInputPressureForBinaryInput(MouseButtonState.down, MouseButtonState.timestamp, InputMap->mouse.button.ramp, CCTimestamp());
        }
            
        case CCInputMapTypeControllerAxis:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                return (CCInputPressure1D){ .timestamp = Controller->timestamp, .pressure = CCControllerGetStateForJoystick1D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x) };
            }
        }
            
        case CCInputMapTypeControllerButton:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const _Bool ControllerButtonState = CCControllerGetStateForButton(Controller, InputMap->controller.button.index);
                
                return CCInputPressureForBinaryInput(ControllerButtonState, Controller->timestamp, InputMap->controller.button.ramp, CCTimestamp());
            }
        }
            
        case CCInputMapTypeControllerHat:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCControllerHat ControllerHatState = CCControllerGetStateForHat(Controller, InputMap->controller.hat.index);
                
                return CCInputPressureForBinaryInput(ControllerHatState != CCControllerHatCenter, Controller->timestamp, InputMap->controller.button.ramp, CCTimestamp());
            }
        }
            
        case CCInputMapTypeVirtualJoystick:
        {
            const CCInputPressure4D Joystick = CCInputGetVirtualJoystickState(InputMap);
            
            return (CCInputPressure1D){ .timestamp = Joystick.timestamp, .pressure = Joystick.pressure.x };
        }
            
        default:
            break;
    }
    
    return (CCInputPressure1D){ .timestamp = CCTimestamp(), .pressure = 0.0f };
}

CCInputPressure2D CCInputGetPressure2D(const CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
            if (!InputMap->group.wantsAllActive)
            {
                uint64_t Timestamp = 0;
                CCVector2D Pressure2 = CCVector2DZero;
                
                for (size_t Loop = 0, Count = InputMap->group.count; Loop < Count; Loop++)
                {
                    const CCInputPressure2D State = CCInputGetPressure2D(&InputMap->group.inputMaps[Loop]);
                    
                    Pressure2 = CCVector2Add(Pressure2, State.pressure);
                    
                    if (State.timestamp > Timestamp) Timestamp = State.timestamp;
                }
                
                return (CCInputPressure2D){ .timestamp = Timestamp, .pressure = CCVector2Clamp(Pressure2, CCVector2DFill(-1.0f), CCVector2DFill(1.0f)) };
            }
            break;
            
        case CCInputMapTypeControllerAxis:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCVector2D Joystick = CCControllerGetStateForJoystick2D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x, InputMap->controller.axis.y);
                
                return (CCInputPressure2D){ .timestamp = Controller->timestamp, .pressure = Joystick };
            }
            
            return (CCInputPressure2D){ .timestamp = CCTimestamp(), .pressure = CCVector2DZero };
        }
            
        case CCInputMapTypeControllerHat:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCControllerHat ControllerHatState = CCControllerGetStateForHat(Controller, InputMap->controller.hat.index);
                const CCInputPressure1D State = CCInputPressureForBinaryInput(TRUE, Controller->timestamp, InputMap->controller.hat.ramp, CCTimestamp());
                
                CCVector2D Pressure;
                
                switch (ControllerHatState)
                {
                    case CCControllerHatCenter:
                        Pressure = CCVector2DZero;
                        break;
                        
                    case CCControllerHatUp:
                        Pressure = CCVector2DMake(0.0f, State.pressure);
                        break;
                        
                    case CCControllerHatRight:
                        Pressure = CCVector2DMake(State.pressure, 0.0f);
                        break;
                        
                    case CCControllerHatDown:
                        Pressure = CCVector2DMake(0.0f, -State.pressure);
                        break;
                        
                    case CCControllerHatLeft:
                        Pressure = CCVector2DMake(-State.pressure, 0.0f);
                        break;
                        
                    case CCControllerHatRightUp:
                        Pressure = CCVector2DMake(State.pressure, State.pressure);
                        break;
                        
                    case CCControllerHatRightDown:
                        Pressure = CCVector2DMake(State.pressure, -State.pressure);
                        break;
                        
                    case CCControllerHatLeftUp:
                        Pressure = CCVector2DMake(-State.pressure, State.pressure);
                        break;
                        
                    case CCControllerHatLeftDown:
                        Pressure = CCVector2DMake(-State.pressure, -State.pressure);
                        break;
                }
                
                return (CCInputPressure2D){ .timestamp = Controller->timestamp, .pressure = Pressure };
            }
        }
            
        case CCInputMapTypeVirtualJoystick:
        {
            const CCInputPressure4D Joystick = CCInputGetVirtualJoystickState(InputMap);
            
            return (CCInputPressure2D){ .timestamp = Joystick.timestamp, .pressure = CCVector2DMake(Joystick.pressure.x, Joystick.pressure.y) };
        }
            
        default:
            break;
    }
    
    const CCInputPressure1D State = CCInputGetPressure1D(InputMap);
    
    return (CCInputPressure2D){ .timestamp = State.timestamp, .pressure = CCVector2DMake(State.pressure, 0.0f) };
}

CCInputPressure3D CCInputGetPressure3D(const CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
            if (!InputMap->group.wantsAllActive)
            {
                uint64_t Timestamp = 0;
                CCVector3D Pressure3 = CCVector3DZero;
                
                for (size_t Loop = 0, Count = InputMap->group.count; Loop < Count; Loop++)
                {
                    const CCInputPressure3D State = CCInputGetPressure3D(&InputMap->group.inputMaps[Loop]);
                    
                    Pressure3 = CCVector3Add(Pressure3, State.pressure);
                    
                    if (State.timestamp > Timestamp) Timestamp = State.timestamp;
                }
                
                return (CCInputPressure3D){ .timestamp = Timestamp, .pressure = CCVector3Clamp(Pressure3, CCVector3DFill(-1.0f), CCVector3DFill(1.0f)) };
            }
            break;
            
        case CCInputMapTypeControllerAxis:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCVector3D Joystick = CCControllerGetStateForJoystick3D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x, InputMap->controller.axis.y, InputMap->controller.axis.z);
                
                return (CCInputPressure3D){ .timestamp = Controller->timestamp, .pressure = Joystick };
            }
            
            return (CCInputPressure3D){ .timestamp = CCTimestamp(), .pressure = CCVector3DZero };
        }
            
        case CCInputMapTypeVirtualJoystick:
        {
            const CCInputPressure4D Joystick = CCInputGetVirtualJoystickState(InputMap);
            
            return (CCInputPressure3D){ .timestamp = Joystick.timestamp, .pressure = CCVector3DMake(Joystick.pressure.x, Joystick.pressure.y, Joystick.pressure.z) };

        }
            
        default:
            break;
    }
    
    const CCInputPressure2D State = CCInputGetPressure2D(InputMap);
    
    return (CCInputPressure3D){ .timestamp = State.timestamp, .pressure = CCVector3DMake(State.pressure.x, State.pressure.y, 0.0f) };
}

CCInputPressure4D CCInputGetPressure4D(const CCInputMap *InputMap)
{
    switch (InputMap->type)
    {
        case CCInputMapTypeGroup:
            if (!InputMap->group.wantsAllActive)
            {
                uint64_t Timestamp = 0;
                CCVector4D Pressure4 = CCVector4DZero;
                
                for (size_t Loop = 0, Count = InputMap->group.count; Loop < Count; Loop++)
                {
                    const CCInputPressure4D State = CCInputGetPressure4D(&InputMap->group.inputMaps[Loop]);
                    
                    Pressure4 = CCVector4Add(Pressure4, State.pressure);
                    
                    if (State.timestamp > Timestamp) Timestamp = State.timestamp;
                }
                
                return (CCInputPressure4D){ .timestamp = Timestamp, .pressure = CCVector4Clamp(Pressure4, CCVector4DFill(-1.0f), CCVector4DFill(1.0f)) };
            }
            break;
            
        case CCInputMapTypeControllerAxis:
        {
            const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
            
            if (CCControllerIsDevice(Controller, InputMap->controller.device))
            {
                const CCVector4D Joystick = CCControllerGetStateForJoystick4D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x, InputMap->controller.axis.y, InputMap->controller.axis.z, InputMap->controller.axis.w);
                
                return (CCInputPressure4D){ .timestamp = Controller->timestamp, .pressure = Joystick };
            }
            
            return (CCInputPressure4D){ .timestamp = CCTimestamp(), .pressure = CCVector4DZero };
        }
            
        case CCInputMapTypeVirtualJoystick:
            return CCInputGetVirtualJoystickState(InputMap);
            
        default:
            break;
    }
    
    const CCInputPressure3D State = CCInputGetPressure3D(InputMap);
    
    return (CCInputPressure4D){ .timestamp = State.timestamp, .pressure = CCVector4DMake(State.pressure.x, State.pressure.y, State.pressure.z, 0.0f) };
}

static CCInputState CCInputMapGetGroupState(const CCInputMap *InputGroupMap)
{
    _Bool AllActive = TRUE, OneActive = FALSE;
    double LatestActiveTimestamp = 0.0, LatestInactiveTimestamp = 0.0;
    
    for (size_t Loop = 0, Count = InputGroupMap->group.count; Loop < Count; Loop++)
    {
        const CCInputMap *InputMap = &InputGroupMap->group.inputMaps[Loop];
        
        switch (InputMap->type)
        {
            case CCInputMapTypeGroup:
            {
                const CCInputState ChildState = CCInputMapGetGroupState(InputMap);
                
                if (ChildState.active)
                {
                    OneActive = TRUE;
                    if (LatestActiveTimestamp < ChildState.timestamp) LatestActiveTimestamp = ChildState.timestamp;
                }
                
                else
                {
                    AllActive = FALSE;
                    if (LatestInactiveTimestamp < ChildState.timestamp) LatestInactiveTimestamp = ChildState.timestamp;
                }
                
                break;
            }
                
            case CCInputMapTypeKeyboard:
            {
                CCKeyboardState KeyState;
                
                if (InputMap->keyboard.isKeycode) KeyState = CCKeyboardGetStateForKeycode(InputMap->keyboard.keycode, InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
                else KeyState = CCKeyboardGetStateForCharacter(InputMap->keyboard.character, InputMap->keyboard.flags, InputMap->keyboard.ignoreModifier, InputMap->keyboard.repeats);
                
                if (KeyState.down)
                {
                    OneActive = TRUE;
                    if (LatestActiveTimestamp < KeyState.timestamp) LatestActiveTimestamp = KeyState.timestamp;
                }
                
                else
                {
                    AllActive = FALSE;
                    if (LatestInactiveTimestamp < KeyState.timestamp) LatestInactiveTimestamp = KeyState.timestamp;
                }
                
                break;
            }
                
            case CCInputMapTypeMouseButton:
            {
                const CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForButton(InputMap->mouse.button.code, InputMap->mouse.button.flags, InputMap->mouse.button.ignoreModifier);
                
                if (MouseButtonState.down)
                {
                    OneActive = TRUE;
                    if (LatestActiveTimestamp < MouseButtonState.timestamp) LatestActiveTimestamp = MouseButtonState.timestamp;
                }
                
                else
                {
                    AllActive = FALSE;
                    if (LatestInactiveTimestamp < MouseButtonState.timestamp) LatestInactiveTimestamp = MouseButtonState.timestamp;
                }
                
                break;
            }
                
            case CCInputMapTypeControllerAxis:
            {
                const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
                
                if (CCControllerIsDevice(Controller, InputMap->controller.device))
                {
                    const CCVector4D Joystick = CCControllerGetStateForJoystick4D(Controller, InputMap->controller.axis.resolution, InputMap->controller.axis.deadzone, InputMap->controller.axis.x, InputMap->controller.axis.y, InputMap->controller.axis.z, InputMap->controller.axis.w);
                    
                    if (CCVector4Equal(Joystick, CCVector4DZero))
                    {
                        AllActive = FALSE;
                        if (LatestInactiveTimestamp < Controller->timestamp) LatestInactiveTimestamp = Controller->timestamp;
                    }
                    
                    else
                    {
                        OneActive = TRUE;
                        if (LatestActiveTimestamp < Controller->timestamp) LatestActiveTimestamp = Controller->timestamp;
                    }
                }
                
                else AllActive = FALSE;
            }
                
            case CCInputMapTypeControllerButton:
            {
                const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
                
                if (CCControllerIsDevice(Controller, InputMap->controller.device))
                {
                    if (CCControllerGetStateForButton(Controller, InputMap->controller.button.index))
                    {
                        OneActive = TRUE;
                        if (LatestActiveTimestamp < Controller->timestamp) LatestActiveTimestamp = Controller->timestamp;
                    }
                    
                    else
                    {
                        AllActive = FALSE;
                        if (LatestInactiveTimestamp < Controller->timestamp) LatestInactiveTimestamp = Controller->timestamp;
                    }
                }
                
                else AllActive = FALSE;
            }
                
            case CCInputMapTypeControllerHat:
            {
                const CCController *Controller = CCControllerGetController(InputMap->controller.connection);
                
                if (CCControllerIsDevice(Controller, InputMap->controller.device))
                {
                    if (CCControllerGetStateForHat(Controller, InputMap->controller.hat.index) != CCControllerHatCenter)
                    {
                        OneActive = TRUE;
                        if (LatestActiveTimestamp < Controller->timestamp) LatestActiveTimestamp = Controller->timestamp;
                    }
                    
                    else
                    {
                        AllActive = FALSE;
                        if (LatestInactiveTimestamp < Controller->timestamp) LatestInactiveTimestamp = Controller->timestamp;
                    }
                }
                
                else AllActive = FALSE;
            }
                
            default:
                break;
        }
    }
        
    const _Bool Active = InputGroupMap->group.wantsAllActive ? AllActive : OneActive;
    
    return (CCInputState){ .timestamp = Active ? LatestActiveTimestamp : LatestInactiveTimestamp, .active = Active };
}

static CCInputPressure4D CCInputGetVirtualJoystickState(const CCInputMap *InputVirtualMap)
{
    uint64_t Timestamp = 0;
    CCVector4D Joystick = CCVector4DZero;
    const size_t Count = InputVirtualMap->virtual.joystick.count;
    
    for (size_t Loop = 0; Loop < Count; Loop += 2)
    {
        const CCInputPressure1D State = CCInputGetPressure1D(&InputVirtualMap->virtual.joystick.inputMaps[Loop]);
        
        Joystick.v[Loop / 2] = State.pressure;
        
        if (State.timestamp > Timestamp) Timestamp = State.timestamp;
    }
    
    for (size_t Loop = 1; Loop < Count; Loop += 2)
    {
        const CCInputPressure1D State = CCInputGetPressure1D(&InputVirtualMap->virtual.joystick.inputMaps[Loop]);
        
        Joystick.v[Loop / 2] -= State.pressure;
        
        if (State.timestamp > Timestamp) Timestamp = State.timestamp;
    }
    
    return (CCInputPressure4D){ .timestamp = Timestamp, .pressure = Joystick };
}
