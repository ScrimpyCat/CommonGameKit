//
//  InputMapMouseButtonComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapMouseButtonComponent_h
#define Blob_Game_InputMapMouseButtonComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID (CCInputMapTypeMouseButton | CC_INPUT_COMPONENT_FLAG)

const char * const CCInputMapMouseButtonComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    int32_t button;
    uint32_t flags;
    float ramp;
    _Bool ignoreModifier;
} CCInputMapMouseButtonComponentClass, *CCInputMapMouseButtonComponentPrivate;


void CCInputMapMouseButtonComponentRegister(void);
void CCInputMapMouseButtonComponentDeregister(void);


static inline void CCInputMapMouseButtonComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = 0.0f;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = FALSE;
}

static inline int32_t CCInputMapMouseButtonComponentGetButton(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->button;
}

static inline void CCInputMapMouseButtonComponentSetButton(CCComponent Component, int32_t Button)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = Button;
}

static inline uint32_t CCInputMapMouseButtonComponentGetFlags(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->flags;
}

static inline void CCInputMapMouseButtonComponentSetFlags(CCComponent Component, uint32_t Flags)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = Flags;
}

static inline float CCInputMapMouseButtonComponentGetRamp(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ramp;
}

static inline void CCInputMapMouseButtonComponentSetRamp(CCComponent Component, float Ramp)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = Ramp;
}

static inline _Bool CCInputMapMouseButtonComponentGetIgnoreModifier(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier;
}

static inline void CCInputMapMouseButtonComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = IgnoreModifier;
}

#endif
