//
//  AnimationInterpolateComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 17/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_AnimationInterpolateComponent_h
#define Blob_Game_AnimationInterpolateComponent_h

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
