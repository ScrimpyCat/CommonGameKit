//
//  InputMapMousePositionComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 11/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapMousePositionComponent_h
#define Blob_Game_InputMapMousePositionComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID (CCInputMapTypeMousePosition | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapMousePositionComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
} CCInputMapMousePositionComponentClass, *CCInputMapMousePositionComponentPrivate;


void CCInputMapMousePositionComponentRegister(void);
void CCInputMapMousePositionComponentDeregister(void);


static inline void CCInputMapMousePositionComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
}

static inline void CCInputMapMousePositionComponentDeallocate(CCComponent Component)
{
    CCInputMapComponentDeallocate(Component);
}

#endif
