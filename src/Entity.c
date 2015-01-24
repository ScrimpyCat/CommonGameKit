//
//  Entity.c
//  Blob Game
//
//  Created by Stefan Johnson on 25/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "Entity.h"
#include "Component.h"
#include "ComponentBase.h"


typedef struct CCEntityInfo {
    CCEntityID id;
    CCComponent *components;
} CCEntityInfo;

CCEntity CCEntityCreate(CCEntityID id, CCAllocatorType Allocator)
{
    CCEntity Entity = CCMalloc(Allocator, sizeof(CCEntityInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);

    if (Entity)
    {
        *Entity = (CCEntityInfo){
            .id = id,
            .components = NULL
        };
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create entity (%" PRIu32 ")", id);
    }
    
    return Entity;
}

void CCEntityDestroy(CCEntity Entity)
{
    CC_SAFE_Free(Entity);
}

void CCEntityAttachComponent(CCEntity Entity, CCComponent Component)
{
    CCAssertLog(CCComponentGetEntity(Component) == NULL, "Component must not be attached to another entity");
    CCComponentSetEntity(Component, Entity);
    
    //TODO: add component to entity
}

void CCEntityDetachComponent(CCEntity Entity, CCComponent Component)
{
    CCAssertLog(CCComponentGetEntity(Component) == Entity, "Component must be attached to this entity");
    CCComponentSetEntity(Component, NULL);
    
    //TODO: remove component from entity
}
