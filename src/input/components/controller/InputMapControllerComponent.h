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

#ifndef CommonGameKit_InputMapControllerComponent_h
#define CommonGameKit_InputMapControllerComponent_h

#include <CommonGameKit/InputMapComponent.h>

#define CC_INPUT_MAP_CONTROLLER_COMPONENT_ID (CCInputMapTypeController | CC_INPUT_COMPONENT_FLAG)

extern const CCString CCInputMapControllerComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
    CCString device;
    int8_t connection;
} CCInputMapControllerComponentClass, *CCInputMapControllerComponentPrivate;


void CCInputMapControllerComponentRegister(void);
void CCInputMapControllerComponentDeregister(void);
void CCInputMapControllerComponentDeserializer(CCComponent Component, CCExpression Arg);

/*!
 * @brief Initialize the input map controller component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCInputMapControllerComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the input map controller component.
 * @param Component The component to be deallocated.
 */
static inline void CCInputMapControllerComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the input map controller device name.
 * @description The device name is the assigned name of the controller this component
 *              should match with.
 *
 * @param Component The input map controller component.
 * @return The input map controller device name.
 */
static inline CCString CCInputMapControllerComponentGetDevice(CCComponent Component);

/*!
 * @brief Set the input map controller device name.
 * @description The device name is the assigned name of the controller this component
 *              should match with.
 *
 * @param Component The input map controller component.
 * @param Device The specific device name of the controller this component should
 *        link with.
 */
static inline void CCInputMapControllerComponentSetDevice(CCComponent Component, CCString Device);

/*!
 * @brief Get the input map controller connection.
 * @description The connection is the fixed index of the controller this component
 *              links with.
 *
 * @param Component The input map controller component.
 * @return The input map controller connection.
 */
static inline int8_t CCInputMapControllerComponentGetConnection(CCComponent Component);

/*!
 * @brief Set the input map controller connection.
 * @description The connection is the fixed index of the controller this component
 *              links with.
 *
 * @param Component The input map controller component.
 * @param Connection The index of a controller this component should link with.
 */
static inline void CCInputMapControllerComponentSetConnection(CCComponent Component, int8_t Connection);

#pragma mark -


static inline void CCInputMapControllerComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
    ((CCInputMapControllerComponentPrivate)Component)->device = 0;
    ((CCInputMapControllerComponentPrivate)Component)->connection = 0;
}

static inline void CCInputMapControllerComponentDeallocate(CCComponent Component)
{
    if (((CCInputMapControllerComponentPrivate)Component)->device) CCStringDestroy(((CCInputMapControllerComponentPrivate)Component)->device);
    CCInputMapComponentDeallocate(Component);
}

static inline CCString CCInputMapControllerComponentGetDevice(CCComponent Component)
{
    return ((CCInputMapControllerComponentPrivate)Component)->device;
}

static inline void CCInputMapControllerComponentSetDevice(CCComponent Component, CCString Device)
{
    if (((CCInputMapControllerComponentPrivate)Component)->device) CCStringDestroy(((CCInputMapControllerComponentPrivate)Component)->device);
    
    ((CCInputMapControllerComponentPrivate)Component)->device = Device ? CCStringCopy(Device) : 0;
}

static inline int8_t CCInputMapControllerComponentGetConnection(CCComponent Component)
{
    return ((CCInputMapControllerComponentPrivate)Component)->connection;
}

static inline void CCInputMapControllerComponentSetConnection(CCComponent Component, int8_t Connection)
{
    ((CCInputMapControllerComponentPrivate)Component)->connection = Connection;
}

#endif
