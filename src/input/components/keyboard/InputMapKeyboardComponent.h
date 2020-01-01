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

#include <CommonGameKit/InputMapComponent.h>

#define CC_INPUT_MAP_KEYBOARD_COMPONENT_ID (CCInputMapTypeKeyboard | CC_INPUT_COMPONENT_FLAG)

extern const CCString CCInputMapKeyboardComponentName;
extern const CCKeyboardKeycode CCInputMapKeyboardComponentKeycodeAny;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    CCKeyboardKeycode keycode;
    CCChar character;
    CCKeyboardModifier flags;
    float ramp;
    _Bool isKeycode, ignoreModifier, repeats;
} CCInputMapKeyboardComponentClass, *CCInputMapKeyboardComponentPrivate;


extern const CCDictionary(CCString, CCKeyboardKeycode) * const CCInputMapKeyboardComponentKeycodeAtoms;
extern const CCDictionary(CCString, CCKeyboardModifier) * const CCInputMapKeyboardComponentModifierAtoms;

void CCInputMapKeyboardComponentRegister(void);
void CCInputMapKeyboardComponentDeregister(void);
void CCInputMapKeyboardComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Initialize the input map keyboard component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCInputMapKeyboardComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the input map keyboard component.
 * @param Component The component to be deallocated.
 */
static inline void CCInputMapKeyboardComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the input map keyboard keycode to match.
 * @param Component The input map keyboard component.
 * @return The input map keyboard keycode.
 */
static inline CCKeyboardKeycode CCInputMapKeyboardComponentGetKeycode(CCComponent Component);

/*!
 * @brief Set the input map keyboard keycode to match.
 * @param Component The input map keyboard component.
 * @param Keycode The input map keyboard keycode.
 */
static inline void CCInputMapKeyboardComponentSetKeycode(CCComponent Component, CCKeyboardKeycode Keycode);

/*!
 * @brief Get the input map keyboard character to match.
 * @param Component The input map keyboard component.
 * @return The input map keyboard character.
 */
static inline CCChar CCInputMapKeyboardComponentGetCharacter(CCComponent Component);

/*!
 * @brief Set the input map keyboard character to match.
 * @param Component The input map keyboard component.
 * @param Character The input map keyboard character.
 */
static inline void CCInputMapKeyboardComponentSetCharacter(CCComponent Component, CCChar Character);

/*!
 * @brief Get the input map keyboard modifiers to match.
 * @param Component The input map keyboard component.
 * @return The input map keyboard modifiers.
 */
static inline CCKeyboardModifier CCInputMapKeyboardComponentGetFlags(CCComponent Component);

/*!
 * @brief Set the input map keyboard modifiers to match.
 * @param Component The input map keyboard component.
 * @param Flags The input map keyboard modifiers.
 */
static inline void CCInputMapKeyboardComponentSetFlags(CCComponent Component, CCKeyboardModifier Flags);

/*!
 * @brief Get the input map keyboard input ramping.
 * @description The ramping affects the speed of the build up/falloff per second.
 * @param Component The input map keyboard component.
 * @return The input map keyboard ramping value.
 */
static inline float CCInputMapKeyboardComponentGetRamp(CCComponent Component);

/*!
 * @brief Set the input map keyboard input ramping.
 * @description The ramping affects the speed of the build up/falloff per second.
 * @param Component The input map keyboard component.
 * @param Ramp The input map keyboard ramping value.
 */
static inline void CCInputMapKeyboardComponentSetRamp(CCComponent Component, float Ramp);

/*!
 * @brief Get whether the input map keyboard uses a keycode or character match.
 * @param Component The input map keyboard component.
 * @return Whether the input map keyboard uses a keycode.
 */
static inline _Bool CCInputMapKeyboardComponentGetIsKeycode(CCComponent Component);

/*!
 * @brief Set whether the input map keyboard uses a keycode or character match.
 * @param Component The input map keyboard component.
 * @param IsKeycode Whether the input map keyboard uses a keycode.
 */
static inline void CCInputMapKeyboardComponentSetIsKeycode(CCComponent Component, _Bool IsKeycode);

/*!
 * @brief Get whether the input map keyboard ignores modifier flags when matching.
 * @param Component The input map keyboard component.
 * @return Whether the input map keyboard ignores modifiers.
 */
static inline _Bool CCInputMapKeyboardComponentGetIgnoreModifier(CCComponent Component);

/*!
 * @brief Set whether the input map keyboard ignores modifier flags when matching.
 * @param Component The input map keyboard component.
 * @param IgnoreModifier Whether the input map keyboard ignores modifiers.
 */
static inline void CCInputMapKeyboardComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier);

/*!
 * @brief Get whether the input map keyboard matches repeats.
 * @description Repeated key presses are when a key is being held down.
 * @param Component The input map keyboard component.
 * @return Whether the input map keyboard matches repeats.
 */
static inline _Bool CCInputMapKeyboardComponentGetRepeats(CCComponent Component);

/*!
 * @brief Set whether the input map keyboard matches repeats.
 * @description Repeated key presses are when a key is being held down.
 * @param Component The input map keyboard component.
 * @param Repeats Whether the input map keyboard matches repeats.
 */
static inline void CCInputMapKeyboardComponentSetRepeats(CCComponent Component, _Bool Repeats);

#pragma mark -


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
