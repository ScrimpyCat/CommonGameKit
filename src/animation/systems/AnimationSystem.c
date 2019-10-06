/*
 *  Copyright (c) 2015, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CC_QUICK_COMPILE
#include "AnimationSystem.h"
#include <threads.h>
#include "AnimationComponent.h"
#include "AnimationKeyframeComponent.h"
#include "AnimationInterpolateComponent.h"

static _Bool CCAnimationSystemTryLock(CCComponentSystemHandle *System);
static void CCAnimationSystemLock(CCComponentSystemHandle *System);
static void CCAnimationSystemUnlock(CCComponentSystemHandle *System);
static _Bool CCAnimationSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id);
static void CCAnimationSystemUpdate(CCComponentSystemHandle *System, double DeltaTime, CCCollection(CCComponent) Components);

static mtx_t Lock;
void CCAnimationSystemRegister(void)
{
    int err;
    if ((err = mtx_init(&Lock, mtx_plain | mtx_recursive)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create render system lock (%d)", err);
        exit(EXIT_FAILURE); //TODO: How should we handle this?
    }
    
    CCComponentSystemRegister(CC_ANIMATION_SYSTEM_ID, CCComponentSystemExecutionTypeRender, (CCComponentSystemUpdateCallback)CCAnimationSystemUpdate, NULL, CCAnimationSystemHandlesComponent, NULL, NULL, CCAnimationSystemTryLock, CCAnimationSystemLock, CCAnimationSystemUnlock);
}

void CCAnimationSystemDeregister(void)
{
    mtx_destroy(&Lock);
    
    CCComponentSystemDeregister(CC_ANIMATION_SYSTEM_ID, CCComponentSystemExecutionTypeRender);
}

static _Bool CCAnimationSystemTryLock(CCComponentSystemHandle *System)
{
    int err = mtx_trylock(&Lock);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
    
    return err == thrd_success;
}

static void CCAnimationSystemLock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_lock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock animation system (%d)", err);
    }
}

static void CCAnimationSystemUnlock(CCComponentSystemHandle *System)
{
    int err;
    if ((err = mtx_unlock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock animation system (%d)", err);
    }
}

static _Bool CCAnimationSystemHandlesComponent(CCComponentSystemHandle *System, CCComponentID id)
{
    return (id & CC_COMPONENT_SYSTEM_FLAG_MASK) == CC_ANIMATION_COMPONENT_FLAG;
}

static void CCAnimationSystemUpdate(CCComponentSystemHandle *System, double DeltaTime, CCCollection(CCComponent) Components)
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
