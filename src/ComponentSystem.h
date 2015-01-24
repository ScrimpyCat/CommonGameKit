//
//  ComponentSystem.h
//  Blob Game
//
//  Created by Stefan Johnson on 7/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_ComponentSystem_h
#define Blob_Game_ComponentSystem_h

#include <stdint.h>
#include "Component.h"

/*!
 * @typedef CCComponentSystemExecutionType
 * @brief The execution type of the system.
 * @constant CCComponentSystemExecutionTypeManual Needs to be manually called (CCComponentSystemUpdateCallback).
 * @constant CCComponentSystemExecutionTypeRender Runs on the render thread (CCComponentSystemTimedUpdateCallback).
 * @constant CCComponentSystemExecutionTypePhysics Runs on the physics thread (CCComponentSystemTimedUpdateCallback).
 * @constant CCComponentSystemExecutionTypeUpdate Runs on the update thread (CCComponentSystemTimedUpdateCallback).
 * @constant CCComponentSystemExecutionTypeInput Runs on the input/main thread (CCComponentSystemUpdateCallback).
 * @constant CCComponentSystemExecutionTypeIO Runs on the IO thread (CCComponentSystemUpdateCallback).
 * @constant CCComponentSystemExecutionMax The maximum amount of execution types.
 */
typedef enum {
    ///The options mask.
    CCComponentSystemExecutionOptionMask = 0xff000000,
    ///The execution type uses the timed update.
    CCComponentSystemExecutionOptionTimedUpdate = (1 << 24),
    
    CCComponentSystemExecutionTypeMask = 0x000000ff,
    ///Needs to be manually called (CCComponentSystemUpdateCallback).
    CCComponentSystemExecutionTypeManual = 0,
    ///Runs on the render thread (CCComponentSystemTimedUpdateCallback).
    CCComponentSystemExecutionTypeRender = 1 | CCComponentSystemExecutionOptionTimedUpdate,
    ///Runs on the physics thread (CCComponentSystemTimedUpdateCallback).
    CCComponentSystemExecutionTypePhysics = 2 | CCComponentSystemExecutionOptionTimedUpdate,
    ///Runs on the update thread (CCComponentSystemTimedUpdateCallback).
    CCComponentSystemExecutionTypeUpdate = 3 | CCComponentSystemExecutionOptionTimedUpdate,
    ///Runs on the input/main thread (CCComponentSystemUpdateCallback).
    CCComponentSystemExecutionTypeInput = 4,
    ///Runs on the IO thread (CCComponentSystemUpdateCallback).
    CCComponentSystemExecutionTypeIO = 5,
    ///The maximum amount of execution types.
    CCComponentSystemExecutionMax
} CCComponentSystemExecutionType;

/*!
 * @brief The unique ID of the system.
 */
typedef uint32_t CCComponentSystemID;

typedef struct {
    CCLinkedListNode node;
    CCComponent component;
} *CCComponentList;


/*!
 * @brief Generic system update callback.
 */
typedef void (*CCComponentSystemUpdateCallback)(void *Context, CCComponentList Components);

/*!
 * @brief Time based system update callback.
 */
typedef void (*CCComponentSystemTimedUpdateCallback)(double DeltaTime, CCComponentList Components);


/*!
 * @brief Handles component callback.
 * @description Determine whether a system has ownership (manages) of a particular component type.
 */
typedef _Bool (*CCComponentSystemHandlesComponentCallback)(CCComponentID id);

/*!
 * @brief Add component callback.
 * @description Called when a new component has been added to the system. If this is not implemented
 *              an explicit call to CCComponentSystemGetAddedComponentsForSystem must be made.
 *
 * @performance This callback may be less efficient than CCComponentSystemGetAddedComponentsForSystem
 *              as this incurs a system lock each time a component is added.
 */
typedef void (*CCComponentSystemAddingComponentCallback)(CCComponent Component);

/*!
 * @brief Remove component callback.
 * @description Called when a component has been removed from the system. If this is not implemented
 *              an explicit call to CCComponentSystemGetRemovedComponentsForSystem must be made.
 */
typedef void (*CCComponentSystemRemovingComponentCallback)(CCComponent Component);


/*!
 * @brief Try lock system callback.
 * @description Attempt to lock the system. It should be a recursive lock.
 */
