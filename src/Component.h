//
//  Component.h
//  Blob Game
//
//  Created by Stefan Johnson on 7/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Component_h
#define Blob_Game_Component_h

#include <stdint.h>
#include <CommonC/Common.h>

#define CC_COMPONENT_ID 0
#define CC_COMPONENT_INHERIT(component) component __inherit

typedef uint32_t CCComponentID;

typedef struct {
    CCComponentID id;
    void *entity;
} CCComponentClass, *CCComponentPrivate;

typedef void *CCComponent;

typedef void (*CCComponentInitializer)(CCComponent Component, CCComponentID id);


void CCComponentRegister(CCComponentID id, const char *Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer);
CCComponent CCComponentCreate(CCComponentID id);
CCComponent CCComponentCreateForName(const char *Name);
void CCComponentDestroy(CCComponent Component);


static inline void CCComponentInitialize(CCComponent Component, CCComponentID id)
{
    *(CCComponentPrivate)Component = (CCComponentClass){ .id = id };
}

static inline CCComponentID CCComponentGetID(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id;
}

static inline void *CCComponentGetEntity(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->entity;
}

#endif
