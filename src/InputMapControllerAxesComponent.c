//
//  InputMapControllerAxesComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 12/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapControllerAxesComponent.h"

const char * const CCInputMapControllerAxesComponentName = "input_map_controller_axes";

void CCInputMapControllerAxesComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID, CCInputMapControllerAxesComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapControllerAxesComponentClass), CCInputMapControllerAxesComponentInitialize, CCInputMapControllerComponentDestroy);
}

void CCInputMapControllerAxesComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID);
}
