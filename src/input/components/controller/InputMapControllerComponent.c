//
//  InputMapControllerComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 13/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapControllerComponent.h"

const char * const CCInputMapControllerComponentName = "input_map_controller";

void CCInputMapControllerComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_CONTROLLER_COMPONENT_ID, CCInputMapControllerComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapControllerComponentClass), CCInputMapControllerComponentInitialize, CCInputMapControllerComponentDeallocate);
}

void CCInputMapControllerComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_CONTROLLER_COMPONENT_ID);
}
