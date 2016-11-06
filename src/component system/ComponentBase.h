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

#ifndef CommonGameKit_ComponentBase_h
#define CommonGameKit_ComponentBase_h

#include <CommonGameKit/Component.h>
#include <CommonGameKit/Entity.h>

#define CC_COMPONENT_ID 0

typedef struct {
    CCComponentID id;
    CCEntity entity;
} CCComponentClass, *CCComponentPrivate;


#pragma mark - Base Component Functions
static inline void CCComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCAssertLog(!(id & CC_COMPONENT_ID_RESERVED_MASK), "Component ID must not include any reserved bits");
    *(CCComponentPrivate)Component = (CCComponentClass){ .id = id, .entity = NULL };
}

static inline void CCComponentDeallocate(CCComponent Component)
{
}

static inline CCComponentID CCComponentGetID(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & ~CC_COMPONENT_ID_RESERVED_MASK;
}

static inline void *CCComponentGetEntity(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->entity;
}

static inline void CCComponentSetEntity(CCComponent Component, CCEntity Entity)
{
    ((CCComponentPrivate)Component)->entity = Entity;
}

static inline void CCComponentSetIsManaged(CCComponent Component, _Bool Managed)
{
    ((CCComponentPrivate)Component)->id = (CC_COMPONENT_ID_RESERVED_MASK * Managed) | CCComponentGetID(Component);
}

static inline _Bool CCComponentGetIsManaged(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & CC_COMPONENT_ID_RESERVED_MASK;
}

#endif
