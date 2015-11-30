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

#ifndef Blob_Game_InputMapMouseButtonComponent_h
#define Blob_Game_InputMapMouseButtonComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID (CCInputMapTypeMouseButton | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapMouseButtonComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    int32_t button;
    uint32_t flags;
    float ramp;
    _Bool ignoreModifier;
} CCInputMapMouseButtonComponentClass, *CCInputMapMouseButtonComponentPrivate;


void CCInputMapMouseButtonComponentRegister(void);
void CCInputMapMouseButtonComponentDeregister(void);


static inline void CCInputMapMouseButtonComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = 0.0f;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = FALSE;
}

static inline void CCInputMapMouseButtonComponentDeallocate(CCComponent Component)
{
    CCInputMapComponentDeallocate(Component);
}

static inline int32_t CCInputMapMouseButtonComponentGetButton(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->button;
}

static inline void CCInputMapMouseButtonComponentSetButton(CCComponent Component, int32_t Button)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = Button;
}

static inline uint32_t CCInputMapMouseButtonComponentGetFlags(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->flags;
}

static inline void CCInputMapMouseButtonComponentSetFlags(CCComponent Component, uint32_t Flags)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = Flags;
}

static inline float CCInputMapMouseButtonComponentGetRamp(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ramp;
}

static inline void CCInputMapMouseButtonComponentSetRamp(CCComponent Component, float Ramp)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = Ramp;
}

static inline _Bool CCInputMapMouseButtonComponentGetIgnoreModifier(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier;
}

static inline void CCInputMapMouseButtonComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = IgnoreModifier;
}

#endif
