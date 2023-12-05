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

#ifndef CommonGameKit_ECSLink_h
#define CommonGameKit_ECSLink_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ECSEntity.h>
#include <CommonGameKit/ECSComponent.h>

typedef CC_ENUM(ECSLinkType, uint32_t) {
    /// Associate the argument with the left entity
    ECSLinkTypeWithLeft = 0,
    /// Associate the argument with the right entity
    ECSLinkTypeWithRight = 8,
    
    ECSLinkTypeGroupMask = 1,
    ECSLinkTypeGroupOne = 0,
    ECSLinkTypeGroupMany = 1,
    
    ECSLinkTypeAssociateMask = (3 << 1),
    /// Associate nothing with the entity
    ECSLinkTypeAssociateNone = (0 << 1),
    /// Associate component data with the entity
    ECSLinkTypeAssociateComponent = (1 << 1),
    /// Associate a add/remove callbacks with the entity
    ECSLinkTypeAssociateCallback = (2 << 1),
    
    ECSLinkTypeDeletionMask = (1 << 3),
    /// Don't delete any entities upon removal of the link.
    ECSLinkTypeDeletionNone = (0 << 3),
    /// A cascading link will destroy this entity on removal of the link.
    ECSLinkTypeDeletionCascading = (1 << 3),
    
    ECSLinkTypeRelationshipMask = (1 << ECSLinkTypeWithLeft) | (1 << ECSLinkTypeWithRight),
    /// Link one entity to one other entity
    ECSLinkTypeRelationshipOneToOne = (ECSLinkTypeGroupOne << ECSLinkTypeWithLeft) | (ECSLinkTypeGroupOne << ECSLinkTypeWithRight),
    /// Link one entity to many other entities
    ECSLinkTypeRelationshipOneToMany = (ECSLinkTypeGroupOne << ECSLinkTypeWithLeft) | (ECSLinkTypeGroupMany << ECSLinkTypeWithRight),
    /// Link many entities to many other entities
    ECSLinkTypeRelationshipManyToMany = (ECSLinkTypeGroupMany << ECSLinkTypeWithLeft) | (ECSLinkTypeGroupMany << ECSLinkTypeWithRight),
};

_Static_assert(_Alignof(ECSLinkType) > 1, "Expects an alignment greater than 1");

typedef struct ECSContext ECSContext;

typedef void (*ECSLinkAddCallback)(ECSContext *Context, ECSEntity Entity, void *Data);
typedef void (*ECSLinkRemoveCallback)(ECSContext *Context, ECSEntity Entity);

typedef struct ECSLink {
    ECSLinkType type;
    union {
        struct {
            ECSComponentID id;
            void *data;
        } component;
        struct {
            ECSLinkAddCallback add;
            ECSLinkRemoveCallback remove;
        } callback;
    } associate[2];
} ECSLink;

typedef struct ECSLinkMap {
    CCArray(CCDictionary) associations;
} ECSLinkMap;

#define ECS_LINK_INVERT(link) ((void*)((uintptr_t)(link) ^ 1))

#define ECS_LINK_IS_INVERTED(link) ((_Bool)((uintptr_t)(link) & 1))

/*!
 * @brief Initialise the link map.
 * @param Context The context to initialise the link map of.
 */
void ECSLinkMapInit(ECSContext *Context);

/*!
 * @brief Add a link between two entities.
 * @param Context The context to add the link to.
 * @param EntityA The left entity. If the link is inverted then this will be the right entity.
 * @param DataA The data to be used to associate with @b EntityA. If @b ECSLinkTypeAssociateNone was used for the entity, then this
 *              argument is ignored. If @b ECSLinkTypeAssociateComponent was used for the entity, then if this argument is not NULL
 *              it is passed as the initial component data instead of the static data referenced in the link, if the argument is NULL
 *              then the data in the link is used instead. If @b ECSLinkTypeAssociateCallback was used for the entity, then this argument
 *              is passed to the @b ECSLinkAddCallback function of the link.
 *
 * @param Link The link to be used for the connection. To invert the order of the link use @b ECS_LINK_INVERT(link). The link is expected
 *             to have an alignment greater than 1.
 *
 * @param EntityB The right entity. If the link is inverted then this will be the left entity.
 * @param DataB The data to be used to associate with @b EntityB. If @b ECSLinkTypeAssociateNone was used for the entity, then this
 *              argument is ignored. If @b ECSLinkTypeAssociateComponent was used for the entity, then if this argument is not NULL
 *              it is passed as the initial component data instead of the static data referenced in the link, if the argument is NULL
 *              then the data in the link is used instead. If @b ECSLinkTypeAssociateCallback was used for the entity, then this argument
 *              is passed to the @b ECSLinkAddCallback function of the link.
 */
