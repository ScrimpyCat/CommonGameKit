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

#ifndef CommonGameKit_Entity_h
#define CommonGameKit_Entity_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/Component.h>

/*!
 * @brief The entity.
 * @description Allows @b CCRetain.
 */
typedef struct CCEntityInfo *CCEntity;


/*!
 * @brief Create a empty entity.
 * @performance Suggested usage is to use one allocator for static entities and another allocator(s)
 *              for dynamic entities.
 *
 * @param id The id to identify this entity. May be NULL, if the entity should not be identifiable.
 * @param Allocator The allocator to be used to allocate the entity with.
 * @return The created entity.
 */
CC_NEW CCEntity CCEntityCreate(CCString CC_COPY(id), CCAllocatorType Allocator);

/*!
 * @brief Destroy an entity.
 * @param Entity The entity to be destroyed.
 */
void CCEntityDestroy(CCEntity CC_DESTROY(Entity));

/*!
 * @brief Attach a component to an entity.
 * @param Entity The entity to attach the component to.
 * @param Component The component to be attached.
 */
void CCEntityAttachComponent(CCEntity Entity, CCComponent CC_OWN(Component));

/*!
 * @brief Detach a component from an entity.
 * @description Destroys the component.
 * @param Entity The entity to detach the component from.
 * @param Component The component to be detached.
 */
void CCEntityDetachComponent(CCEntity Entity, CCComponent CC_DESTROY(Component));

/*!
* @brief Get the current components for the entity.
* @return The current component list.
*/
CCCollection CCEntityGetComponents(CCEntity Entity);

#endif
