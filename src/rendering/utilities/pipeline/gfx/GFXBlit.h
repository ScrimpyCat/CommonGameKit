/*
 *  Copyright (c) 2016, Stefan Johnson
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

#ifndef Blob_Game_GFXBlit_h
#define Blob_Game_GFXBlit_h

#include "GFXTexture.h"
#include "GFXFramebuffer.h"

typedef struct {
    GFXFramebuffer framebuffer;
    size_t index;
    CCRect region;
} GFXBlitFramebuffer;

typedef struct {
    void *internal;
    GFXBlitFramebuffer source;
    GFXBlitFramebuffer destination;
    GFXTextureHint filter;
} GFXBlitInfo;

/*!
 * @brief The graphics blit operation.
 * @description Allows @b CCRetain.
 */
typedef GFXBlitInfo *GFXBlit;

/*!
 * @brief Create a blit operation.
 * @description To commit a blit it must be submitted using @b GFXBlitSubmit.
 *              One blit command can be reused and submitted as many times as
 *              needed. State of a blit command can also be changed before each
 *              submit.
 *
 * @param Allocator The allocator to be used for the allocations.
 * @return The created blit operation.
 */
CC_NEW GFXBlit GFXBlitCreate(CCAllocatorType Allocator);

/*!
 * @brief Destroy a blit operation.
 * @param Blit The blit operation to be destroyed.
 */
void GFXBlitDestroy(GFXBlit CC_DESTROY(Blit));

/*!
 * @brief Submit the blit command.
 * @param Blit The blit operation.
 */
void GFXBlitSubmit(GFXBlit Blit);

/*!
 * @brief Set the source to be used for the blit command.
 * @param Blit The blit operation.
 * @param Framebuffer The framebuffer to be used as the source. Retains a reference to
 *        the framebuffer.
 * @param Index The framebuffer attachment to copy from.
 * @param Region The region to copy.
 */
void GFXBlitSetSource(GFXBlit Blit, GFXFramebuffer CC_RETAIN(Framebuffer), size_t Index, CCRect Region);

/*!
 * @brief Set the destination to be used for the blit command.
 * @param Blit The blit operation.
 * @param Framebuffer The framebuffer to be used for the destination. Retains a reference to
 *        the framebuffer.
 * @param Index The framebuffer attachment to write to.
 * @param Region The region to write to.
 */
void GFXBlitSetDestination(GFXBlit Blit, GFXFramebuffer CC_RETAIN(Framebuffer), size_t Index, CCRect Region);

/*!
 * @brief Set the texture filtering to be used for the blit command if the regions are of
 *        differing sizes.
 *
 * @param Blit The blit operation.
 * @param FilterMode The filter type.
 *        @b GFXTextureHintFilterModeNearest
 *        @b GFXTextureHintFilterModeLinear
 */
void GFXBlitSetFilterMode(GFXBlit Blit, GFXTextureHint FilterMode);

#endif
