/*
 *  Copyright (c) 2015, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CC_QUICK_COMPILE
#include "Entity.h"
#include "Component.h"
#include "ComponentBase.h"
#include <inttypes.h>


typedef struct CCEntityInfo {
    CCString id;
    CCCollection(CCComponent) components;
} CCEntityInfo;

static void CCEntityComponentDestructor(CCCollection(CCComponent) Collection, CCComponent *Component)
{
    CCComponentSetEntity(*Component, NULL);
    CCComponentDestroy(*Component);
}

static void CCEntityDestructor(CCEntity Entity)
{
    CCCollectionDestroy(Entity->components);
}

CCEntity CCEntityCreate(CCString id, CCAllocatorType Allocator)
{
    CCEntity Entity = CCMalloc(Allocator, sizeof(CCEntityInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);

    if (Entity)
    {
        *Entity = (CCEntityInfo){
            .id = CCStringCopy(id),
            .components = CCCollectionCreate(Allocator, CCCollectionHintSizeSmall, sizeof(CCComponent), (CCCollectionElementDestructor)CCEntityComponentDestructor)
        };
        
        CCMemorySetDestructor(Entity, (CCMemoryDestructorCallback)CCEntityDestructor);
    }
    
    else
    {
        CC_LOG_ERROR_CUSTOM("Failed to create entity (%S)", id);
    }
    
    return Entity;
}

void CCEntityDestroy(CCEntity Entity)
{
    CCFree(Entity);
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

CCCollection(CCComponent) CCEntityGetComponents(CCEntity Entity)
{
    return Entity->components; //TODO: will components be attached/detached from multiple threads?
}
