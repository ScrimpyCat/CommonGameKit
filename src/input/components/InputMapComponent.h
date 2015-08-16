//
//  InputMapComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapComponent_h
#define Blob_Game_InputMapComponent_h

#include "InputSystem.h"
#include "ComponentBase.h"

#define CC_INPUT_MAP_COMPONENT_ID (CCInputMapTypeNone | CC_INPUT_COMPONENT_FLAG)

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    char *action; //TODO: Should probably make a string type
    void (*callback)();
} CCInputMapComponentClass, *CCInputMapComponentPrivate;


static inline void CCInputMapComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCInputMapComponentPrivate)Component)->action = NULL;
    ((CCInputMapComponentPrivate)Component)->callback = NULL;
}

static inline void CCInputMapComponentDeallocate(CCComponent Component)
{
    CC_SAFE_Free(((CCInputMapComponentPrivate)Component)->action);
    CCComponentDeallocate(Component);
}

static inline const char *CCInputMapComponentGetAction(CCComponent Component)
{
    return ((CCInputMapComponentPrivate)Component)->action;
}

static inline void CCInputMapComponentSetAction(CCComponent Component, const char *Action)
{
    if (Action)
    {
        const size_t Length = strlen(Action);
        CC_SAFE_Realloc(((CCInputMapComponentPrivate)Component)->action, (Length + 1) * sizeof(char),
                        CC_LOG_ERROR("Failed to create new action name (%" PRIu32 " : %s) due to allocation failure", CCComponentGetID(Component), Action);
                        return;
                        );
        
        strcpy(((CCInputMapComponentPrivate)Component)->action, Action);
    }
    
    else CC_SAFE_Free(((CCInputMapComponentPrivate)Component)->action);
}

static inline void (*CCInputMapComponentGetCallback(CCComponent Component))()
{
    return ((CCInputMapComponentPrivate)Component)->callback;
}

static inline void CCInputMapComponentSetCallback(CCComponent Component, void (*Callback)())
{
    ((CCInputMapComponentPrivate)Component)->callback = Callback;
}

#endif
