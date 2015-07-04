//
//  InputMapComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapComponent.h"

const char * const CCInputMapComponentName = "input_map";

void CCInputMapComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_COMPONENT_ID, CCInputMapComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapComponentClass), CCInputMapComponentInitialize);
}

void CCInputMapComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_COMPONENT_ID);
}