void ECSLinkAdd(ECSContext *Context, ECSEntity EntityA, void *DataA, const ECSLink *Link, ECSEntity EntityB, void *DataB);

/*!
 * @brief Remove a specific link for the two entities.
 * @param Context The context to remove the link from.
 * @param EntityA The left entity. If the link is inverted then this will be the right entity.
 * @param Link The link to be removed. To invert the order of the link use @b ECS_LINK_INVERT(link). The link is expected to have an
 *             alignment greater than 1.
 *
 * @param EntityB The right entity. If the link is inverted then this will be the left entity.
 */
void ECSLinkRemove(ECSContext *Context, ECSEntity EntityA, const ECSLink *Link, ECSEntity EntityB);

/*!
 * @brief Remove all instances of the given link type for the entity.
 * @param Context The context to remove the link from.
 * @param Entity The left entity. If the link is inverted then this will be the right entity.
 * @param Link The link to be removed. To invert the order of the link use @b ECS_LINK_INVERT(link). The link is expected to have an
 *             alignment greater than 1.
 */
void ECSLinkRemoveLinkForEntity(ECSContext *Context, ECSEntity Entity, const ECSLink *Link);

/*!
 * @brief Remove all links for the entity.
 * @param Context The context to remove the link from.
 * @param Entity The entity to have all its linked removed.
 */
void ECSLinkRemoveAllLinksForEntity(ECSContext *Context, ECSEntity Entity);

/*!
 * @brief Remove all linkes between the two entities.
 * @param Context The context to remove the link from.
 * @param EntityA The first entity.
 * @param EntityB The second entity.
 */
void ECSLinkRemoveAllLinksBetweenEntities(ECSContext *Context, ECSEntity EntityA, ECSEntity EntityB);

/*!
 * @brief Remove all instances of the given link type for all entities.
 * @param Context The context to remove the link from.
 * @param Link The link to be removed. Link order is ignored.
 */
void ECSLinkRemoveLink(ECSContext *Context, const ECSLink *Link);

#pragma - Query

/*!
 * @brief Check whether two entities are linked together with the specified link.
 * @param Context The context to query the link of.
 * @param EntityA The left entity. If the link is inverted then this will be the right entity.
 * @param Link The link to be queried. To invert the order of the link use @b ECS_LINK_INVERT(link). The link is expected to have an
 *             alignment greater than 1.
 *
 * @param EntityB The right entity. If the link is inverted then this will be the left entity.
 * @return Return whether the entities are linked (TRUE), or not (FALSE).
 */
_Bool ECSLinked(ECSContext *Context, ECSEntity EntityA, const ECSLink *Link, ECSEntity EntityB);

/*!
 * @brief Get the entities the entity is linked to with the specified link.
 * @param Context The context to query the link of.
 * @param Entity The entity to get the entities linked to.
 * @param Link The link to be queried. To invert the order of the link use @b ECS_LINK_INVERT(link). The link is expected to have an
 *             alignment greater than 1.
 *
 * @param Count A pointer to where to set the number of returned entities.
 * @return Return a pointer to the linked entities. Or NULL if there aren't any.
 */
const ECSEntity *ECSLinkGet(ECSContext *Context, ECSEntity Entity, const ECSLink *Link, size_t *Count);

/*!
 * @brief Get the link enumerable for the given entity.
 * @param Context The context to query the link of.
 * @param Entity The entity to get the entities linked to.
 * @param Enumerable A pointer to where to store the link enumerable.
 */
void ECSLinkEnumerable(ECSContext *Context, ECSEntity Entity, CCEnumerable *Enumerable);

#endif
