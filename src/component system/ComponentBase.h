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
    CCAssertLog(!(id & CC_COMPONENT_ID_RESERVED_MASK), "Component ID must not include any reserved bits");
    *(CCComponentPrivate)Component = (CCComponentClass){ .id = id, .entity = NULL };
}

static inline CCComponentID CCComponentGetID(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & ~CC_COMPONENT_ID_RESERVED_MASK;
}

static inline void *CCComponentGetEntity(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->entity;
}

static inline void CCComponentSetEntity(CCComponent Component, CCEntity Entity)
{
    ((CCComponentPrivate)Component)->entity = Entity;
}

static inline void CCComponentSetIsManaged(CCComponent Component, _Bool Managed)
{
    ((CCComponentPrivate)Component)->id = (CC_COMPONENT_ID_RESERVED_MASK * Managed) | CCComponentGetID(Component);
}

static inline _Bool CCComponentGetIsManaged(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & CC_COMPONENT_ID_RESERVED_MASK;
}

#endif
