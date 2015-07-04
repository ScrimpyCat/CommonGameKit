//
//  InputSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputSystem.h"
#include "Window.h"
#include "Keyboard.h"
#include "InputMapKeyboardComponent.h"

static void CCWindowFocus(GLFWwindow *Window, int Focus);
static _Bool CCInputSystemHandlesComponent(CCComponentID id);
static void CCInputSystemUpdate(void *Context, CCCollection Components);

void CCInputSystemRegister(void)
{
    glfwSetWindowFocusCallback(CCWindow, CCWindowFocus);
    glfwSetKeyCallback(CCWindow, CCKeyboardInput);
    glfwSetCharModsCallback(CCWindow, CCKeyboardCharInput);
    
    CCComponentSystemRegister(CC_INPUT_SYSTEM_ID, CCComponentSystemExecutionTypeInput, CCInputSystemUpdate, CCInputSystemHandlesComponent, NULL, NULL, NULL, NULL, NULL);
}

void CCInputSystemDeregister(void)
{
    CCComponentSystemDeregister(CC_INPUT_SYSTEM_ID, CCComponentSystemExecutionTypeInput);
}

CCCollection CCInputSystemGetComponents(CCInputMapType InputType)
{
    CCAssertLog(InputType != CCInputMapTypeNone, "Should not try find components for this type");
    _Static_assert(CCInputMapTypeKeyboard == 2 &&
                   CCInputMapTypeMouse == 3 &&
                   CCInputMapTypeController == 4, "Expects input types to have these values");
    
    CCComponentID Type = (CCComponentID[]){ CC_INPUT_MAP_KEYBOARD_COMPONENT_ID }[InputType - CCInputMapTypeKeyboard];
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCComponentSystemGetComponentsForSystem(CC_INPUT_SYSTEM_ID), &Enumerator);
    
    CCCollection InputComponents = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintHeavyEnumerating | CCCollectionHintHeavyInserting, sizeof(CCComponent), NULL);
    for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCComponentID id = CCComponentGetID(*Component);
        if (id == Type)
        {
            CCCollectionInsertElement(InputComponents, Component);
        }
        
        //else id == input map group
    }
    
    return InputComponents;
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
}
