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
#include "Message.h"


typedef struct {
    CCComponentID id;
    CCString name; //TODO: Could be put into its own hashmap
    CCAllocatorType allocator;
    CCComponentInitializer initializer;
    CCComponentDestructor destructor;
    CCComponentMessageHandler messageHandler;
    size_t size;
    //TODO: Dependencies
} CCComponentInfo;

static void CCComponentListElementDestructor(CCCollection Collection, CCComponentInfo *Element)
{
    if (Element->name) CCStringDestroy(Element->name);
}

static CCCollection ComponentList = NULL; //TODO: Probably more optimal if it is a hashmap
void CCComponentRegister(CCComponentID id, CCString Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer, CCComponentMessageHandler MessageHandler, CCComponentDestructor Destructor)
{
    if (!ComponentList) ComponentList = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeMedium | CCCollectionHintHeavyFinding, sizeof(CCComponentInfo), (CCCollectionElementDestructor)CCComponentListElementDestructor);
    
    CCCollectionInsertElement(ComponentList, &(CCComponentInfo){
        .id = id,
        .name = (Name ? CCStringCopy(Name) : 0),
        .allocator = Allocator,
        .initializer = Initializer,
        .destructor = Destructor,
        .messageHandler = MessageHandler,
        .size = Size
    });
}

static CCComparisonResult CCComponentIDComparator(const CCComponentInfo *Component, const CCComponentID *id)
{
    return Component->id == *id ? CCComparisonResultEqual : CCComparisonResultInvalid;
}

static CCComparisonResult CCComponentNameComparator(const CCComponentInfo *Component, CCString *Name)
{
    return (Component->name) && (CCStringEqual(*Name, Component->name)) ? CCComparisonResultEqual : CCComparisonResultInvalid;
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
        CC_LOG_ERROR_CUSTOM("Failed to create component (%" PRIu32 " : %S) of size (%zu)", Info->id, Info->name, Info->size);
    }
    
    else
    {
        if (Info->initializer)
        {
            Info->initializer(Component, Info->id);
        }
        
        if (Info->destructor)
        {
            CCMemorySetDestructor(Component, Info->destructor);
        }
    }
    
    return Component;
}

static CCComponentInfo *CCComponentInfoFindByID(CCComponentID id)
{
    return CCCollectionGetElement(ComponentList, CCCollectionFindElement(ComponentList, &id, (CCComparator)CCComponentIDComparator));
}

static CCComponentInfo *CCComponentInfoFindByName(CCString Name)
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

CCComponent CCComponentCreateForName(CCString Name)
{
    CCAssertLog(Name, "Name cannot be NULL");
    
    CCComponentInfo *Info = CCComponentInfoFindByName(Name);
    
    if (Info) return CCComponentCreateFromInfo(Info);
    
    CC_LOG_WARNING_CUSTOM("Could not find component with name (%S)", Name);
    
    return NULL;
}

void CCComponentDestroy(CCComponent Component)
{
    CCFree(Component);
}

void CCComponentHandleMessage(CCComponent Component, CCMessage *Message)
{
    CCAssertLog(Component, "Component must not be NULL");
    
    CCComponentInfo *Info = CCComponentInfoFindByID(CCComponentGetID(Component));
    
    if (Info)
    {
        if (Info->messageHandler) Info->messageHandler(Component, Message);
    }
}
