//
//  AnimationComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 14/08/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_AnimationComponent_h
#define Blob_Game_AnimationComponent_h

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