typedef _Bool (*CCComponentSystemTryLockCallback)(void);

/*!
 * @brief Lock system callback.
 * @description Lock the system. It should be a recursive lock.
 */
typedef void (*CCComponentSystemLockCallback)(void);

/*!
 * @brief Unlock system callback.
 * @description Unlock the system. It should be a recursive lock.
 */
typedef void (*CCComponentSystemUnlockCallback)(void);


/*!
 * @brief Register a system.
 * @param id The unique ID of the system.
 * @param ExecutionType How the system should be executed.
 * @param Update The update callback of the system. Type of update depends on execution type. May be NULL.
 * @param HandlesComponent The callback to determine if the system manages the component. May be NULL.
 * @param AddingComponent The callback to let the system know a component has been added. May be NULL,
 *        must then call CCComponentSystemGetAddedComponentsForSystem.
 *
 * @param RemovingComponent The callback to let the system know a component has been removed. May be NULL,
 *        must then call CCComponentSystemGetRemovedComponentsForSystem.
 *
 * @param TryLock The callback to attempt a try lock. May be NULL.
 * @param Lock The callback to lock the system. May be NULL.
 * @param Unlock The callback to unlock the system. May be NULL.
 */
void CCComponentSystemRegister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType, CCComponentSystemUpdateCallback Update, CCComponentSystemHandlesComponentCallback HandlesComponent, CCComponentSystemAddingComponentCallback AddingComponent, CCComponentSystemRemovingComponentCallback RemovingComponent, CCComponentSystemTryLockCallback TryLock, CCComponentSystemLockCallback Lock, CCComponentSystemUnlockCallback Unlock);

/*!
 * @brief Deregister the system.
 * @warning Only provided for unit tests, not safe to call in actual program.
 * @param id The unique ID of the system.
 * @param ExecutionType The execution type the system should be removed from.
 */
void CCComponentSystemDeregister(CCComponentSystemID id, CCComponentSystemExecutionType ExecutionType);

/*!
 * @brief Run the systems of a particular execution type.
 * @description Locks the systems and calls their update callbacks.
 * @param ExecutionType The execution type to be executed.
 */
void CCComponentSystemRun(CCComponentSystemExecutionType ExecutionType);

/*!
 * @brief Get the current components for the system.
 * @warning Should obtain locks to the system prior to calling this function.
 * @return The current component list.
 */
CCComponentList CCComponentSystemGetComponentsForSystem(CCComponentSystemID id);

/*!
 * @brief Get the current added components for the system.
 * @description Adds those components to the main component list of the system, and clears the added components
 *              list.
 *
 * @warning This call must be made within a system lock.
 *
 * @param id The unique ID of the system.
 * @return The component list of the added components.
 */
CCComponentList CCComponentSystemGetAddedComponentsForSystem(CCComponentSystemID id);

/*!
 * @brief Get the current removed components for the system.
 * @description Removes those components from the main component list of the system, and clears the removed components
 *              list.
 *
 * @warning This call must be made within a system lock.
 *
 * @param id The unique ID of the system.
 * @return The component list of the removed components.
 */
CCComponentList CCComponentSystemGetRemovedComponentsForSystem(CCComponentSystemID id);

/*!
 * @brief Attempt to lock the system.
 * @param The unique ID of the system.
 * @return True if was able to lock the system, otherwise false.
 */
_Bool CCComponentSystemTryLock(CCComponentSystemID id);

/*!
 * @brief Lock the system.
 * @param The unique ID of the system.
 */
void CCComponentSystemLock(CCComponentSystemID id);

/*!
 * @brief Unlock the system.
 * @param The unique ID of the system.
 */
void CCComponentSystemUnlock(CCComponentSystemID id);


#pragma mark - Component List Functions
/*!
 * @brief Enumerate to the next component in the list.
 * @param List The current list.
 * @return The new list where the current component is the next one.
 */
static inline CCComponentList CCComponentListEnumerateNext(CCComponentList List)
{
    return (CCComponentList)CCLinkedListEnumerateNext((CCLinkedList)List);
}

/*!
 * @brief Get the current component from the list.
 * @param List The current list.
 * @return The current component in the list.
 */
static inline CCComponent CCComponentListGetCurrentComponent(CCComponentList List)
{
    return List? List->component : NULL;
}

#endif
