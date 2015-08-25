//
//  AnimationInterpolateComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 17/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "AnimationInterpolateComponent.h"

const char * const CCAnimationInterpolateComponentName = "animation_interpolate";

void CCAnimationInterpolateComponentRegister(void)
{
    CCComponentRegister(CC_ANIMATION_INTERPOLATE_COMPONENT_ID, CCAnimationInterpolateComponentName, CC_STD_ALLOCATOR, sizeof(CCAnimationInterpolateComponentClass), CCAnimationInterpolateComponentInitialize, CCAnimationInterpolateComponentDeallocate);
}

void CCAnimationInterpolateComponentDeregister(void)
{
    CCComponentDeregister(CC_ANIMATION_INTERPOLATE_COMPONENT_ID);
}
