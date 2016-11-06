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

#ifndef CommonGameKit_AnimationComponent_h
#define CommonGameKit_AnimationComponent_h

#include "AnimationSystem.h"
#include "ComponentBase.h"
#include <CommonC/Common.h>


#define CC_ANIMATION_COMPONENT_ID (CCAnimationTypeNone | CC_ANIMATION_COMPONENT_FLAG)

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    char *name; //TODO: string type
    CCAnimationLoop loop;
    struct {
        float speed;
        double position;
        _Bool playing;
    } control;
} CCAnimationComponentClass, *CCAnimationComponentPrivate;


void CCAnimationComponentRegister(void);
void CCAnimationComponentDeregister(void);


static inline void CCAnimationComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCAnimationComponentPrivate)Component)->loop = CCAnimationLoopOnce;
    ((CCAnimationComponentPrivate)Component)->name = NULL;
    ((CCAnimationComponentPrivate)Component)->control.speed = 1.0f;
    ((CCAnimationComponentPrivate)Component)->control.position = 0.0f;
    ((CCAnimationComponentPrivate)Component)->control.playing = FALSE;
}

static inline void CCAnimationComponentDeallocate(CCComponent Component)
{
    CC_SAFE_Free(((CCAnimationComponentPrivate)Component)->name);
    CCComponentDeallocate(Component);
}

static inline const char *CCAnimationComponentGetName(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->name;
}

static inline void CCAnimationComponentSetName(CCComponent Component, const char *Name)
{
    if (Name)
    {
        const size_t Length = strlen(Name);
        CC_SAFE_Realloc(((CCAnimationComponentPrivate)Component)->name, (Length + 1) * sizeof(char),
                        CC_LOG_ERROR("Failed to create new action name (%" PRIu32 " : %s) due to allocation failure", CCComponentGetID(Component), Name);
                        return;
                        );
        
        strcpy(((CCAnimationComponentPrivate)Component)->name, Name);
    }
    
    else CC_SAFE_Free(((CCAnimationComponentPrivate)Component)->name);
}

static inline CCAnimationLoop CCAnimationComponentGetLoop(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->loop;
}

static inline void CCAnimationComponentSetLoop(CCComponent Component, CCAnimationLoop Loop)
{
    ((CCAnimationComponentPrivate)Component)->loop = Loop;
}

static inline float CCAnimationComponentGetSpeed(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.speed;
}

static inline void CCAnimationComponentSetSpeed(CCComponent Component, float Speed)
{
    ((CCAnimationComponentPrivate)Component)->control.speed = Speed;
}

static inline double CCAnimationComponentGetPosition(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.position;
}

static inline void CCAnimationComponentSetPosition(CCComponent Component, double Position)
{
    ((CCAnimationComponentPrivate)Component)->control.position = Position;
}

static inline _Bool CCAnimationComponentGetPlaying(CCComponent Component)
{
    return ((CCAnimationComponentPrivate)Component)->control.playing;
}

static inline void CCAnimationComponentSetPlaying(CCComponent Component, _Bool Playing)
{
    ((CCAnimationComponentPrivate)Component)->control.playing = Playing;
}

#endif
