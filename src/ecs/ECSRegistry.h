/*
 *  Copyright (c) 2023, Stefan Johnson
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

#ifndef CommonGameKit_ECSRegistry_h
#define CommonGameKit_ECSRegistry_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSEntity.h>

typedef CCBigIntFast ECSRegistryID;

typedef struct ECSRegistry {
    ECSRegistryID id;
    CCDictionary(ECSRegistryID, ECSEntity) registeredEntites;
    CCArray(ECSRegistryID) uniqueEntityIDs;
} ECSRegistry;

typedef struct ECSContext ECSContext;

/*!
 * @brief Initialise the registry with a given starting ID.
 * @param Context The context to initialise the registry of.
 * @param ID The starting ID.
 */
void ECSRegistryInit(ECSContext *Context, ECSRegistryID ID);

/*!
 * @brief Register an entity with the registry.
 * @param Context The context to register the entity with.
 * @param Entity The entity to be registered.
 * @return Returns the registry ID for the registered entity. If the entity was already registered the same ID will be returned.
 */
ECSRegistryID ECSRegistryRegister(ECSContext *Context, ECSEntity Entity);

/*!
 * @brief Deregister an entity.
 * @description Deregistering an entity doesn't make the registry ID available again. To reuse that ID, @b ECSRegistryReregister
 *              must be used.
 *
 * @param Context The context to deregister the entity from.
 * @param Entity The entity to be deregistered.
 */
void ECSRegistryDeregister(ECSContext *Context, ECSEntity Entity);

/*!
 * @brief Reregister an entity with the provided ID.
 * @param Context The context to reregister the entity with.
 * @param Entity The entity to be registered.
 * @param ID The ID to register the entity with.
 * @param AcquireID Whether a pre-existing registration should be replaced (TRUE) or not (FALSE). If the ID should not be acquired
 *                  then the function will assert that the ID is not already in use.
 */
void ECSRegistryReregister(ECSContext *Context, ECSEntity Entity, ECSRegistryID ID, _Bool AcquireID);

/*!
 * @brief Lookup the entity for the given registry ID.
 * @param Context The context to lookup the entity for.
 * @param ID The registry ID to lookup.
 * @return Returns the entity if one is currently registered for that ID, otherwise returns @b ECS_ENTITY_NULL.
 */
ECSEntity ECSRegistryLookup(ECSContext *Context, ECSRegistryID ID);

/*!
 * @brief Get the registry ID for a registered entity.
 * @param Context The context to get the registry ID.
 * @param Entity The entity to retrieve the registry ID for.
 * @return Returns the registry ID for the entity or NULL if there isn't one.
 */
ECSRegistryID ECSRegistryGetID(ECSContext *Context, ECSEntity Entity);

#endif
