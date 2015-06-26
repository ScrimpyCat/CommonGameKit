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

#endif
