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

#ifndef CommonGameKit_InputMapMouseButtonComponent_h
#define CommonGameKit_InputMapMouseButtonComponent_h

#include <CommonGameKit/InputMapComponent.h>

#define CC_INPUT_MAP_MOUSE_BUTTON_COMPONENT_ID (CCInputMapTypeMouseButton | CC_INPUT_COMPONENT_FLAG)

extern const CCString CCInputMapMouseButtonComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    CCMouseButton button;
    CCKeyboardModifier flags;
    float ramp;
    _Bool ignoreModifier;
} CCInputMapMouseButtonComponentClass, *CCInputMapMouseButtonComponentPrivate;


extern const CCDictionary(CCString, CCMouseButton) * const CCInputMapMouseButtonComponentButtonAtoms;

void CCInputMapMouseButtonComponentRegister(void);
void CCInputMapMouseButtonComponentDeregister(void);
void CCInputMapMouseButtonComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred);

/*!
 * @brief Initialize the input map mouse button component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCInputMapMouseButtonComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the input map mouse button component.
 * @param Component The component to be deallocated.
 */
static inline void CCInputMapMouseButtonComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the input map mouse button to match.
 * @param Component The input map mouse button component.
 * @return The input map mouse button.
 */
static inline CCMouseButton CCInputMapMouseButtonComponentGetButton(CCComponent Component);

/*!
 * @brief Set the input map mouse button to match.
 * @param Component The input map mouse button component.
 * @param Keycode The input map mouse button.
 */
static inline void CCInputMapMouseButtonComponentSetButton(CCComponent Component, CCMouseButton Button);

/*!
 * @brief Get the input map mouse button modifiers to match.
 * @param Component The input map mouse button component.
 * @return The input map mouse button modifiers.
 */
static inline CCKeyboardModifier CCInputMapMouseButtonComponentGetFlags(CCComponent Component);

/*!
 * @brief Set the input map mouse button modifiers to match.
 * @param Component The input map mouse button component.
 * @param Flags The input map mouse button modifiers.
 */
static inline void CCInputMapMouseButtonComponentSetFlags(CCComponent Component, CCKeyboardModifier Flags);

/*!
 * @brief Get the input map mouse button input ramping.
 * @description The ramping affects the speed of the build up/falloff per second.
 * @param Component The input map mouse button component.
 * @return The input map mouse button ramping value.
 */
static inline float CCInputMapMouseButtonComponentGetRamp(CCComponent Component);

/*!
 * @brief Set the input map mouse button input ramping.
 * @description The ramping affects the speed of the build up/falloff per second.
 * @param Component The input map mouse button component.
 * @param Ramp The input map mouse button ramping value.
 */
static inline void CCInputMapMouseButtonComponentSetRamp(CCComponent Component, float Ramp);

/*!
 * @brief Get whether the input map mouse button ignores modifier flags when matching.
 * @param Component The input map mouse button component.
 * @return Whether the input map mouse button ignores modifiers.
 */
static inline _Bool CCInputMapMouseButtonComponentGetIgnoreModifier(CCComponent Component);

/*!
 * @brief Set whether the input map mouse button ignores modifier flags when matching.
 * @param Component The input map mouse button component.
 * @param IgnoreModifier Whether the input map mouse button ignores modifiers.
 */
static inline void CCInputMapMouseButtonComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier);

#pragma mark -


static inline void CCInputMapMouseButtonComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = 0;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = 0.0f;
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = FALSE;
}

static inline void CCInputMapMouseButtonComponentDeallocate(CCComponent Component)
{
    CCInputMapComponentDeallocate(Component);
}

static inline CCMouseButton CCInputMapMouseButtonComponentGetButton(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->button;
}

static inline void CCInputMapMouseButtonComponentSetButton(CCComponent Component, CCMouseButton Button)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->button = Button;
}

static inline CCKeyboardModifier CCInputMapMouseButtonComponentGetFlags(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->flags;
}

static inline void CCInputMapMouseButtonComponentSetFlags(CCComponent Component, CCKeyboardModifier Flags)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->flags = Flags;
}

static inline float CCInputMapMouseButtonComponentGetRamp(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ramp;
}

static inline void CCInputMapMouseButtonComponentSetRamp(CCComponent Component, float Ramp)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ramp = Ramp;
}

static inline _Bool CCInputMapMouseButtonComponentGetIgnoreModifier(CCComponent Component)
{
    return ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier;
}

static inline void CCInputMapMouseButtonComponentSetIgnoreModifier(CCComponent Component, _Bool IgnoreModifier)
{
    ((CCInputMapMouseButtonComponentPrivate)Component)->ignoreModifier = IgnoreModifier;
}

#endif
