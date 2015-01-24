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

#define CC_COMPONENT_INHERIT(component) component __inherit

/*!
 * @brief The unique ID of the component.
 */
typedef uint32_t CCComponentID;

/*!
 * @brief Generic Component handler.
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
 * @brief Register a component type.
 * @param id The unique ID to identify the component.
 * @param Name An optional name for the component, may be NULL.
 * @param Allocator The type of allocator to be used.
 * @param Size The total size of this component.
 * @param Initializer The callback to use for initialization of this component.
 */
void CCComponentRegister(CCComponentID id, const char *Name, CCAllocatorType Allocator, size_t Size, CCComponentInitializer Initializer);

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
CCComponent CCComponentCreate(CCComponentID id);

/*!
 * @brief Create a component of type for name.
 * @performance Creating components this way is slower than with the ID.
 * @param Name The unique name of the component type.
 * @return The created component.
 */
CCComponent CCComponentCreateForName(const char *Name);

/*!
 * @brief Destroy a component.
 * @param Component The component to be destroyed.
 */
void CCComponentDestroy(CCComponent Component);

#endif
