//
//  Component.c
//  Blob Game
//
//  Created by Stefan Johnson on 7/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "Component.h"
#include <string.h>


typedef struct {
    CCComponentID id;
    char *name; //TODO: Could be put into its own hashmap
    CCAllocatorType allocator;
    CCComponentInitializer initializer;
    size_t size;
    //TODO: Dependencies
} CCComponentInfo;

CCComponentInfo *ComponentList = NULL; //TODO: Probably more optimal if it is a hashmap
static size_t ComponentsCount = 0;
void CCComponentRegister(CCComponentID id, const char *Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer)
{
    CC_SAFE_Realloc(ComponentList, sizeof(typeof(*ComponentList)) * ++ComponentsCount,
                    CC_LOG_ERROR("Failed to add new component (%" PRIu32 " : %s) due to realloc failing. Allocation size: %zu", id, Name, sizeof(typeof(*ComponentList)) * ComponentsCount);
                    ComponentsCount--;
                    return;
                    );
    
    char *ComponentName = NULL;
    if (Name)
    {
        CC_SAFE_Malloc(ComponentName, sizeof(char) * (strlen(Name) + 1),
                       CC_LOG_ERROR("Failed to add new component (%" PRIu32 " : %s) due to allocation space for name failing. Allocation size: %zu", id, Name, sizeof(char) * (strlen(Name) + 1));
                       ComponentsCount--;
                       return;
                       );
        
        strcpy(ComponentName, Name);
    }
    
    
    ComponentList[ComponentsCount - 1] = (typeof(*ComponentList)){
        .id = id,
        .name = ComponentName,
        .allocator = Allocator,
        .initializer = Initializer,
        .size = Size
    };
}

void CCComponentDeregister(CCComponentID id)
{
    //Note: Really only needed for unit tests, so doesn't matter if not correct
    for (size_t Loop = 0, Count = ComponentsCount; Loop < Count; Loop++)
    {
        if (ComponentList[Loop].id == id)
        {
            memset(&ComponentList[Loop], 0, sizeof(CCComponentInfo));
        }
    }
}

static CCComponent CCComponentCreateFromInfo(CCComponentInfo *Info)
{
    CCComponent Component = CCMalloc(Info->allocator, Info->size, NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (!Component)
    {
        CC_LOG_ERROR("Failed to create component (%" PRIu32 " : %s) of size (%zu)", Info->id, Info->name, Info->size);
    }
    
    else if (Info->initializer)
    {
        Info->initializer(Component, Info->id);
    }
    
    return Component;
}

CCComponent CCComponentCreate(CCComponentID id)
{
    for (size_t Loop = 0, Count = ComponentsCount; Loop < Count; Loop++)
    {
        if (ComponentList[Loop].id == id) return CCComponentCreateFromInfo(&ComponentList[Loop]);
    }
    
    CC_LOG_WARNING("Could not find component with id (%" PRIu32 ")", id);
    
    return NULL;
}

CCComponent CCComponentCreateForName(const char *Name)
{
    CCAssertLog(Name, "Name cannot be NULL");
    
    for (size_t Loop = 0, Count = ComponentsCount; Loop < Count; Loop++)
    {
        if ((ComponentList[Loop].name) && (!strcmp(Name, ComponentList[Loop].name))) return CCComponentCreateFromInfo(&ComponentList[Loop]);
    }
    
    CC_LOG_WARNING("Could not find component with name (%s)", Name);
    
    return NULL;
}

void CCComponentDestroy(CCComponent Component)
{
    CC_SAFE_Free(Component);
}
