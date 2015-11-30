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

#include "InputSystem.h"
#include "Window.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Controller.h"
#include "InputMapKeyboardComponent.h"
#include "InputMapGroupComponent.h"
#include "InputMapMousePositionComponent.h"
#include "InputMapMouseButtonComponent.h"
#include "InputMapMouseScrollComponent.h"
#include "InputMapMouseDropComponent.h"
#include "InputMapControllerAxesComponent.h"
#include "InputMapControllerButtonComponent.h"

typedef struct {
    double timestamp;
    CCInputState active;
} CCInputMapGroupState;

static void CCWindowFocus(GLFWwindow *Window, int Focus);
static _Bool CCInputSystemHandlesComponent(CCComponentID id);
static void CCInputSystemUpdate(void *Context, CCCollection Components);
static CCInputMapGroupState CCInputSystemGetGroupStateForComponent(CCComponent Component);
static CCVector2D CCInputSystemGetSimulatedGroupPressure2(CCComponent Component);
static CCVector3D CCInputSystemGetSimulatedGroupPressure3(CCComponent Component);

void CCInputSystemRegister(void)
{
    CCKeyboardStateReset();
    
    glfwSetWindowFocusCallback(CCWindow, CCWindowFocus);
    
    glfwSetKeyCallback(CCWindow, CCKeyboardInput);
    glfwSetCharModsCallback(CCWindow, CCKeyboardCharInput);
    
    glfwSetDropCallback(CCWindow, CCMouseDropInput);
    glfwSetScrollCallback(CCWindow, CCMouseScrollInput);
    glfwSetMouseButtonCallback(CCWindow, CCMouseButtonInput);
    glfwSetCursorPosCallback(CCWindow, CCMousePositionInput);
    glfwSetCursorEnterCallback(CCWindow, CCMouseEnterInput);
    
    CCControllerSetup();
    
    CCComponentSystemRegister(CC_INPUT_SYSTEM_ID, CCComponentSystemExecutionTypeInput, CCInputSystemUpdate, CCInputSystemHandlesComponent, NULL, NULL, NULL, NULL, NULL);
}

void CCInputSystemDeregister(void)
{
    CCComponentSystemDeregister(CC_INPUT_SYSTEM_ID, CCComponentSystemExecutionTypeInput);
}

static CCCollection CCInputSystemGetComponentsInCollection(CCCollection Group, CCInputMapType InputType)
{
    /*
     TODO: Types could be cleaned up, and possibly incorporate base components for the different types of inputs
     e.g. single state vs variable state inputs
     */
    CCAssertLog(InputType != CCInputMapTypeNone, "Should not try find components for this type");
    CCAssertLog(InputType != CCInputMapTypeController, "Should not try find components for this type");
    _Static_assert(CCInputMapTypeKeyboard == 2 &&
                   CCInputMapTypeMousePosition == 3 &&
                   CCInputMapTypeMouseButton == 4 &&
                   CCInputMapTypeMouseScroll == 5 &&
                   CCInputMapTypeMouseDrop == 6 &&
                   CCInputMapTypeController == 7 &&
                   CCInputMapTypeControllerAxes == 8 &&
                   CCInputMapTypeControllerButton == 9 &&
                   CCInputMapTypeGroup == 10, "Expects input types to have these values");
    
    CCComponentID Type = (CCComponentID[]){
        CC_INPUT_MAP_KEYBOARD_COMPONENT_ID,
        CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID,
        CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID,
        CC_INPUT_MAP_MOUSE_SCROLL_COMPONENT_ID,
        CC_INPUT_MAP_MOUSE_DROP_COMPONENT_ID,
        0,
        CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID,
        CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID,
        CC_INPUT_MAP_GROUP_COMPONENT_ID
    }[InputType - CCInputMapTypeKeyboard];
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Group, &Enumerator);
    
    CCCollection InputComponents = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating | CCCollectionHintHeavyInserting, sizeof(CCComponent), NULL);
    for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCComponentID id = CCComponentGetID(*Component);
        if (id == Type)
        {
            CCCollectionInsertElement(InputComponents, Component);
        }
        
        else if ((id & CCInputMapTypeMask) == CCInputMapTypeGroup) //TODO: Should we retrieve nested groups if asking for groups? (e.g. remove the 'else')
        {
            CCCollection Children = CCInputSystemGetComponentsInCollection(CCInputMapGroupComponentGetInputMaps(*Component), InputType);
            CCCollectionInsertCollection(InputComponents, Children, NULL); //TODO: consume insert
            CCCollectionDestroy(Children);
        }
    }
    
    return InputComponents;
}

CCCollection CCInputSystemGetComponents(CCInputMapType InputType)
{
    return CCInputSystemGetComponentsInCollection(CCComponentSystemGetComponentsForSystem(CC_INPUT_SYSTEM_ID), InputType);
}

