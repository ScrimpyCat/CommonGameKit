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

#ifndef CommonGameKit_InputMapComponent_h
#define CommonGameKit_InputMapComponent_h

#include <CommonGameKit/InputSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/Expression.h>

#define CC_INPUT_MAP_COMPONENT_ID (CCInputMapTypeNone | CC_INPUT_COMPONENT_FLAG)

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCString action;
    void (*callback)();
} CCInputMapComponentClass, *CCInputMapComponentPrivate;

void CCInputMapComponentDeserializer(CCComponent Component, CCExpression Arg);

static inline void CCInputMapComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCInputMapComponentPrivate)Component)->action = 0;
    ((CCInputMapComponentPrivate)Component)->callback = NULL;
}

static inline void CCInputMapComponentDeallocate(CCComponent Component)
{
    if (((CCInputMapComponentPrivate)Component)->action) CCStringDestroy(((CCInputMapComponentPrivate)Component)->action);
    CCComponentDeallocate(Component);
}

static inline CCString CCInputMapComponentGetAction(CCComponent Component)
{
    return ((CCInputMapComponentPrivate)Component)->action;
}

static inline void CCInputMapComponentSetAction(CCComponent Component, CCString Action)
{
    if (((CCInputMapComponentPrivate)Component)->action) CCStringDestroy(((CCInputMapComponentPrivate)Component)->action);
    
    ((CCInputMapComponentPrivate)Component)->action = Action ? CCStringCopy(Action) : 0;
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
