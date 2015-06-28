//
//  RenderComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 28/06/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_RenderComponent_h
#define Blob_Game_RenderComponent_h

#include "RenderSystem.h"
#include "ComponentBase.h"
#include <CommonC/Common.h>

//Note: Currently just a demo component


#define CC_RENDER_COMPONENT_ID (1 | CC_RENDER_COMPONENT_FLAG)

const char * const CCRenderComponentName;

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