static CCComponent CCInputSystemFindComponentForActionInCollection(CCCollection Group, const char *Action)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Group, &Enumerator);
    
    for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCComponentID id = CCComponentGetID(*Component);
        if ((id & CC_INPUT_COMPONENT_FLAG) == CC_INPUT_COMPONENT_FLAG)
        {
            const char *InputAction = CCInputMapComponentGetAction(*Component);
            if ((InputAction) && (!strcmp(InputAction, Action))) return *Component;
            else if (id == CC_INPUT_MAP_GROUP_COMPONENT_ID)
            {
                CCComponent Input = CCInputSystemFindComponentForActionInCollection(CCInputMapGroupComponentGetInputMaps(*Component), Action);
                if (Input) return Input;
            }
        }
    }
    
    return NULL;
}

CCInputState CCInputSystemGetStateForAction(CCEntity Entity, const char *Action)
{
    CCComponent Input = CCInputSystemFindComponentForActionInCollection(CCEntityGetComponents(Entity), Action);
    if (Input)
    {
        switch (CCComponentGetID(Input))
        {
            case CC_INPUT_MAP_KEYBOARD_COMPONENT_ID:
                return CCKeyboardGetStateForComponent(Input).down ? CCInputStateActive : CCInputStateInactive;
                
            case CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID:
                return CCMouseButtonGetStateForComponent(Input).down ? CCInputStateActive : CCInputStateInactive;
                
            case CC_INPUT_MAP_GROUP_COMPONENT_ID:
                return CCInputSystemGetGroupStateForComponent(Input).active;
                
            case CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID:
                return CCControllerGetStateForComponent(Input).button.down ? CCInputStateActive : CCInputStateInactive;
                
            case CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID:
            {
                const CCControllerAxesState ControllerAxesState = CCControllerGetStateForComponent(Input).axes;
                return (ControllerAxesState.position.x != 0.0f) || (ControllerAxesState.position.y != 0.0f) || (ControllerAxesState.position.z != 0.0f);
            }
                
            default:
                break;
        }
    }
    
    return CCInputStateInactive;
}

static float CCInputSystemPressureForBinaryInput(CCInputState State, double Timestamp, float Ramp)
{
    Ramp *= (glfwGetTime() - Timestamp);
    Ramp = Ramp == 0.0f ? 1.0f : Ramp;
    return CCClampf((State == CCInputStateActive ? 0.0f + Ramp : 1.0f - Ramp), 0.0f, 1.0f);
}

float CCInputSystemGetPressureForAction(CCEntity Entity, const char *Action)
{
    CCComponent Input = CCInputSystemFindComponentForActionInCollection(CCEntityGetComponents(Entity), Action);
    if (Input)
    {
        switch (CCComponentGetID(Input))
        {
            case CC_INPUT_MAP_KEYBOARD_COMPONENT_ID:
            {
                CCKeyboardState KeyState = CCKeyboardGetStateForComponent(Input);
                return CCInputSystemPressureForBinaryInput(KeyState.down ? CCInputStateActive : CCInputStateInactive, KeyState.timestamp, CCInputMapKeyboardComponentGetRamp(Input));
            }
                
            case CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID:
            {
                CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForComponent(Input);
                return CCInputSystemPressureForBinaryInput(MouseButtonState.down ? CCInputStateActive : CCInputStateInactive, MouseButtonState.timestamp, CCInputMapMouseButtonComponentGetRamp(Input));
            }
                
            case CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID:
            {
                const CCControllerButtonState ControllerButtonState = CCControllerGetStateForComponent(Input).button;
                return CCInputSystemPressureForBinaryInput(ControllerButtonState.down ? CCInputStateActive : CCInputStateInactive, ControllerButtonState.timestamp, CCInputMapControllerButtonComponentGetRamp(Input));
            }
                
            case CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID:
            {
                const CCControllerAxesState ControllerAxesState = CCControllerGetStateForComponent(Input).axes;
                return ControllerAxesState.position.x;
            }
                
            case CC_INPUT_MAP_GROUP_COMPONENT_ID:
            {
                CCInputMapGroupState GroupState = CCInputSystemGetGroupStateForComponent(Input);
                return CCInputSystemPressureForBinaryInput(GroupState.active, GroupState.timestamp, 0.0f); //TODO: would we want groups to have value ramps?
            }
                
            default:
                break;
        }
    }
    
    return 0.0f;
}

