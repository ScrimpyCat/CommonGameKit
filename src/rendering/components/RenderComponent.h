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

#ifndef CommonGameKit_RenderComponent_h
#define CommonGameKit_RenderComponent_h

#include <CommonGameKit/RenderSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonC/Common.h>

//Note: Currently just a demo component


#define CC_RENDER_COMPONENT_ID (2 | CC_RENDER_COMPONENT_FLAG)

extern const CCString CCRenderComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCRect rect;
    CCColourRGB colour;
} CCRenderComponentClass, *CCRenderComponentPrivate;


void CCRenderComponentRegister(void);
void CCRenderComponentDeregister(void);


static inline void CCRenderComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCRenderComponentPrivate)Component)->rect = (CCRect){ .position = CCVector2DZero, .size = CCVector2DZero };
    ((CCRenderComponentPrivate)Component)->colour = CCVector3DZero;
}

static inline void CCRenderComponentDeallocate(CCComponent Component)
{
    CCComponentDeallocate(Component);
}

static inline CCRect CCRenderComponentGetRect(CCComponent Component)
{
    return ((CCRenderComponentPrivate)Component)->rect;
}

static inline void CCRenderComponentSetRect(CCComponent Component, CCRect Rect)
{
    ((CCRenderComponentPrivate)Component)->rect = Rect;
}

static inline CCColourRGB CCRenderComponentGetColour(CCComponent Component)
{
    return ((CCRenderComponentPrivate)Component)->colour;
}

static inline void CCRenderComponentSetColour(CCComponent Component, CCColourRGB Colour)
{
    ((CCRenderComponentPrivate)Component)->colour = Colour;
}

#endif
