//
//  Entity.h
//  Blob Game
//
//  Created by Stefan Johnson on 25/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Entity_h
#define Blob_Game_Entity_h

#include <stdint.h>
#include <CommonC/Common.h>
#include "Component.h"

/*!
 * @brief An ID to identify the entity.
 * @description May be unique, or may be used for multiple entities to identify them all together.
 */
typedef uint32_t CCEntityID;

/*!
 * @brief The entity.
 */
typedef struct CCEntityInfo *CCEntity;


/*!
 * @brief Create a empty entity.
 * @performance Suggested usage is to use one allocator for static entities and another allocator(s)
 *              for dynamic entities.
 *
 * @param id The id to identify this entity.
 * @param Allocator The allocator to be used to allocate the entity with.
 * @return The created entity.
 */
CCEntity CCEntityCreate(CCEntityID id, CCAllocatorType Allocator);

/*!
 * @brief Destroy an entity.
 * @param Entity The entity to be destroyed.
 */
void CCEntityDestroy(CCEntity Entity);

/*!
 * @brief Attach a component to an entity.
 * @param Entity The entity to attach the component to.
 * @param Component The component to be attached.
 */
void CCEntityAttachComponent(CCEntity Entity, CCComponent Component);

/*!
 * @brief Detach a component from an entity.
 * @param Entity The entity to detach the component from.
 * @param Component The component to be detached.
 */
void CCEntityDetachComponent(CCEntity Entity, CCComponent Component);

#endif
