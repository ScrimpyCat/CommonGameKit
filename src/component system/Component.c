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

#include "Component.h"
#include <string.h>
#include <inttypes.h>
#include "ComponentBase.h"


typedef struct {
    CCComponentID id;
    char *name; //TODO: Could be put into its own hashmap
    CCAllocatorType allocator;
    CCComponentInitializer initializer;
    CCComponentDestructor destructor;
    size_t size;
    //TODO: Dependencies
} CCComponentInfo;

static void CCComponentListElementDestructor(CCCollection Collection, CCComponentInfo *Element)
{
    CC_SAFE_Free(Element->name);
}

static CCCollection ComponentList = NULL; //TODO: Probably more optimal if it is a hashmap
void CCComponentRegister(CCComponentID id, const char *Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer, CCComponentDestructor Destructor)
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
        .destructor = Destructor,
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

static CCComponentInfo *CCComponentInfoFindByID(CCComponentID id)
{
    return CCCollectionGetElement(ComponentList, CCCollectionFindElement(ComponentList, &id, (CCComparator)CCComponentIDComparator));
}

static CCComponentInfo *CCComponentInfoFindByName(const char *Name)
{
    return CCCollectionGetElement(ComponentList, CCCollectionFindElement(ComponentList, &Name, (CCComparator)CCComponentNameComparator));
}

CCComponent CCComponentCreate(CCComponentID id)
{
    CCComponentInfo *Info = CCComponentInfoFindByID(id);
    
    if (Info) return CCComponentCreateFromInfo(Info);
    
    CC_LOG_WARNING("Could not find component with id (%" PRIu32 ")", id);
    
    return NULL;
}

CCComponent CCComponentCreateForName(const char *Name)
{
    CCAssertLog(Name, "Name cannot be NULL");
    
    CCComponentInfo *Info = CCComponentInfoFindByName(Name);
    
    if (Info) return CCComponentCreateFromInfo(Info);
    
    CC_LOG_WARNING("Could not find component with name (%s)", Name);
    
    return NULL;
}

void CCComponentDestroy(CCComponent Component)
{
    if (Component)
    {
        CCComponentInfo *Info = CCComponentInfoFindByID(CCComponentGetID(Component));
        
        if (Info)
        {
            if (Info->destructor) Info->destructor(Component);
        }
        
        CC_SAFE_Free(Component);
    }
}
