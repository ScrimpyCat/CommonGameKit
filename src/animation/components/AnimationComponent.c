//
//  AnimationComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 16/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "AnimationComponent.h"

const char * const CCAnimationComponentName = "animation";

void CCAnimationComponentRegister(void)
{
    CCComponentRegister(CC_ANIMATION_COMPONENT_ID, CCAnimationComponentName, CC_STD_ALLOCATOR, sizeof(CCAnimationComponentClass), CCAnimationComponentInitialize, CCAnimationComponentDeallocate);
}

void CCAnimationComponentDeregister(void)
{
    CCComponentDeregister(CC_ANIMATION_COMPONENT_ID);
}
