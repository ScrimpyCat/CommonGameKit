//
//  InputMapKeyboardComponent.h
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_InputMapKeyboardComponent_h
#define Blob_Game_InputMapKeyboardComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_KEYBOARD_COMPONENT_ID (CCInputMapTypeKeyboard | CC_INPUT_COMPONENT_FLAG)

const char * const CCInputMapKeyboardComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    int32_t keycode;
    uint32_t character;
    uint32_t flags;
    _Bool isKeycode, ignoreModifier;
} CCInputMapKeyboardComponentClass, *CCInputMapKeyboardComponentPrivate;


void CCInputMapKeyboardComponentRegister(void);
void CCInputMapKeyboardComponentDeregister(void);


static inline void CCInputMapKeyboardComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapKeyboardComponentPrivate)Component)->keycode = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->character = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->flags = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->isKeycode = FALSE;
    ((CCInputMapKeyboardComponentPrivate)Component)->ignoreModifier = FALSE;
}

static inline int32_t CCInputMapKeyboardComponentGetKeycode(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->keycode;
}

static inline void CCInputMapKeyboardComponentSetKeycode(CCComponent Component, int32_t Keycode)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->keycode = Keycode;
}

static inline uint32_t CCInputMapKeyboardComponentGetCharacter(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->character;
}

static inline void CCInputMapKeyboardComponentSetCharacter(CCComponent Component, uint32_t Character)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->character = Character;
}

static inline uint32_t CCInputMapKeyboardComponentGetFlags(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->flags;
}

static inline void CCInputMapKeyboardComponentSetFlags(CCComponent Component, uint32_t Flags)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->flags = Flags;
}

static inline _Bool CCInputMapKeyboardComponentGetIsKeycode(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->isKeycode;
}

static inline void CCInputMapKeyboardComponentSetIsKeycode(CCComponent Component, _Bool IsKeycode)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->isKeycode = IsKeycode;
}

static inline _Bool CCInputMapKeyboardComponentGetIgnoreModifier(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->ignoreModifier;
}

static inline void CCInputMapKeyboardComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->ignoreModifier = IgnoreModifier;
}

#endif
