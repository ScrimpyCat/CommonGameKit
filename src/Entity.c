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
    CCCollection components;
} CCEntityInfo;

static void CCEntityComponentDestructor(CCCollection Collection, CCComponent *Component)
{
    CCAssertLog(!CCComponentGetIsManaged(*Component), "Entity should not be destroying managed components");
    
    CCComponentSetEntity(*Component, NULL);
    CCComponentDestroy(*Component);
}

CCEntity CCEntityCreate(CCEntityID id, CCAllocatorType Allocator)
{
    CCEntity Entity = CCMalloc(Allocator, sizeof(CCEntityInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);

    if (Entity)
    {
        *Entity = (CCEntityInfo){
            .id = id,
            .components = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall, sizeof(CCComponent), (CCCollectionElementDestructor)CCEntityComponentDestructor)
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
    CCCollectionDestroy(Entity->components);
    CC_SAFE_Free(Entity);
}

void CCEntityAttachComponent(CCEntity Entity, CCComponent Component)
{
    CCAssertLog(CCComponentGetEntity(Component) == NULL, "Component must not be attached to another entity");
    
    CCComponentSetEntity(Component, Entity);
    CCCollectionInsertElement(Entity->components, &Component); //TODO: will components be attached/detached from multiple threads?
}

void CCEntityDetachComponent(CCEntity Entity, CCComponent Component)
{
    CCAssertLog(CCComponentGetEntity(Component) == Entity, "Component must be attached to this entity");
    
    CCComponentSetEntity(Component, NULL);
    CCCollectionRemoveElement(Entity->components, CCCollectionFindElement(Entity->components, &Component, NULL)); //TODO: will components be attached/detached from multiple threads?
}

CCCollection CCEntityGetComponents(CCEntity Entity)
{
    return Entity->components; //TODO: will components be attached/detached from multiple threads?
}
