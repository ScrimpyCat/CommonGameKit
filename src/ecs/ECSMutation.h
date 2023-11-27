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

#ifndef CommonGameKit_ECSMutation_h
#define CommonGameKit_ECSMutation_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSComponent.h>
#include <CommonGameKit/ECSEntity.h>
#include <CommonGameKit/ECSContext.h>

/*!
 * @brief A mutation callback.
 * @param Context The ECS context the mutation is being applied to.
 * @param Data The data associated with this callback.
 * @param NewEntities An array of entity IDs for the newly created entities in this mutation.
 * @param NewEntityCount The number of newly created entities.
 */
typedef void (*ECSMutableCallback)(ECSContext *Context, void *Data, ECSEntity *NewEntities, size_t NewEntityCount);

typedef struct {
    ECSProxyEntity entity;
    ECSRegistryID id;
    _Bool acquire;
} ECSMutableReplaceRegistryState;

typedef struct {
    ECSProxyEntity entity;
    size_t count;
    ECSTypedComponent *components;
} ECSMutableAddComponentState;

typedef struct {
    ECSProxyEntity entity;
    size_t count;
    ECSComponentID *ids;
} ECSMutableRemoveComponentState;

typedef struct {
    ECSMutableCallback callback;
    void *data;
} ECSMutableCustomCallbackState;

typedef struct ECSMutableState {
    struct {
        _Atomic(size_t) create;
        struct {
            _Atomic(size_t) count;
            ECSEntity *entities;
        } remove;
    } entity;
    struct {
        struct {
            _Atomic(size_t) count;
            ECSProxyEntity *entities;
        } add;
        struct {
            _Atomic(size_t) count;
            ECSEntity *entities;
        } remove;
        struct {
            _Atomic(size_t) count;
            ECSMutableReplaceRegistryState *state;
        } replace;
    } registry;
    struct {
        struct {
            _Atomic(size_t) count;
            ECSMutableAddComponentState *state;
        } add;
        struct {
            _Atomic(size_t) count;
            ECSMutableRemoveComponentState *state;
        } remove;
    } component;
    struct {
        _Atomic(size_t) count;
        ECSMutableCustomCallbackState *state;
    } custom;
    struct {
        _Atomic(size_t) size;
        void *data;
    } shared;
} ECSMutableState;

#define ECS_MUTABLE_STATE_INIT \
.entity = { \
    .create = ATOMIC_VAR_INIT(0), \
    .remove = { \
        .count = ATOMIC_VAR_INIT(0) \
    } \
}, \
.registry = { \
    .add = { \
        .count = ATOMIC_VAR_INIT(0) \
    }, \
    .remove = { \
        .count = ATOMIC_VAR_INIT(0) \
    }, \
    .replace = { \
        .count = ATOMIC_VAR_INIT(0) \
    } \
}, \
.component = { \
    .add = { \
        .count = ATOMIC_VAR_INIT(0) \
    }, \
    .remove = { \
        .count = ATOMIC_VAR_INIT(0) \
    } \
}, \
.custom = { \
    .count = ATOMIC_VAR_INIT(0) \
}, \
.shared = { \
    .size = ATOMIC_VAR_INIT(0) \
}

#define ECS_MUTABLE_STATE_CREATE(entitiesMax, replaceRegistryMax, addComponentMax, removeComponentMax, customCallbackMax, sharedDataMax) (ECSMutableState){ \
    ECS_MUTABLE_STATE_INIT, \
    .entity.remove.entities = (ECSEntity[entitiesMax]){}, \
    .registry.add.entities = (ECSEntity[entitiesMax]){}, \
    .registry.remove.entities = (ECSEntity[entitiesMax]){}, \
    .registry.replace.state = (ECSMutableReplaceRegistryState[replaceRegistryMax]){}, \
    .component.add.state = (ECSMutableAddComponentState[addComponentMax]){}, \
    .component.remove.state = (ECSMutableRemoveComponentState[removeComponentMax]){}, \
    .custom.state = (ECSMutableCustomCallbackState[customCallbackMax]){}, \
    .shared.data = (uint8_t[sharedDataMax]){} \
}

/*!
 * @brief Set the maximum number of entities that can be staged in a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateEntitiesMax;

/*!
 * @brief Set the maximum number of replace registry states that can be staged in a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateReplaceRegistryMax;

/*!
 * @brief Set the maximum number of add component states that can be staged in a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateAddComponentMax;

/*!
 * @brief Set the maximum number of remove component states that can be staged in a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateRemoveComponentMax;

/*!
 * @brief Set the maximum number of custom callback states that can be staged in a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateCustomCallbackMax;

/*!
 * @brief Set the maximum size of the shared data that can be used by a mutation.
 * @warning This must be set prior to any mutation calls.
 */
extern size_t ECSMutableStateSharedDataMax;

