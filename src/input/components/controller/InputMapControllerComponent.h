//
//  InputMapControllerComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 13/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapControllerComponent_h
#define Blob_Game_InputMapControllerComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_CONTROLLER_COMPONENT_ID (CCInputMapTypeController | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapControllerComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    char *device; //TODO: Should probably make a string type
    int8_t connection;
} CCInputMapControllerComponentClass, *CCInputMapControllerComponentPrivate;


void CCInputMapControllerComponentRegister(void);
void CCInputMapControllerComponentDeregister(void);


static inline void CCInputMapControllerComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapControllerComponentPrivate)Component)->device = NULL;
    ((CCInputMapControllerComponentPrivate)Component)->connection = 0;
}

static inline void CCInputMapControllerComponentDeallocate(CCComponent Component)
{
    CC_SAFE_Free(((CCInputMapControllerComponentPrivate)Component)->device);
    CCInputMapComponentDeallocate(Component);
}

static inline const char *CCInputMapControllerComponentGetDevice(CCComponent Component)
{
    return ((CCInputMapControllerComponentPrivate)Component)->device;
}

static inline void CCInputMapControllerComponentSetDevice(CCComponent Component, const char *Device)
{
    if (Device)
    {
        const size_t Length = strlen(Device);
        CC_SAFE_Realloc(((CCInputMapControllerComponentPrivate)Component)->device, (Length + 1) * sizeof(char),
                        CC_LOG_ERROR("Failed to create new device name (%" PRIu32 " : %s) due to allocation failure", CCComponentGetID(Component), Device);
                        return;
                        );
        
        strcpy(((CCInputMapControllerComponentPrivate)Component)->device, Device);
    }
    
    else CC_SAFE_Free(((CCInputMapControllerComponentPrivate)Component)->device);
}

static inline int8_t CCInputMapControllerComponentGetConnection(CCComponent Component)
{
    return ((CCInputMapControllerComponentPrivate)Component)->connection;
}

static inline void CCInputMapControllerComponentSetConnection(CCComponent Component, int8_t Connection)
{
    ((CCInputMapControllerComponentPrivate)Component)->connection = Connection;
}

#endif
