//
//  InputMapMouseDropComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapMouseDropComponent_h
#define Blob_Game_InputMapMouseDropComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_MOUSE_DROP_COMPONENT_ID (CCInputMapTypeMouseDrop | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapMouseDropComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
} CCInputMapMouseDropComponentClass, *CCInputMapMouseDropComponentPrivate;


void CCInputMapMouseDropComponentRegister(void);
void CCInputMapMouseDropComponentDeregister(void);


static inline void CCInputMapMouseDropComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
}

#endif
