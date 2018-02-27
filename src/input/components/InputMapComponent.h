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

#ifndef CommonGameKit_InputMapComponent_h
#define CommonGameKit_InputMapComponent_h

#include <CommonGameKit/InputSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/Expression.h>

#define CC_INPUT_MAP_COMPONENT_ID (CCInputMapTypeNone | CC_INPUT_COMPONENT_FLAG)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstrict-prototypes"
/*!
 * @brief Callback to respond to input events.
 * @description This should take the form of one of the valid input map callbacks:
 *
 *              - @b CCInputMapKeyboardCallback
 *
 *              - @b CCInputMapMouseCallback
 */
typedef void (*CCInputMapComponentCallback)();
#pragma clang diagnostic pop

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCString action;
    CCInputMapComponentCallback callback;
} CCInputMapComponentClass, *CCInputMapComponentPrivate;

void CCInputMapComponentDeserializer(CCComponent Component, CCExpression Arg);

/*!
 * @brief Expose a callback to the deserializer.
 * @description This allows for deserialized expressions to reference the callback.
 * @param Name The name of the atom to be used to reference this callback.
 * @param Type The type the callback belongs to. The same name can be used for different
 *        types.
 *
 * @param Callback The input map callback.
 */
void CCInputMapComponentRegisterCallback(CCString CC_COPY(Name), CCInputMapType Type, CCInputMapComponentCallback Callback);

/*!
 * @brief Initialize the input map component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCInputMapComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the input map component.
 * @param Component The component to be deallocated.
 */
static inline void CCInputMapComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the action name of the input map.
 * @description The action name is used to identify the component in calls made to the
 *              input system, such as @b CCInputSystemGetStateForAction.
 *
 * @param Component The input map component.
 * @return The input map action name.
 */
static inline CCString CCInputMapComponentGetAction(CCComponent Component);

/*!
 * @brief Set the action name of the input map.
 * @description The action name is used to identify the component in calls made to the
 *              input system, such as @b CCInputSystemGetStateForAction.
 *
 * @param Component The input map component.
 * @param Action The input map action name. Ownership is transferred to the component.
 */
static inline void CCInputMapComponentSetAction(CCComponent Component, CCString CC_OWN(Action));

/*!
 * @brief Get the callback for the input map.
 * @description The callback is used by the input system as a trigger for input events.
 * @param Component The input map component.
 * @return The input map callback.
 */
static inline CCInputMapComponentCallback CCInputMapComponentGetCallback(CCComponent Component);

/*!
 * @brief Set the callback of the input map.
 * @description The callback is used by the input system as a trigger for input events.
 * @param Component The input map component.
 * @param Callback The input map callback.
 */
static inline void CCInputMapComponentSetCallback(CCComponent Component, CCInputMapComponentCallback Callback);


#pragma mark -

static inline void CCInputMapComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCInputMapComponentPrivate)Component)->action = 0;
    ((CCInputMapComponentPrivate)Component)->callback = NULL;
}

static inline void CCInputMapComponentDeallocate(CCComponent Component)
{
    if (((CCInputMapComponentPrivate)Component)->action) CCStringDestroy(((CCInputMapComponentPrivate)Component)->action);
    CCComponentDeallocate(Component);
}

static inline CCString CCInputMapComponentGetAction(CCComponent Component)
{
    return ((CCInputMapComponentPrivate)Component)->action;
}

static inline void CCInputMapComponentSetAction(CCComponent Component, CCString Action)
{
    if (((CCInputMapComponentPrivate)Component)->action) CCStringDestroy(((CCInputMapComponentPrivate)Component)->action);
    
    ((CCInputMapComponentPrivate)Component)->action = Action ? CCStringCopy(Action) : 0;
}

static inline CCInputMapComponentCallback CCInputMapComponentGetCallback(CCComponent Component)
{
    return ((CCInputMapComponentPrivate)Component)->callback;
}

static inline void CCInputMapComponentSetCallback(CCComponent Component, CCInputMapComponentCallback Callback)
{
    ((CCInputMapComponentPrivate)Component)->callback = Callback;
}

#endif
