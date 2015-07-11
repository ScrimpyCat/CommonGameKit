//
//  InputMapMouseScrollComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapMouseScrollComponent_h
#define Blob_Game_InputMapMouseScrollComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_MOUSE_SCROLL_COMPONENT_ID (CCInputMapTypeMouseScroll | CC_INPUT_COMPONENT_FLAG)

const char * const CCInputMapMouseScrollComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
} CCInputMapMouseScrollComponentClass, *CCInputMapMouseScrollComponentPrivate;


void CCInputMapMouseScrollComponentRegister(void);
void CCInputMapMouseScrollComponentDeregister(void);


static inline void CCInputMapMouseScrollComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
}

#endif
