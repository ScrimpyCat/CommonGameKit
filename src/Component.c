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

static void CCComponentListElementDestructor(CCCollection Collection, CCComponentInfo *Element)
{
    CC_SAFE_Free(Element->name);
}

CCCollection ComponentList = NULL; //TODO: Probably more optimal if it is a hashmap
void CCComponentRegister(CCComponentID id, const char *Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer)
{
    if (!ComponentList) ComponentList = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyFinding, sizeof(CCComponentInfo), (CCCollectionElementDestructor)CCComponentListElementDestructor);
    
    char *ComponentName = NULL;
    if (Name)
    {
        CC_SAFE_Malloc(ComponentName, sizeof(char) * (strlen(Name) + 1),
                       CC_LOG_ERROR("Failed to add new component (%" PRIu32 " : %s) due to allocation space for name failing. Allocation size: %zu", id, Name, sizeof(char) * (strlen(Name) + 1));
                       return;
                       );
        
        strcpy(ComponentName, Name);
    }
    
    
    CCCollectionInsertElement(ComponentList, &(CCComponentInfo){
        .id = id,
        .name = ComponentName,
        .allocator = Allocator,
        .initializer = Initializer,
        .size = Size
    });
}

static CCComparisonResult CCComponentIDComparator(const CCComponentInfo *Component, const CCComponentID *id)
{
    return Component->id == *id ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCComparisonResult CCComponentNameComparator(const CCComponentInfo *Component, const char **Name)
{
    return (Component->name) && (!strcmp(*Name, Component->name)) ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

void CCComponentDeregister(CCComponentID id)
{
    CCCollectionRemoveElement(ComponentList, CCCollectionFindElement(ComponentList, &id, (CCComparator)CCComponentIDComparator));
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
    CCComponentInfo *Info = CCCollectionGetElement(ComponentList, CCCollectionFindElement(ComponentList, &id, (CCComparator)CCComponentIDComparator));
    
    if (Info) return CCComponentCreateFromInfo(Info);
    
    CC_LOG_WARNING("Could not find component with id (%" PRIu32 ")", id);
    
    return NULL;
}

CCComponent CCComponentCreateForName(const char *Name)
{
    CCAssertLog(Name, "Name cannot be NULL");
    
    CCComponentInfo *Info = CCCollectionGetElement(ComponentList, CCCollectionFindElement(ComponentList, &Name, (CCComparator)CCComponentNameComparator));
    
    if (Info) return CCComponentCreateFromInfo(Info);
    
    CC_LOG_WARNING("Could not find component with name (%s)", Name);
    
    return NULL;
}

void CCComponentDestroy(CCComponent Component)
{
    CC_SAFE_Free(Component);
}
