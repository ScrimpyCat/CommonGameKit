/*
 *  Copyright (c) 2022, Stefan Johnson
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

#ifndef CommonGameKit_ECSEntity_h
#define CommonGameKit_ECSEntity_h

#include <CommonGameKit/Base.h>

/*!
 * @brief An entity.
 */
typedef size_t ECSEntity;

/*!
 * @brief A proxy entity that can either be an @b ECSEntity or a relative entity index.
 */
typedef size_t ECSProxyEntity;

#define ECS_RELATIVE_ENTITY_FLAG ~(SIZE_MAX >> 1)

#define ECS_RELATIVE_ENTITY(x) ((x) | ECS_RELATIVE_ENTITY_FLAG)

#define ECS_ENTITY_NULL ((ECSEntity)-1)

/*!
 * @brief Resolve a proxy entity.
 * @param ProxyEntity The proxy entity to be resolved.
 * @param Entities The entities that can be referenced by a relative entity.
 * @param Count The number of entities.
 * @return Returns the resolved entity.
 */
static inline ECSEntity ECSProxyEntityResolve(ECSProxyEntity ProxyEntity, ECSEntity *Entities, size_t Count);

#pragma mark -

static inline ECSEntity ECSProxyEntityResolve(ECSProxyEntity ProxyEntity, ECSEntity *Entities, size_t Count)
{
    if (ProxyEntity & ECS_RELATIVE_ENTITY_FLAG)
    {
        ProxyEntity = ProxyEntity & ~ECS_RELATIVE_ENTITY_FLAG;
        
        CCAssertLog(ProxyEntity < Count, "Relative entity (%zu) is out of bounds (%zu)", ProxyEntity, Count);
        
        return Entities[ProxyEntity];
    }
    
    return ProxyEntity;
}

#endif
