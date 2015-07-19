//
//  EntityManager.h
//  Blob Game
//
//  Created by Stefan Johnson on 23/02/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_EntityManager_h
#define Blob_Game_EntityManager_h

#include "Entity.h"

/*!
 * @brief Creates the global entity manager.
 */
void CCEntityManagerCreate(void);

/*!
 * @brief Destroys the global entity manager.
 */
void CCEntityManagerDestroy(void);

/*!
 * @brief Updates the global entity manager.
 * @warning This call must be made within an entity manager lock.
 */
void CCEntityManagerUpdate(void);

/*!
 * @brief Adds an entity to be managed by the global entity manager.
 * @param Entity The entity to be managed.
 */
void CCEntityManagerAddEntity(CCEntity Entity);

/*!
 * @brief Removes an entity from being managed by the global entity manager.
 * @warning After this call it is no longer safe to reference the entity.
 * @param Entity The entity being managed.
 */
void CCEntityManagerRemoveEntity(CCEntity Entity);

/*!
 * @brief Get the current entities from the manager.
 * @warning Should obtain locks to the entity manager prior to calling this function.
 * @return The current entity list.
 */
CCCollection CCEntityManagerGetEntities(void);

/*!
 * @brief Attempt to lock the entity manager.
 * @return True if was able to lock the entity manager, otherwise false.
 */
_Bool CCEntityManagerTryLock(void);

/*!
 * @brief Lock the entity manager.
 */
void CCEntityManagerLock(void);

/*!
 * @brief Unlock the entity manager.
 */
void CCEntityManagerUnlock(void);

#endif
