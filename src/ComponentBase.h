//
//  ComponentBase.h
//  Blob Game
//
//  Created by Stefan Johnson on 25/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_ComponentBase_h
#define Blob_Game_ComponentBase_h

#include "Component.h"
#include "Entity.h"

#define CC_COMPONENT_ID 0

typedef struct {
    CCComponentID id;
    CCEntity entity;
} CCComponentClass, *CCComponentPrivate;


#pragma mark - Base Component Functions
static inline void CCComponentInitialize(CCComponent Component, CCComponentID id)
{
    *(CCComponentPrivate)Component = (CCComponentClass){ .id = id };
}

static inline CCComponentID CCComponentGetID(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id;
}

static inline void *CCComponentGetEntity(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->entity;
}

static inline void CCComponentSetEntity(CCComponent Component, CCEntity Entity)
{
    ((CCComponentPrivate)Component)->entity = Entity;
}

#endif
