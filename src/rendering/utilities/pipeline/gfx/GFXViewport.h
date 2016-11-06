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

#ifndef CommonGameKit_GFXViewport_h
#define CommonGameKit_GFXViewport_h

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} GFXViewport;

/*!
 * @brief Create a 2D viewport.
 * @description Depth defaults to 0.0 - 1.0 range.
 * @param X The minimum x to clip from.
 * @param Y The minimum y to clip from.
 * @param Width The maximum width to clip to.
 * @param Height The maximum height to clip to.
 * @return The viewport.
 */
static inline GFXViewport GFXViewport2D(float X, float Y, float Width, float Height);

/*!
 * @brief Create a 3D viewport.
 * @param X The minimum x to clip from.
 * @param Y The minimum y to clip from.
 * @param Width The maximum width to clip to.
 * @param Height The maximum height to clip to.
 * @param MinDepth The minimum depth to clip from.
 * @param MaxDepth The maximum depth to clip to.
 * @return The viewport.
 */
static inline GFXViewport GFXViewport3D(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth);


#pragma mark -

static inline GFXViewport GFXViewport2D(float X, float Y, float Width, float Height)
{
    return GFXViewport3D(X, Y, Width, Height, 0.0f, 1.0f);
}

static inline GFXViewport GFXViewport3D(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth)
{
    return (GFXViewport){ .x = X, .y = Y, .width = Width, .height = Height, .minDepth = MinDepth, .maxDepth = MaxDepth };
}

#endif
