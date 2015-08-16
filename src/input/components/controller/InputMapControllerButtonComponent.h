//
//  InputMapControllerButtonComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 19/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapControllerButtonComponent_h
#define Blob_Game_InputMapControllerButtonComponent_h

#include "InputMapControllerComponent.h"

#define CC_INPUT_MAP_CONTROLLER_BUTTON_COMPONENT_ID (CCInputMapTypeControllerButton | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapControllerButtonComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapControllerComponentClass);
    int32_t button;
    float ramp;
} CCInputMapControllerButtonComponentClass, *CCInputMapControllerButtonComponentPrivate;


void CCInputMapControllerButtonComponentRegister(void);
void CCInputMapControllerButtonComponentDeregister(void);


static inline void CCInputMapControllerButtonComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapControllerComponentInitialize(Component, id);
    ((CCInputMapControllerButtonComponentPrivate)Component)->button = 0;
    ((CCInputMapControllerButtonComponentPrivate)Component)->ramp = 0.0f;
}

static inline void CCInputMapControllerButtonComponentDeallocate(CCComponent Component)
{
    CCInputMapControllerComponentDeallocate(Component);
}

static inline int32_t CCInputMapControllerButtonComponentGetButton(CCComponent Component)
{
    return ((CCInputMapControllerButtonComponentPrivate)Component)->button;
}

static inline void CCInputMapControllerButtonComponentSetButton(CCComponent Component, int32_t Button)
{
    ((CCInputMapControllerButtonComponentPrivate)Component)->button = Button;
}

static inline float CCInputMapControllerButtonComponentGetRamp(CCComponent Component)
{
    return ((CCInputMapControllerButtonComponentPrivate)Component)->ramp;
}

static inline void CCInputMapControllerButtonComponentSetRamp(CCComponent Component, float Ramp)
{
    ((CCInputMapControllerButtonComponentPrivate)Component)->ramp = Ramp;
}

#endif