CCVector2D CCInputSystemGetPressure2ForAction(CCEntity Entity, const char *Action)
{
    CCComponent Input = CCInputSystemFindComponentForActionInCollection(CCEntityGetComponents(Entity), Action);
    if (Input)
    {
        switch (CCComponentGetID(Input))
        {
            case CC_INPUT_MAP_GROUP_COMPONENT_ID:
                return CCInputSystemGetSimulatedGroupPressure2(Input);
                
            case CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID:
            {
                const CCControllerAxesState ControllerAxesState = CCControllerGetStateForComponent(Input).axes;
                return CCVector2DMake(ControllerAxesState.position.x, ControllerAxesState.position.y);
            }
                
            default:
                break;
        }
    }
    
    return CCVector2DZero;
}

CCVector3D CCInputSystemGetPressure3ForAction(CCEntity Entity, const char *Action)
{
    CCComponent Input = CCInputSystemFindComponentForActionInCollection(CCEntityGetComponents(Entity), Action);
    if (Input)
    {
        switch (CCComponentGetID(Input))
        {
            case CC_INPUT_MAP_GROUP_COMPONENT_ID:
                return CCInputSystemGetSimulatedGroupPressure3(Input);
                
            case CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID:
                return CCControllerGetStateForComponent(Input).axes.position;
                
            default:
                break;
        }
    }
    
    return CCVector3DZero;
}

static void CCWindowFocus(GLFWwindow *Window, int Focus)
{
    if (!Focus)
    {
        CCKeyboardStateReset();
    }
}

static _Bool CCInputSystemHandlesComponent(CCComponentID id)
{
    return (id & 0x7f000000) == CC_INPUT_COMPONENT_FLAG;
}

static void CCInputSystemUpdate(void *Context, CCCollection Components)
{
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(CC_INPUT_SYSTEM_ID));
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(CC_INPUT_SYSTEM_ID));
    
    CCControllerUpdateState();
}

static CCInputMapGroupState CCInputSystemGetGroupStateForComponent(CCComponent Component)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCInputMapGroupComponentGetInputMaps(Component), &Enumerator);
    
    CCInputState AllActive = CCInputStateActive, OneActive = CCInputStateInactive;
    double LatestActiveTimestamp = 0.0, LatestInactiveTimestamp = 0.0;
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        switch (CCComponentGetID(*Input))
        {
            case CC_INPUT_MAP_KEYBOARD_COMPONENT_ID:
            {
                const CCKeyboardState KeyState = CCKeyboardGetStateForComponent(*Input);
                if (KeyState.down)
                {
                    OneActive = CCInputStateActive;
                    if (LatestActiveTimestamp < KeyState.timestamp) LatestActiveTimestamp = KeyState.timestamp;
                }
                
                else
                {
                    AllActive = CCInputStateInactive;
                    if (LatestInactiveTimestamp < KeyState.timestamp) LatestInactiveTimestamp = KeyState.timestamp;
                }
                
                break;
            }
                
            case CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID:
            {
                const CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForComponent(Input);
                if (MouseButtonState.down)
                {
                    OneActive = CCInputStateActive;
                    if (LatestActiveTimestamp < MouseButtonState.timestamp) LatestActiveTimestamp = MouseButtonState.timestamp;
                }
                
                else
                {
                    AllActive = CCInputStateInactive;
                    if (LatestInactiveTimestamp < MouseButtonState.timestamp) LatestInactiveTimestamp = MouseButtonState.timestamp;
                }
                
                break;
            }
                
            case CC_INPUT_MAP_GROUP_COMPONENT_ID:
            {
                const CCInputMapGroupState ChildState = CCInputSystemGetGroupStateForComponent(*Input);
                if (ChildState.active)
                {
                    OneActive = CCInputStateActive;
                    if (LatestActiveTimestamp < ChildState.timestamp) LatestActiveTimestamp = ChildState.timestamp;
                }
                
                else
                {
                    AllActive = CCInputStateInactive;
                    if (LatestInactiveTimestamp < ChildState.timestamp) LatestInactiveTimestamp = ChildState.timestamp;
                }
                
                break;
            }
                
            case CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID:
            {
                const CCControllerButtonState ControllerButtonState = CCControllerGetStateForComponent(Input).button;
                if (ControllerButtonState.down)
                {
                    OneActive = CCInputStateActive;
                    if (LatestActiveTimestamp < ControllerButtonState.timestamp) LatestActiveTimestamp = ControllerButtonState.timestamp;
                }
                
                else
                {
                    AllActive = CCInputStateInactive;
                    if (LatestInactiveTimestamp < ControllerButtonState.timestamp) LatestInactiveTimestamp = ControllerButtonState.timestamp;
                }
                
                break;
            }
                
            default:
                break;
        }
    }
    
    const CCInputState Active = CCInputMapGroupComponentGetWantsAllActive(Component) ? AllActive : OneActive;
    return (CCInputMapGroupState){ .timestamp = Active ? LatestActiveTimestamp : LatestInactiveTimestamp, .active = Active };
}

