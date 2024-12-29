/*
 *  Copyright (c) 2024, Stefan Johnson
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

#include "ReflectedTypes.h"
#include "Input.h"

#ifndef CC_REFLECT_MAP_STATELESS_VALUE_MAPPED_INDEX_BASE
#define CC_REFLECT_MAP_STATELESS_VALUE_MAPPED_INDEX_BASE 10
#endif

void CCReflectOpaqueTypeDescriptorMapCGKDefaults(CCReflectType Type, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator, CCReflectMapIntent Intent)
{
#define CC_REFLECT_VALUE_OPAQUE_MAPPER(value) value##Mapper
#define CC_REFLECT_VALUE_OPAQUE_UNMAPPER(value) value##Unmapper
    
#define CC_REFLECT_STATELESS_OPAQUE_TYPES \
    CCInputMap
    
    if CC_REFLECT_MAP_STATELESS_VALUES_WHEN(CC_REFLECT_VALUE_IS_TYPE, CC_REFLECT_STATELESS_OPAQUE_TYPES)
    else CCReflectOpaqueTypeDescriptorMapDefaults(Type, Data, Args, Handler, Zone, Allocator, Intent);
}

void CCReflectOpaqueTypeDescriptorUnmapCGKDefaults(CCReflectType Type, CCReflectType MappedType, const void *Data, void *Args, CCReflectTypeHandler Handler, CCMemoryZone Zone, CCAllocatorType Allocator)
{
    if CC_REFLECT_UNMAP_STATELESS_VALUES(CC_REFLECT_VALUE_TO_OPAQUE, CC_REFLECT_STATELESS_OPAQUE_TYPES)
    
    CCReflectOpaqueTypeDescriptorUnmapDefaults(Type, MappedType, Data, Args, Handler, Zone, Allocator);
}
