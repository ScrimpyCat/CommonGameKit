//
//  InputSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputSystem_h
#define Blob_Game_InputSystem_h

#include "ComponentSystem.h"
#include "Keyboard.h"

//Note: Currently just a demo system

#define CC_INPUT_SYSTEM_ID 21
#define CC_INPUT_COMPONENT_FLAG 0x2000000

typedef enum {
    CCInputMapTypeNone = 1,
    CCInputMapTypeKeyboard,
    CCInputMapTypeMouse,
    CCInputMapTypeController
} CCInputMapType;

typedef void (*CCInputMapKeyboardCallback)(CCComponent Component, CCKeyboardMap State);

void CCInputSystemRegister(void);
void CCInputSystemDeregister(void);

/*!
 * @brief Get the input components registered by the system for a certain type.
 * @param InputType The type of components to retrieve.
 * @return The list of components. Note: This collection must be destroyed.
 */
CCCollection CCInputSystemGetComponents(CCInputMapType InputType);

#endif
