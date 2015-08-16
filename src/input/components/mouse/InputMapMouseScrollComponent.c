//
//  InputMapMouseScrollComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapMouseScrollComponent.h"

const char * const CCInputMapMouseScrollComponentName = "input_map_mouse_scroll";

void CCInputMapMouseScrollComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_MOUSE_SCROLL_COMPONENT_ID, CCInputMapMouseScrollComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapMouseScrollComponentClass), CCInputMapMouseScrollComponentInitialize, CCInputMapMouseScrollComponentDeallocate);
}

void CCInputMapMouseScrollComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_MOUSE_SCROLL_COMPONENT_ID);
}
