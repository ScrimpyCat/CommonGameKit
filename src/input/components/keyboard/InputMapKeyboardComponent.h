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

#ifndef CommonGameKit_InputMapKeyboardComponent_h
#define CommonGameKit_InputMapKeyboardComponent_h

#include "InputMapComponent.h"

#define CC_INPUT_MAP_KEYBOARD_COMPONENT_ID (CCInputMapTypeKeyboard | CC_INPUT_COMPONENT_FLAG)

extern const char * const CCInputMapKeyboardComponentName;
extern const CCKeyboardKeycode CCInputMapKeyboardComponentKeycodeAny;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    CCKeyboardKeycode keycode;
    CCChar character;
    CCKeyboardModifier flags;
    float ramp;
    _Bool isKeycode, ignoreModifier, repeats;
} CCInputMapKeyboardComponentClass, *CCInputMapKeyboardComponentPrivate;


void CCInputMapKeyboardComponentRegister(void);
void CCInputMapKeyboardComponentDeregister(void);


static inline void CCInputMapKeyboardComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapKeyboardComponentPrivate)Component)->keycode = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->character = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->flags = 0;
    ((CCInputMapKeyboardComponentPrivate)Component)->ramp = 0.0f;
    ((CCInputMapKeyboardComponentPrivate)Component)->isKeycode = FALSE;
    ((CCInputMapKeyboardComponentPrivate)Component)->ignoreModifier = FALSE;
    ((CCInputMapKeyboardComponentPrivate)Component)->repeats = FALSE;
}

static inline void CCInputMapKeyboardComponentDeallocate(CCComponent Component)
{
    CCInputMapComponentDeallocate(Component);
}

static inline CCKeyboardKeycode CCInputMapKeyboardComponentGetKeycode(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->keycode;
}

static inline void CCInputMapKeyboardComponentSetKeycode(CCComponent Component, CCKeyboardKeycode Keycode)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->keycode = Keycode;
}

static inline CCChar CCInputMapKeyboardComponentGetCharacter(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->character;
}

static inline void CCInputMapKeyboardComponentSetCharacter(CCComponent Component, CCChar Character)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->character = Character;
}

static inline CCKeyboardModifier CCInputMapKeyboardComponentGetFlags(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->flags;
}

static inline void CCInputMapKeyboardComponentSetFlags(CCComponent Component, CCKeyboardModifier Flags)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->flags = Flags;
}

static inline float CCInputMapKeyboardComponentGetRamp(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->ramp;
}

static inline void CCInputMapKeyboardComponentSetRamp(CCComponent Component, float Ramp)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->ramp = Ramp;
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

static inline _Bool CCInputMapKeyboardComponentGetRepeats(CCComponent Component)
{
    return ((CCInputMapKeyboardComponentPrivate)Component)->repeats;
}

static inline void CCInputMapKeyboardComponentSetRepeats(CCComponent Component, _Bool Repeats)
{
    ((CCInputMapKeyboardComponentPrivate)Component)->repeats = Repeats;
}

#endif
