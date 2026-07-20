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

#ifndef CommonGameKit_GFXStencil_h
#define CommonGameKit_GFXStencil_h

#include <CommonGameKit/GFXCondition.h>

typedef enum {
    GFXStencilOperationMask = 7,
    /// Keep the stencil value.
    GFXStencilOperationKeep = 0,
    /// Set the stencil value to zero.
    GFXStencilOperationZero,
    /// Replace the stencil value with the reference value.
    GFXStencilOperationReplace,
    /// Increment the stencil value clamping it to the max value. value = min(value + 1, max)
    GFXStencilOperationIncrementClamp,
    /// Decrement the stencil value clamping it to 0. value = max(value - 1, 0)
    GFXStencilOperationDecrementClamp,
    /// Increment the stencil value wrapping around to 0 once it exceeds the max value.
    GFXStencilOperationIncrementWrap,
    /// Decrement the stencil value wrapping around to the max value once it goes below 0.
    GFXStencilOperationDecrementWrap,
    /// Invert the stencil value.
    GFXStencilOperationInvert
} GFXStencilOperation;

typedef enum {
    GFXStencilNone = 0,
    
    GFXStencilFaceFront = 0,
    GFXStencilFaceBack = 12,
    
    GFXStencilFail = 9,
    GFXStencilFailDepth = 6,
    GFXStencilPass = 3,
    
    GFXStencilPassMask = 7,
    GFXStencilPassNever = GFXConditionPassNever,
    GFXStencilPassIfEqual = GFXConditionPassIfEqual,
    GFXStencilPassIfNotEqual = GFXConditionPassIfNotEqual,
    GFXStencilPassIfLessThan = GFXConditionPassIfLessThan,
    GFXStencilPassIfLessThanEqual = GFXConditionPassIfLessThanEqual,
    GFXStencilPassIfGreaterThan = GFXConditionPassIfGreaterThan,
    GFXStencilPassIfGreaterThanEqual = GFXConditionPassIfGreaterThanEqual,
    GFXStencilPassAlways = GFXConditionPassAlways
} GFXStencil;

/*!
 * @brief Get the condition function.
 * @param StencilMask The stencil mask.
 * @param Face The face to get the condition for.
 * @return The condition function.
 *         @b GFXStencilPassNever
 *         @b GFXStencilPassIfEqual
 *         @b GFXStencilPassIfNotEqual
 *         @b GFXStencilPassIfLessThan
 *         @b GFXStencilPassIfLessThanEqual
 *         @b GFXStencilPassIfGreaterThan
 *         @b GFXStencilPassIfGreaterThanEqual
 *         @b GFXStencilPassAlways
 *
 */
static inline GFXCondition GFXStencilGetFunction(GFXStencil StencilMask, GFXStencil Face);

/*!
 * @brief Get the pass/fail operation.
 * @param StencilMask The stencil mask.
 * @param Face The face to get the operation for.
 * @param Type The pass/fail type to get the operation for.
 * @return The pass/fail operation.
 */
static inline GFXStencilOperation GFXStencilGetOperation(GFXStencil StencilMask, GFXStencil Face, GFXStencil Type);

/*!
 * @brief Create a front facing stencil mask.
 * @description Can be OR'd with a front facing mask.
 * @param StencilFailOperation The operation to perform when the stencil test fails.
 * @param DepthFailOperation The operation to perform when the depth test fails.
 * @param PassOperation The operation to perform when the stencil and depth tests pass.
 * @param StencilCondition The stencil test condition.
 * @return The front facing stencil mask.
 */
static inline GFXStencil GFXStencilFront(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition);

/*!
 * @brief Create a back facing stencil mask.
 * @description Can be OR'd with a front facing mask.
 * @param StencilFailOperation The operation to perform when the stencil test fails.
 * @param DepthFailOperation The operation to perform when the depth test fails.
 * @param PassOperation The operation to perform when the stencil and depth tests pass.
 * @param StencilCondition The stencil test condition.
 * @return The back facing stencil mask.
 */
static inline GFXStencil GFXStencilBack(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition);

/*!
 * @brief Create a front and back facing stencil mask.
 * @param StencilFailOperation The operation to perform when the stencil test fails.
 * @param DepthFailOperation The operation to perform when the depth test fails.
 * @param PassOperation The operation to perform when the stencil and depth tests pass.
 * @param StencilCondition The stencil test condition.
 * @return The front and back facing stencil mask.
 */
static inline GFXStencil GFXStencilFrontBack(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition);

#pragma mark -

static inline GFXCondition GFXStencilGetFunction(GFXStencil StencilMask, GFXStencil Face)
{
    CCAssertLog((Face == GFXStencilFaceFront) || (Face == GFXStencilFaceBack), "Face must be front or back");
    
    return (StencilMask >> Face) & GFXStencilPassMask;
}

static inline GFXStencilOperation GFXStencilGetOperation(GFXStencil StencilMask, GFXStencil Face, GFXStencil Type)
{
    CCAssertLog((Face == GFXStencilFaceFront) || (Face == GFXStencilFaceBack), "Face must be front or back");
    CCAssertLog((Type == GFXStencilFail) || (Type == GFXStencilFailDepth) || (Type == GFXStencilPass), "Type must be a fail or pass");
    
    return ((StencilMask >> Face) >> Type) & GFXStencilOperationMask;
}

static inline GFXStencil GFXStencilFront(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition)
{
    return ((StencilFailOperation << GFXStencilFail) | (DepthFailOperation << GFXStencilFailDepth) | (PassOperation << GFXStencilPass) | StencilCondition) << GFXStencilFaceFront;
}

static inline GFXStencil GFXStencilBack(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition)
{
    return ((StencilFailOperation << GFXStencilFail) | (DepthFailOperation << GFXStencilFailDepth) | (PassOperation << GFXStencilPass) | StencilCondition) << GFXStencilFaceBack;
}

static inline GFXStencil GFXStencilFrontBack(GFXStencilOperation StencilFailOperation, GFXStencilOperation DepthFailOperation, GFXStencilOperation PassOperation, GFXCondition StencilCondition)
{
    return GFXStencilFront(StencilFailOperation, DepthFailOperation, PassOperation, StencilCondition) | GFXStencilBack(StencilFailOperation, DepthFailOperation, PassOperation, StencilCondition);
}

#endif
