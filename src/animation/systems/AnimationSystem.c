//
//  AnimationSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 16/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "AnimationSystem.h"
#include "tinycthread.h"
#include <CommonC/Common.h>
#include "AnimationComponent.h"
#include "AnimationKeyframeComponent.h"
#include "AnimationInterpolateComponent.h"

static _Bool CCAnimationSystemTryLock(void);
static void CCAnimationSystemLock(void);
static void CCAnimationSystemUnlock(void);
static _Bool CCAnimationSystemHandlesComponent(CCComponentID id);
static void CCAnimationSystemUpdate(double DeltaTime, CCCollection Components);

static mtx_t Lock;
void CCAnimationSystemRegister(void)
{
    int err;
    if ((err = mtx_init(&Lock, mtx_plain | mtx_recursive)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create render system lock (%d)", err);
        exit(EXIT_FAILURE); //TODO: How should we handle this?
    }
    
    CCComponentSystemRegister(CC_ANIMATION_SYSTEM_ID, CCComponentSystemExecutionTypeRender, (CCComponentSystemUpdateCallback)CCAnimationSystemUpdate, CCAnimationSystemHandlesComponent, NULL, NULL, CCAnimationSystemTryLock, CCAnimationSystemLock, CCAnimationSystemUnlock);
}

void CCAnimationSystemDeregister(void)
{
    mtx_destroy(&Lock);
    
    CCComponentSystemDeregister(CC_ANIMATION_SYSTEM_ID, CCComponentSystemExecutionTypeRender);
}

static _Bool CCAnimationSystemTryLock(void)
{
    int err = mtx_trylock(&Lock);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
    
    return err == thrd_success;
}

static void CCAnimationSystemLock(void)
{
    int err;
    if ((err = mtx_lock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
}

static void CCAnimationSystemUnlock(void)
{
    int err;
    if ((err = mtx_unlock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock animation system (%d)", err);
    }
}

static _Bool CCAnimationSystemHandlesComponent(CCComponentID id)
{
    return (id & 0x7f000000) == CC_ANIMATION_COMPONENT_FLAG;
}

static void CCAnimationSystemUpdate(double DeltaTime, CCCollection Components)
{
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(CC_ANIMATION_SYSTEM_ID));
    
    CC_COLLECTION_FOREACH(CCComponent, Animation, Components)
    {
        if (CCAnimationComponentGetPlaying(Animation))
        {
            double Position = CCAnimationComponentGetPosition(Animation) + DeltaTime;
            switch (CCComponentGetID(Animation) & CCAnimationTypeMask)
            {
                case CCAnimationTypeNone:
                    break;
                    
                case CCAnimationTypeKeyframe:
                case CCAnimationTypeInterpolate:
                {
                    if (CCAnimationKeyframeComponentGetIsFinished(Animation)) break;
                    
                    CCOrderedCollection Data = CCAnimationKeyframeComponentGetData(Animation);
                    const size_t FrameCount = Data ? CCCollectionGetCount(Data) : 0;
                    size_t Index = CCAnimationKeyframeComponentGetIndex(Animation);
                    
                    for (double FrameTime = CCAnimationKeyframeComponentGetCurrentFrame(Animation); Position >= FrameTime; FrameTime = CCAnimationKeyframeComponentGetCurrentFrame(Animation))
                    {
                        Position -= FrameTime;
                        CCAnimationKeyframeDirection Direction = CCAnimationKeyframeComponentGetDirection(Animation);
                        if (((Index + Direction) >= FrameCount) || ((intmax_t)(Index + Direction) < 0))
                        {
                            switch (CCAnimationComponentGetLoop(Animation))
                            {
                                case CCAnimationLoopOnce:
                                    CCAnimationKeyframeComponentSetIsFinished(Animation, TRUE);
                                    break;
                                    
                                case CCAnimationLoopRepeatRoll:
                                    Index = 0;
                                    break;
                                    
                                case CCAnimationLoopRepeatFlip:
                                    CCAnimationKeyframeComponentSetDirection(Animation, (Direction *= CCAnimationKeyframeDirectionReverse));
                                    Index += Direction;
                                    break;
                            }
                        }
                        
                        else Index += Direction;
                        
                        CCAnimationKeyframeComponentSetIndex(Animation, Index);
                    }
                    break;
                }
            }
            
            CCAnimationComponentSetPosition(Animation, Position);
        }
    }
    
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(CC_ANIMATION_SYSTEM_ID));
}
