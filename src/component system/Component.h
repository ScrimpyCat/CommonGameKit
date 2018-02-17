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

#ifndef CommonGameKit_Component_h
#define CommonGameKit_Component_h

#include <stdint.h>
#include <CommonC/Common.h>
#include <CommonGameKit/MessageType.h>

#define CC_COMPONENT_INHERIT(component) component __inherit
#define CC_COMPONENT_ID_RESERVED_MASK ~((CCComponentID)-1 >> 1)
#define CC_COMPONENT_SYSTEM_FLAG_MASK 0x7f000000

/*!
 * @brief The unique ID of the component.
 * @description Highest bit is reserved for internal use, see CC_COMPONENT_ID_RESERVED_MASK.
 */
typedef uint32_t CCComponentID;

/*!
 * @brief Generic Component handle.
 * @description Allows @b CCRetain.
 */
typedef void *CCComponent;

/*!
 * @brief The custom component initializer callback.
 * @description Perform all initialization for your component's fields in here, and call the initializer of
 *              the inherited component.
 *
 * @param Component The component to be initialized.
 * @param id The component ID for the component being initialized.
 */
typedef void (*CCComponentInitializer)(CCComponent Component, CCComponentID id);

/*!
 * @brief The custom component destructor callback.
 * @description Perform all cleanup for your component's fields in here, and call the destructor of
 *              the inherited component.
 *
 * @param Component The component to be destroyed.
 */
typedef void (*CCComponentDestructor)(CCComponent Component);

/*!
 * @brief The message handler callback.
 * @description Handle any messages the component should respond to internally. If the message is not
 *              known, it can be ignored.
 *
 * @param Component The component to handle the message.
 * @param Message The message to be handled.
 */
typedef void (*CCComponentMessageHandler)(CCComponent Component, CCMessage *Message);


/*!
 * @brief Register a component type.
 * @param id The unique ID to identify the component.
 * @param Name An optional name for the component, may be NULL.
 * @param Allocator The type of allocator to be used.
 * @param Size The total size of this component.
 * @param Initializer The callback to use for initialization of this component.
 * @param MessageHandler The callback to use for handling messages. May be NULL.
 * @param Destructor The callback to use for cleanup of this component.
 */
void CCComponentRegister(CCComponentID id, CCString CC_COPY(Name), CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer, CCComponentMessageHandler MessageHandler, CCComponentDestructor Destructor);

/*!
 * @brief Deregister a component type.
 * @warning Only provided for unit tests, not safe to call in actual program.
 * @param id The unique ID of the component type to be removed.
 */
void CCComponentDeregister(CCComponentID id);

/*!
 * @brief Create a component of type for ID.
 * @param id The unique ID of the component type.
 * @return The created component.
 */
CC_NEW CCComponent CCComponentCreate(CCComponentID id);

/*!
 * @brief Create a component of type for name.
 * @performance Creating components this way is slower than with the ID.
 * @param Name The unique name of the component type.
 * @return The created component.
 */
CC_NEW CCComponent CCComponentCreateForName(CCString Name);

/*!
 * @brief Destroy a component.
 * @param Component The component to be destroyed.
 */
void CCComponentDestroy(CCComponent CC_DESTROY(Component));

/*!
 * @brief Pass a message to the component.
 * @param Component The component to handle the message.
 * @param Message The message to be handled.
 */
void CCComponentHandleMessage(CCComponent Component, CCMessage *Message);

#endif
