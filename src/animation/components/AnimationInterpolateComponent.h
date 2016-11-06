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

#ifndef CommonGameKit_AnimationInterpolateComponent_h
#define CommonGameKit_AnimationInterpolateComponent_h

#include "AnimationKeyframeComponent.h"

#define CC_ANIMATION_INTERPOLATE_COMPONENT_ID (CCAnimationTypeInterpolate | CC_ANIMATION_COMPONENT_FLAG)

extern const char * const CCAnimationInterpolateComponentName;

typedef void (*CCAnimationInterpolator)(void *Previous, void *Next, double Time, void *Result);

typedef struct {
    CC_COMPONENT_INHERIT(CCAnimationKeyframeComponentClass);
    CCAnimationInterpolator interpolator;
    void *result;
} CCAnimationInterpolateComponentClass, *CCAnimationInterpolateComponentPrivate;


void CCAnimationInterpolateComponentRegister(void);
void CCAnimationInterpolateComponentDeregister(void);


static inline void CCAnimationInterpolateComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCAnimationKeyframeComponentInitialize(Component, id);
    ((CCAnimationInterpolateComponentPrivate)Component)->interpolator = NULL;
    ((CCAnimationInterpolateComponentPrivate)Component)->result = NULL;
}

static inline void CCAnimationInterpolateComponentDeallocate(CCComponent Component)
{
    CC_SAFE_Free(((CCAnimationInterpolateComponentPrivate)Component)->result);
    
    CCAnimationKeyframeComponentDeallocate(Component);
}

static inline CCAnimationInterpolator CCAnimationInterpolateComponentGetInterpolator(CCComponent Component)
{
    return ((CCAnimationInterpolateComponentPrivate)Component)->interpolator;
}

static inline void CCAnimationInterpolateComponentSetInterpolator(CCComponent Component, CCAnimationInterpolator Interpolator)
{
    ((CCAnimationInterpolateComponentPrivate)Component)->interpolator = Interpolator;
}

static inline void *CCAnimationInterpolateComponentGetCurrentInterpolatedData(CCComponent Component)
{
    CCOrderedCollection Data = CCAnimationKeyframeComponentGetData(Component);
    if (Data)
    {
        size_t Index = CCAnimationKeyframeComponentGetIndex(Component);
        void *Current = CCOrderedCollectionGetElementAtIndex(Data, Index);
        
        const CCAnimationKeyframeDirection Direction = CCAnimationKeyframeComponentGetDirection(Component);
        if (((Index + Direction) >= CCCollectionGetCount(Data)) || ((intmax_t)(Index + Direction) < 0))
        {
            switch (CCAnimationComponentGetLoop(Component))
            {
                case CCAnimationLoopOnce:
                    break;
                    
                case CCAnimationLoopRepeatRoll:
                    Index = 0;
                    break;
                    
                case CCAnimationLoopRepeatFlip:
                    Index += Direction * CCAnimationKeyframeDirectionReverse;
                    break;
            }
        }
        
        else Index += Direction;
        
        void *Next = CCOrderedCollectionGetElementAtIndex(Data, Index);
        if (((CCAnimationInterpolateComponentPrivate)Component)->interpolator)
        {
            //TODO: flag to see if timer changed and so recalculate
            CC_SAFE_Realloc(((CCAnimationInterpolateComponentPrivate)Component)->result, CCCollectionGetElementSize(Data),
                           return NULL;
                           );
            
            ((CCAnimationInterpolateComponentPrivate)Component)->interpolator(Current, Next, CCAnimationComponentGetPosition(Component) / CCAnimationKeyframeComponentGetCurrentFrame(Component), ((CCAnimationInterpolateComponentPrivate)Component)->result);
            
            return ((CCAnimationInterpolateComponentPrivate)Component)->result;
        }
    }
    
    return NULL;
}

#endif
