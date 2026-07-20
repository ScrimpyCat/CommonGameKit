/*
 *  Copyright (c) 2025, Stefan Johnson
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

#ifndef CommonGameKit_GFXDepth_h
#define CommonGameKit_GFXDepth_h

#include <CommonGameKit/GFXCondition.h>

typedef enum {
    GFXDepthPassMask = 7,
    GFXDepthPassNever = GFXConditionPassNever,
    GFXDepthPassIfEqual = GFXConditionPassIfEqual,
    GFXDepthPassIfNotEqual = GFXConditionPassIfNotEqual,
    GFXDepthPassIfLessThan = GFXConditionPassIfLessThan,
    GFXDepthPassIfLessThanEqual = GFXConditionPassIfLessThanEqual,
    GFXDepthPassIfGreaterThan = GFXConditionPassIfGreaterThan,
    GFXDepthPassIfGreaterThanEqual = GFXConditionPassIfGreaterThanEqual,
    GFXDepthPassAlways = GFXConditionPassAlways,
    
    GFXDepthWrite = (1 << 3)
} GFXDepth;

/*!
 * @brief Get the condition function.
 * @param DepthMask The depth mask.
 * @return The condition function.
 *         @b GFXDepthPassNever
 *         @b GFXDepthPassIfEqual
 *         @b GFXDepthPassIfNotEqual
 *         @b GFXDepthPassIfLessThan
 *         @b GFXDepthPassIfLessThanEqual
 *         @b GFXDepthPassIfGreaterThan
 *         @b GFXDepthPassIfGreaterThanEqual
 *         @b GFXDepthPassAlways
 *
 */
static inline GFXCondition GFXDepthGetFunction(GFXDepth DepthMask);

/*!
 * @brief Get whether the depth buffer will be written to.
 * @param DepthMask The depth mask.
 * @return Whether the depth buffer will be written to.
 */
static inline _Bool GFXDepthIsWritable(GFXDepth DepthMask);

#pragma mark -

static inline GFXCondition GFXDepthGetFunction(GFXDepth DepthMask)
{
    return DepthMask & GFXDepthPassMask;
}

static inline _Bool GFXDepthIsWritable(GFXDepth DepthMask)
{
    return DepthMask & GFXDepthWrite;
}

#endif
