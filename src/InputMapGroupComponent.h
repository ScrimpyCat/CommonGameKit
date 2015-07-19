//
//  InputMapGroupComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 10/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapGroupComponent_h
#define Blob_Game_InputMapGroupComponent_h

#include "InputMapComponent.h"
#include <CommonC/Common.h>

#define CC_INPUT_MAP_GROUP_COMPONENT_ID (CCInputMapTypeGroup | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapGroupComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    CCOrderedCollection inputMaps;
    _Bool wantsAllActive;
} CCInputMapGroupComponentClass, *CCInputMapGroupComponentPrivate;


void CCInputMapGroupComponentRegister(void);
void CCInputMapGroupComponentDeregister(void);
void CCInputMapGroupComponentElementDestructor(CCCollection Collection, CCComponent *Element);


static inline void CCInputMapGroupComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapGroupComponentPrivate)Component)->inputMaps = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating | CCCollectionHintSizeSmall, sizeof(CCComponent), (CCCollectionElementDestructor)CCInputMapGroupComponentElementDestructor);
    ((CCInputMapGroupComponentPrivate)Component)->wantsAllActive = FALSE;
}

static inline void CCInputMapGroupComponentDestroy(CCComponent Component)
{
    CCCollectionDestroy(((CCInputMapGroupComponentPrivate)Component)->inputMaps);
    CCInputMapComponentDestroy(Component);
}

static inline CCOrderedCollection CCInputMapGroupComponentGetInputMaps(CCComponent Component)
{
    return ((CCInputMapGroupComponentPrivate)Component)->inputMaps;
}

static inline void CCInputMapGroupComponentAddInputMap(CCComponent Component, CCComponent InputMap)
{
    CCOrderedCollectionAppendElement(((CCInputMapGroupComponentPrivate)Component)->inputMaps, &InputMap);
}

static inline void CCInputMapGroupComponentRemoveInputMap(CCComponent Component, CCComponent InputMap)
{
    CCCollectionRemoveElement(((CCInputMapGroupComponentPrivate)Component)->inputMaps, CCCollectionFindElement(((CCInputMapGroupComponentPrivate)Component)->inputMaps, &InputMap, NULL));
}

static inline _Bool CCInputMapGroupComponentGetWantsAllActive(CCComponent Component)
{
    return ((CCInputMapGroupComponentPrivate)Component)->wantsAllActive;
}

static inline void CCInputMapGroupComponentSetWantsAllActive(CCComponent Component, _Bool WantsAllActive)
{
    ((CCInputMapGroupComponentPrivate)Component)->wantsAllActive = WantsAllActive;
}

#endif
