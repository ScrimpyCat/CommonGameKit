//
//  InputMapControllerAxesComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 12/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapControllerAxesComponent_h
#define Blob_Game_InputMapControllerAxesComponent_h

#include "InputMapControllerComponent.h"

#define CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID (CCInputMapTypeControllerAxes | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapControllerAxesComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapControllerComponentClass);
    float deadzone;
    uint8_t resolution;
    int32_t x, y, z;
    _Bool oneSidedX, oneSidedY, oneSidedZ;
    _Bool flipX, flipY, flipZ;
} CCInputMapControllerAxesComponentClass, *CCInputMapControllerAxesComponentPrivate;


void CCInputMapControllerAxesComponentRegister(void);
void CCInputMapControllerAxesComponentDeregister(void);


static inline void CCInputMapControllerAxesComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapControllerComponentInitialize(Component, id);
    ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone = 0.0f;
    ((CCInputMapControllerAxesComponentPrivate)Component)->resolution = 0;
    ((CCInputMapControllerAxesComponentPrivate)Component)->x = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->y = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->z = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipX = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipY = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ = FALSE;
}

static inline float CCInputMapControllerAxesComponentGetDeadzone(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone;
}

static inline void CCInputMapControllerAxesComponentSetDeadzone(CCComponent Component, float Deadzone)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone = Deadzone;
}

static inline uint8_t CCInputMapControllerAxesComponentGetResolution(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->resolution;
}

static inline void CCInputMapControllerAxesComponentSetResolution(CCComponent Component, uint8_t Resolution)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->resolution = Resolution;
}

static inline int32_t CCInputMapControllerAxesComponentGetX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->x;
}

static inline void CCInputMapControllerAxesComponentSetX(CCComponent Component, int32_t x)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->x = x;
}

static inline int32_t CCInputMapControllerAxesComponentGetY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->y;
}

static inline void CCInputMapControllerAxesComponentSetY(CCComponent Component, int32_t y)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->y = y;
}

static inline int32_t CCInputMapControllerAxesComponentGetZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->z;
}

static inline void CCInputMapControllerAxesComponentSetZ(CCComponent Component, int32_t z)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->z = z;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedX(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedY(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedZ(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipX;
}

static inline void CCInputMapControllerAxesComponentSetFlipX(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipX = Flip;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipY;
}

static inline void CCInputMapControllerAxesComponentSetFlipY(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipY = Flip;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ;
}

static inline void CCInputMapControllerAxesComponentSetFlipZ(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ = Flip;
}

#endif
