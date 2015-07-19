//
//  InputMapControllerButtonComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 19/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapControllerButtonComponent.h"

const char * const CCInputMapControllerButtonComponentName = "input_map_controller_button";

void CCInputMapControllerButtonComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID, CCInputMapControllerButtonComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapControllerButtonComponentClass), CCInputMapControllerButtonComponentInitialize, CCInputMapControllerComponentDestroy);
}

void CCInputMapControllerButtonComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID);
}
