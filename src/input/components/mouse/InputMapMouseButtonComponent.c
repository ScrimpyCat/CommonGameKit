//
//  InputMapMouseButtonComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapMouseButtonComponent.h"

const char * const CCInputMapMouseButtonComponentName = "input_map_mouse_button";

void CCInputMapMouseButtonComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID, CCInputMapMouseButtonComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapMouseButtonComponentClass), CCInputMapMouseButtonComponentInitialize, CCInputMapMouseButtonComponentDeallocate);
}

void CCInputMapMouseButtonComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID);
}
