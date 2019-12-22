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

#ifndef CommonGameKit_EntityManager_h
#define CommonGameKit_EntityManager_h

#include <CommonGameKit/Entity.h>

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
 * @description Assigns the entity a unique ID.
 * @param Entity The entity to be managed.
 */
void CCEntityManagerAddEntity(CCEntity CC_OWN(Entity));

/*!
 * @brief Adds an entity to be managed by the global entity manager.
 * @param Entity The entity to be managed.
 * @param ID The unique id to assign to the entity. This should only be an ID that is
 *        retrieved from @b CCEntityGetID.
 */
void CCEntityManagerAddEntityWithID(CCEntity CC_OWN(Entity), CCString ID);

/*!
 * @brief Removes an entity from being managed by the global entity manager.
 * @warning After this call it is no longer safe to reference the entity.
 * @param Entity The entity being managed.
 */
void CCEntityManagerRemoveEntity(CCEntity CC_DESTROY(Entity));

/*!
 * @brief Get the current entities from the manager.
 * @warning Should obtain locks to the entity manager prior to calling this function.
 * @return The current entity list.
 */
CCCollection(CCEntity) CCEntityManagerGetEntities(void);

/*!
 * @brief Get the entity with ID from the manager.
 * @warning Should obtain locks to the entity manager prior to calling this function.
 * @return The entity with the given ID.
 */
CCEntity CCEntityManagerGetEntity(CCString ID);

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
