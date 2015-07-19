//
//  InputMapGroupComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 10/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputMapGroupComponent.h"

const char * const CCInputMapGroupComponentName = "input_map_group";

void CCInputMapGroupComponentRegister(void)
{
    CCComponentRegister(CC_INPUT_MAP_GROUP_COMPONENT_ID, CCInputMapGroupComponentName, CC_STD_ALLOCATOR, sizeof(CCInputMapGroupComponentClass), CCInputMapGroupComponentInitialize, CCInputMapGroupComponentDestroy);
}

void CCInputMapGroupComponentDeregister(void)
{
    CCComponentDeregister(CC_INPUT_MAP_GROUP_COMPONENT_ID);
}

void CCInputMapGroupComponentElementDestructor(CCCollection Collection, CCComponent *Element)
{
    CCComponentDestroy(*Element);
}
