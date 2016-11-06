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

#ifndef CommonGameKit_AnimationKeyframeComponent_h
#define CommonGameKit_AnimationKeyframeComponent_h

#include "AnimationComponent.h"
#include <CommonC/Common.h>

#define CC_ANIMATION_KEYFRAME_COMPONENT_ID (CCAnimationTypeKeyframe | CC_ANIMATION_COMPONENT_FLAG)

extern const char * const CCAnimationKeyframeComponentName;

typedef enum {
    CCAnimationKeyframeDirectionForward = 1,
    CCAnimationKeyframeDirectionBackward = -1,
    CCAnimationKeyframeDirectionReverse = CCAnimationKeyframeDirectionBackward
} CCAnimationKeyframeDirection;

typedef struct {
    CC_COMPONENT_INHERIT(CCAnimationComponentClass);
    CCOrderedCollection data;
    union {
        CCOrderedCollection frames; //collection of doubles specifying how long each frame should last
        double frame;
    };
    struct {
        size_t index;
        CCAnimationKeyframeDirection direction;
        _Bool finished;
    } state;
    _Bool multipleFrames;
} CCAnimationKeyframeComponentClass, *CCAnimationKeyframeComponentPrivate;


void CCAnimationKeyframeComponentRegister(void);
void CCAnimationKeyframeComponentDeregister(void);


static inline void CCAnimationKeyframeComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCAnimationComponentInitialize(Component, id);
    ((CCAnimationKeyframeComponentPrivate)Component)->data = NULL;
    ((CCAnimationKeyframeComponentPrivate)Component)->frames = NULL;
    ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames = TRUE;
    ((CCAnimationKeyframeComponentPrivate)Component)->state.index = 0;
    ((CCAnimationKeyframeComponentPrivate)Component)->state.direction = CCAnimationKeyframeDirectionForward;
    ((CCAnimationKeyframeComponentPrivate)Component)->state.finished = FALSE;
}

static inline void CCAnimationKeyframeComponentDeallocate(CCComponent Component)
{
    if (((CCAnimationKeyframeComponentPrivate)Component)->data) CCCollectionDestroy(((CCAnimationKeyframeComponentPrivate)Component)->data);
    if ((((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames) && (((CCAnimationKeyframeComponentPrivate)Component)->frames)) CCCollectionDestroy(((CCAnimationKeyframeComponentPrivate)Component)->frames);
    CCAnimationComponentDeallocate(Component);
}

static inline CCOrderedCollection CCAnimationKeyframeComponentGetData(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->data;
}

static inline void CCAnimationKeyframeComponentSetData(CCComponent Component, CCOrderedCollection Data)
{
    if (((CCAnimationKeyframeComponentPrivate)Component)->data) CCCollectionDestroy(((CCAnimationKeyframeComponentPrivate)Component)->data);
    
    ((CCAnimationKeyframeComponentPrivate)Component)->data = Data;
}

static inline CCOrderedCollection CCAnimationKeyframeComponentGetFrames(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames ? ((CCAnimationKeyframeComponentPrivate)Component)->frames : NULL;
}

static inline void CCAnimationKeyframeComponentSetFrames(CCComponent Component, CCOrderedCollection Frames)
{
    if ((((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames) && (((CCAnimationKeyframeComponentPrivate)Component)->frames)) CCCollectionDestroy(((CCAnimationKeyframeComponentPrivate)Component)->frames);
    
    ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames = TRUE;
    ((CCAnimationKeyframeComponentPrivate)Component)->frames = Frames;
}

static inline double CCAnimationKeyframeComponentGetFrame(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames ? NAN : ((CCAnimationKeyframeComponentPrivate)Component)->frame;
}

static inline void CCAnimationKeyframeComponentSetFrame(CCComponent Component, double Frame)
{
    CCAnimationKeyframeComponentSetFrames(Component, NULL);
    
    ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames = FALSE;
    ((CCAnimationKeyframeComponentPrivate)Component)->frame = Frame;
}

static inline size_t CCAnimationKeyframeComponentGetIndex(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->state.index;
}

static inline void CCAnimationKeyframeComponentSetIndex(CCComponent Component, size_t Index)
{
    ((CCAnimationKeyframeComponentPrivate)Component)->state.index = Index;
}

static inline CCAnimationKeyframeDirection CCAnimationKeyframeComponentGetDirection(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->state.direction;
}

static inline void CCAnimationKeyframeComponentSetDirection(CCComponent Component, CCAnimationKeyframeDirection Direction)
{
    ((CCAnimationKeyframeComponentPrivate)Component)->state.direction = Direction;
}

static inline _Bool CCAnimationKeyframeComponentGetHasMultipleFrames(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames;
}

static inline _Bool CCAnimationKeyframeComponentGetIsFinished(CCComponent Component)
{
    return ((CCAnimationKeyframeComponentPrivate)Component)->state.finished;
}

static inline void CCAnimationKeyframeComponentSetIsFinished(CCComponent Component, _Bool Finished)
{
    ((CCAnimationKeyframeComponentPrivate)Component)->state.finished = Finished;
}

#pragma mark - Convenience functions

static inline double CCAnimationKeyframeComponentGetCurrentFrame(CCComponent Component)
{
    if (((CCAnimationKeyframeComponentPrivate)Component)->multipleFrames)
    {
        if (((CCAnimationKeyframeComponentPrivate)Component)->frames)
        {
            return *(double*)CCOrderedCollectionGetElementAtIndex(((CCAnimationKeyframeComponentPrivate)Component)->frames, ((CCAnimationKeyframeComponentPrivate)Component)->state.index);
        }
        
        return NAN;
    }
    
    else return ((CCAnimationKeyframeComponentPrivate)Component)->frame;
}

static inline void *CCAnimationKeyframeComponentGetCurrentData(CCComponent Component)
{
    if (((CCAnimationKeyframeComponentPrivate)Component)->data)
    {
        return CCOrderedCollectionGetElementAtIndex(((CCAnimationKeyframeComponentPrivate)Component)->data, ((CCAnimationKeyframeComponentPrivate)Component)->state.index);
    }
    
    return NULL;
}

static inline void CCAnimationKeyframeComponentReset(CCComponent Component)
{
    CCAnimationKeyframeComponentSetIndex(Component, 0);
    CCAnimationKeyframeComponentSetIsFinished(Component, FALSE);
    CCAnimationKeyframeComponentSetDirection(Component, CCAnimationKeyframeDirectionForward);
    CCAnimationComponentSetPosition(Component, 0.0);
}

#endif
