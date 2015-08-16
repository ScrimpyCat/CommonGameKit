//
//  InputMapMousePositionComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapMousePositionComponent.h"

const char * const CCInputMapMousePositionComponentName = "input_map_mouse_position";

void CCInputMapMousePositionComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID, CCInputMapMousePositionComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapMousePositionComponentClass), CCInputMapMousePositionComponentInitialize, CCInputMapMousePositionComponentDeallocate);
}

void CCInputMapMousePositionComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID);
}
