/*
 *  Copyright (c) 2017, Stefan Johnson
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

#ifndef CommonGameKit_RelationParentComponent_h
#define CommonGameKit_RelationParentComponent_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/Entity.h>


#define CC_RELATION_PARENT_COMPONENT_ID 2

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCEntity parent;
} CCRelationParentComponentClass, *CCRelationParentComponentPrivate;


void CCRelationParentComponentRegister(void);
void CCRelationParentComponentDeregister(void);


static inline void CCRelationParentComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCRelationParentComponentPrivate)Component)->parent = NULL;
}

static inline void CCRelationParentComponentDeallocate(CCComponent Component)
{
    if (((CCRelationParentComponentPrivate)Component)->parent) CCEntityDestroy(((CCRelationParentComponentPrivate)Component)->parent);
    
    CCComponentDeallocate(Component);
}

static inline CCEntity CCRelationParentComponentGetParent(CCComponent Component)
{
    return ((CCRelationParentComponentPrivate)Component)->parent;
}

static inline void CCRelationParentComponentSetParent(CCComponent Component, CCEntity Parent)
{
    CCAssertLog(!((CCRelationParentComponentPrivate)Component)->parent, "Parent should not be changed once set");
    
    ((CCRelationParentComponentPrivate)Component)->parent = CCRetain(Parent);
}

#endif
