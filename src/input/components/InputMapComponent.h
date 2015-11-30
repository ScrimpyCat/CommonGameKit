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
