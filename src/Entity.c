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
    CCArray components; //TODO: Change later so doesn't waste memory
} CCEntityInfo;

CCEntity CCEntityCreate(CCEntityID id, CCAllocatorType Allocator)
{
    CCEntity Entity = CCMalloc(Allocator, sizeof(CCEntityInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);

    if (Entity)
    {
        *Entity = (CCEntityInfo){
            .id = id,
            .components = CCArrayCreate(Allocator, sizeof(CCComponent), 1)
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
    CCArrayAppendElement(Entity->components, &Component);
}

void CCEntityDetachComponent(CCEntity Entity, CCComponent Component)
{
    CCAssertLog(CCComponentGetEntity(Component) == Entity, "Component must be attached to this entity");
    CCComponentSetEntity(Component, NULL);
    
    for (size_t Loop = 0, Count = CCArrayGetCount(Entity->components); Loop < Count; Loop++)
    {
        if (*(CCComponent*)CCArrayGetElementAtIndex(Entity->components, Loop) == Component)
        {
            CCArrayReplaceElementAtIndex(Entity->components, Loop, &(CCComponent){ NULL });
            break;
        }
    }
}