static CCVector2D CCInputSystemGetSimulatedGroupPressure2(CCComponent Component)
{
    CCOrderedCollection Inputs = CCInputMapGroupComponentGetInputMaps(Component);
    const size_t Count = CCCollectionGetCount(Inputs);
    
    CCAssertLog(Count == 2 || Count == 4, "To correctly simulate a 2 axis input device, there must either be 2 or 4 single axis inputs");
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Inputs, &Enumerator);
    
    float Pressure[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        const size_t Index = CCOrderedCollectionGetIndex(Inputs, CCCollectionEnumeratorGetEntry(&Enumerator));
        switch (CCComponentGetID(*Input))
        {
            case CC_INPUT_MAP_KEYBOARD_COMPONENT_ID:
            {
                const CCKeyboardState KeyState = CCKeyboardGetStateForComponent(*Input);
                Pressure[Index] = CCInputSystemPressureForBinaryInput(KeyState.down ? CCInputStateActive : CCInputStateInactive, KeyState.timestamp, CCInputMapKeyboardComponentGetRamp(*Input));
                break;
            }
                
            case CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID: //TODO: Should create a base component for binary single axis inputs
            {
                const CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForComponent(*Input);
                Pressure[Index] = CCInputSystemPressureForBinaryInput(MouseButtonState.down ? CCInputStateActive : CCInputStateInactive, MouseButtonState.timestamp, CCInputMapMouseButtonComponentGetRamp(*Input));
                break;
            }
                
            case CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID:
            {
                const CCControllerButtonState ControllerButtonState = CCControllerGetStateForComponent(Input).button;
                Pressure[Index] = CCInputSystemPressureForBinaryInput(ControllerButtonState.down ? CCInputStateActive : CCInputStateInactive, ControllerButtonState.timestamp, CCInputMapControllerButtonComponentGetRamp(*Input));
                break;
            }
                
            default:
                CCAssertLog(0, "Must contain only single axis inputs");
                break;
        }
    }
    
    return Count == 2 ? CCVector2DMake(Pressure[0], Pressure[1]) : CCVector2DMake(Pressure[2] - Pressure[0], Pressure[3] - Pressure[1]);
}

static CCVector3D CCInputSystemGetSimulatedGroupPressure3(CCComponent Component)
{
    CCOrderedCollection Inputs = CCInputMapGroupComponentGetInputMaps(Component);
    const size_t Count = CCCollectionGetCount(Inputs);
    
    CCAssertLog(Count == 3 || Count == 6, "To correctly simulate a 3 axis input device, there must either be 3 or 6 single axis inputs");
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Inputs, &Enumerator);
    
    float Pressure[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    for (CCComponent *Input = CCCollectionEnumeratorGetCurrent(&Enumerator); Input; Input = CCCollectionEnumeratorNext(&Enumerator))
    {
        const size_t Index = CCOrderedCollectionGetIndex(Inputs, CCCollectionEnumeratorGetEntry(&Enumerator));
        switch (CCComponentGetID(*Input))
        {
            case CC_INPUT_MAP_KEYBOARD_COMPONENT_ID:
            {
                CCKeyboardState KeyState = CCKeyboardGetStateForComponent(*Input);
                Pressure[Index] = CCInputSystemPressureForBinaryInput(KeyState.down ? CCInputStateActive : CCInputStateInactive, KeyState.timestamp, CCInputMapKeyboardComponentGetRamp(*Input));
                break;
            }
                
            case CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID: //TODO: Should create a base component for binary single axis inputs
            {
                CCMouseButtonState MouseButtonState = CCMouseButtonGetStateForComponent(*Input);
                Pressure[Index] = CCInputSystemPressureForBinaryInput(MouseButtonState.down ? CCInputStateActive : CCInputStateInactive, MouseButtonState.timestamp, CCInputMapMouseButtonComponentGetRamp(*Input));
                break;
            }
                
            case CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID:
            {
                const CCControllerButtonState ControllerButtonState = CCControllerGetStateForComponent(Input).button;
                Pressure[Index] = CCInputSystemPressureForBinaryInput(ControllerButtonState.down ? CCInputStateActive : CCInputStateInactive, ControllerButtonState.timestamp, CCInputMapControllerButtonComponentGetRamp(*Input));
                break;
            }
                
            default:
                CCAssertLog(0, "Must contain only single axis inputs");
                break;
        }
    }
    
    return Count == 3 ? CCVector3DMake(Pressure[0], Pressure[1], Pressure[2]) : CCVector3DMake(Pressure[3] - Pressure[0], Pressure[4] - Pressure[1], Pressure[5] - Pressure[2]);
}
