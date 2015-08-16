//
//  AnimationSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 16/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_AnimationSystem_h
#define Blob_Game_AnimationSystem_h

#include "ComponentSystem.h"

#define CC_ANIMATION_SYSTEM_ID 31
#define CC_ANIMATION_COMPONENT_FLAG 0x3000000

typedef enum {
    CCAnimationTypeMask = 0xff,
    CCAnimationTypeNone = 1,
    CCAnimationTypeKeyframe,
    CCAnimationTypeInterpolate
} CCAnimationType;

typedef enum {
    CCAnimationLoopOnce,
    CCAnimationLoopRepeatRoll,
    CCAnimationLoopRepeatFlip
} CCAnimationLoop;

void CCAnimationSystemRegister(void);
void CCAnimationSystemDeregister(void);

#endif