/*!
 * @brief Get some memory that can store data of size.
 * @param Context The ECS context to set the shared mutation data for. Must not be NULL.
 * @param Size The size of the data to be stored.
 * @return Returns the pointer to the memory that can be written to.
 */
void *ECSMutationSetSharedData(ECSContext *Context, size_t Size);

/*!
 * @brief Get the shared data
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to get the shared mutation data for. Must not be NULL.
 * @return Returns the pointer to the shared data.
 */
void *ECSMutationGetSharedData(ECSContext *Context);

/*!
 * @brief Stage an entity create mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of entities to be created.
 * @return Returns the relative index for the first entity. Each entity can be referenced from this index up to index + (count - 1).
 */
ECSProxyEntity ECSMutationStageEntityCreate(ECSContext *Context, size_t Count);

/*!
 * @brief Stage an entity destroy mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of entities to be destroyed.
 * @return A pointer to an array (of size @b Count) to store the entities that will be destroyed.
 */
ECSEntity *ECSMutationStageEntityDestroy(ECSContext *Context, size_t Count);

/*!
 * @brief Stage an entity register mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of entities to be registered.
 * @return A pointer to an array (of size @b Count) to store the entities that will be registered.
 */
ECSProxyEntity *ECSMutationStageRegistryRegister(ECSContext *Context, size_t Count);

/*!
 * @brief Stage an entity deregister mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of entities to be deregistered.
 * @return A pointer to an array (of size @b Count) to store the entities that will be deregistered.
 */
ECSEntity *ECSMutationStageRegistryDeregister(ECSContext *Context, size_t Count);

/*!
 * @brief Stage an entity reregister mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of entities to be reregistered.
 * @return A pointer to an array (of size @b Count) to store the entities that will be reregistered.
 */
ECSMutableReplaceRegistryState *ECSMutationStageRegistryReregister(ECSContext *Context, size_t Count);

/*!
 * @brief Stage an add components mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of add component requests.
 * @return A pointer to an array to store the add component states. The state should only be set to data that is safe to access from the thread that will
 *         apply it, or can store the data in the memory from @b ECSMutationSetSharedData.
 */
ECSMutableAddComponentState *ECSMutationStageEntityAddComponents(ECSContext *Context, size_t Count);

/*!
 * @brief Stage a remove components mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of remove component requests.
 * @return A pointer to an array to store the remove component states. The state should only be set to data that is safe to access from the thread that will
 *         apply it, or can store the data in the memory from @b ECSMutationSetSharedData.
 */
ECSMutableRemoveComponentState *ECSMutationStageEntityRemoveComponents(ECSContext *Context, size_t Count);

/*!
 * @brief Stage a custom callback mutation.
 * @param Context The ECS context to stage mutations for. Must not be NULL.
 * @param Count The number of callbacks to add.
 * @return A pointer to an array to store the custom callback states. The state should only be set to data that is safe to access from the thread that will
 *         apply it, or can store the data in the memory from @b ECSMutationSetSharedData.
 */
ECSMutableCustomCallbackState *ECSMutationStageCustomCallback(ECSContext *Context, size_t Count);

/*!
 * @brief Get the number of staged entity create requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @return Returns the number of entities that will be created.
 */
size_t ECSMutationInspectEntityCreate(ECSContext *Context);

/*!
 * @brief Get the number of staged entity destroy requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of entities that will be destroyed.
 */
ECSEntity *ECSMutationInspectEntityDestroy(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged entity register requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of entities that will be registered.
 */
ECSProxyEntity *ECSMutationInspectRegistryRegister(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged entity deregister requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of entities that will be deregistered.
 */
ECSEntity *ECSMutationInspectRegistryDeregister(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged entity reregister requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of entities that will be reregistered.
 */
ECSMutableReplaceRegistryState *ECSMutationInspectRegistryReregister(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged add component requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of add component states.
 */
ECSMutableAddComponentState *ECSMutationInspectEntityAddComponents(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged remove component requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of remove component states.
 */
ECSMutableRemoveComponentState *ECSMutationInspectEntityRemoveComponents(ECSContext *Context, size_t *Count);

/*!
 * @brief Get the number of staged custom callback requests.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context to inspect. Must not be NULL.
 * @param Count A pointer to where the size of the array should be stored. Must not be NULL.
 * @return Returns a pointer to an array of custom callback states.
 */
ECSMutableCustomCallbackState *ECSMutationInspectCustomCallback(ECSContext *Context, size_t *Count);

/*!
 * @brief Apply all staged mutations.
 * @warning This is not threadsafe. It shouldn't be used at the same time other mutation calls are made on the context.
 * @param Context The ECS context that the mutations will be applied to. Must not be NULL.
 */
void ECSMutationApply(ECSContext *Context);

#endif
