//
//  AnimationKeyframeComponent.c
//  Blob Game
//
//  Created by Stefan Johnson on 16/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "AnimationKeyframeComponent.h"

const char * const CCAnimationKeyframeComponentName = "animation_keyframe";

void CCAnimationKeyframeComponentRegister(void)
{
    CCComponentRegister(CC_ANIMATION_KEYFRAME_COMPONENT_ID, CCAnimationKeyframeComponentName, CC_STD_ALLOCATOR, sizeof(CCAnimationKeyframeComponentClass), CCAnimationKeyframeComponentInitialize, CCAnimationKeyframeComponentDeallocate);
}

void CCAnimationKeyframeComponentDeregister(void)
{
    CCComponentDeregister(CC_ANIMATION_KEYFRAME_COMPONENT_ID);
}
