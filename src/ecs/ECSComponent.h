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

#ifndef CommonGameKit_ECSComponent_h
#define CommonGameKit_ECSComponent_h

#include <CommonGameKit/ECSArchetype.h>

typedef ECSArchetype(1) ECSPackedComponent;

typedef CCArray ECSIndexedComponent;

typedef uint32_t ECSComponentID;

typedef CC_FLAG_ENUM(ECSComponentStorage, ECSComponentID) {
    ECSComponentStorageTypeArchetype = (0 << 28),
    ECSComponentStorageTypePacked = (1 << 28),
    ECSComponentStorageTypeIndexed = (2 << 28),
    ECSComponentStorageTypeLocal = (3 << 28),
    
    ECSComponentStorageModifierDuplicate = (1 << 30),
    ECSComponentStorageModifierTag = (1 << 31),
    
    ECSComponentStorageTypeMask = 0x30000000,
    ECSComponentStorageModifierMask = 0xc0000000,
    ECSComponentStorageMask = ECSComponentStorageTypeMask | ECSComponentStorageModifierMask
};

typedef struct {
    ECSComponentID id;
    void *data;
} ECSTypedComponent;

#endif
