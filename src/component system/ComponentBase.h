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

#ifndef CommonGameKit_ComponentBase_h
#define CommonGameKit_ComponentBase_h

#include <CommonGameKit/Component.h>
#include <CommonGameKit/Entity.h>

#define CC_COMPONENT_ID 0

typedef struct {
    CCComponentID id;
    CCEntity entity;
} CCComponentClass, *CCComponentPrivate;

/*!
 * @brief Get the entities for the list of components.
 * @description If the component list is an ordered list, the entity list will be ordered too.
 * @param Components The list of components.
 * @return The list of entities. This must be destroyed.
 */
CC_NEW CCCollection(CCEntity) CCComponentGetEntities(CCCollection(CCComponent) Components);

#pragma mark - Base Component Functions
/*!
 * @brief Initialize the component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the component.
 * @param Component The component to be deallocated.
 */
static inline void CCComponentDeallocate(CCComponent Component);

/*!
 * @brief Get the type ID of the component.
 * @param Component The component.
 * @return The component ID.
 */
static inline CCComponentID CCComponentGetID(CCComponent Component);

/*!
 * @brief Get the entity the component belongs to.
 * @param Component The component.
 * @return The entity of the component.
 */
static inline CCEntity CCComponentGetEntity(CCComponent Component);

/*!
 * @brief Set the entity the component belongs to.
 * @note Should typically use @b CCEntityAttachComponent and @b CCEntityDetachComponent instead.
 * @param Component The component.
 * @param Entity The entity this component will belong to.
 */
static inline void CCComponentSetEntity(CCComponent Component, CCEntity Entity);

/*!
 * @brief Set whether this component is managed by a system.
 * @note Should typically use @b CCComponentSystemAddComponent and @b CCComponentSystemRemoveComponent
 *       instead.
 *
 * @param Component The component.
 * @param Managed Whether the entity is being managed or not.
 */
static inline void CCComponentSetIsManaged(CCComponent Component, _Bool Managed);

/*!
 * @brief Check whether this component is managed by a system.
 * @param Component The component.
 * @return Whether the component is managed or not.
 */
static inline _Bool CCComponentGetIsManaged(CCComponent Component);


#pragma mark -
static inline void CCComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCAssertLog(!(id & CC_COMPONENT_ID_RESERVED_MASK), "Component ID must not include any reserved bits");
    *(CCComponentPrivate)Component = (CCComponentClass){ .id = id, .entity = NULL };
}

static inline void CCComponentDeallocate(CCComponent Component)
{
}

static inline CCComponentID CCComponentGetID(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & ~CC_COMPONENT_ID_RESERVED_MASK;
}

static inline CCEntity CCComponentGetEntity(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->entity;
}

static inline void CCComponentSetEntity(CCComponent Component, CCEntity Entity)
{
    ((CCComponentPrivate)Component)->entity = Entity;
}

static inline void CCComponentSetIsManaged(CCComponent Component, _Bool Managed)
{
    ((CCComponentPrivate)Component)->id = (CC_COMPONENT_ID_RESERVED_MASK * Managed) | CCComponentGetID(Component);
}

static inline _Bool CCComponentGetIsManaged(CCComponent Component)
{
    return ((CCComponentPrivate)Component)->id & CC_COMPONENT_ID_RESERVED_MASK;
}

#endif
