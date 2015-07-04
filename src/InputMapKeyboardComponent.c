//
//  InputMapKeyboardComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapKeyboardComponent.h"

const char * const CCInputMapKeyboardComponentName = "input_map_keyboard";

void CCInputMapKeyboardComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, CCInputMapKeyboardComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapKeyboardComponentClass), CCInputMapKeyboardComponentInitialize);
}

void CCInputMapKeyboardComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_KEYBOARD_COMPONENT_ID);
}
