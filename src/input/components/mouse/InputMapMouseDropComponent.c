//
//  InputMapMouseDropComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapMouseDropComponent.h"

const char * const CCInputMapMouseDropComponentName = "input_map_mouse_drop";

void CCInputMapMouseDropComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_MOUSE_DROP_COMPONENT_ID, CCInputMapMouseDropComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapMouseDropComponentClass), CCInputMapMouseDropComponentInitialize, CCInputMapMouseDropComponentDeallocate);
}

void CCInputMapMouseDropComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_MOUSE_DROP_COMPONENT_ID);
}
