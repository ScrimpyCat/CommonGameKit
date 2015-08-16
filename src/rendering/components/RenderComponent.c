//
//  RenderComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 28/06/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "RenderComponent.h"

const char * const CCRenderComponentName = "render";

void CCRenderComponentRegister(void)
{
    CCComponentRegister(CC_RENDER_COMPONENT_ID, CCRenderComponentName, CC_STD_ALLOCATOR, sizeof(CCRenderComponentClass), CCRenderComponentInitialize, CCRenderComponentDeallocate);
}

void CCRenderComponentDeregister(void)
{
    CCComponentDeregister(CC_RENDER_COMPONENT_ID);
}
