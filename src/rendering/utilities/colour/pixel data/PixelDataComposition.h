/*
 *  Copyright (c) 2020 Stefan Johnson
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

#ifndef CommonGameKit_PixelDataComposition_h
#define CommonGameKit_PixelDataComposition_h

#include <CommonGameKit/PixelData.h>

extern const CCPixelDataInterface * const CCPixelDataComposition;

typedef struct {
    size_t x, y, z;
    size_t width, height, depth;
} CCPixelDataCompositionReferenceRegion;

typedef struct {
    /// The referenced pixel data
    struct {
        CCPixelData data;
        size_t x, y, z;
    } pixel;
    /// The region of the composite the pixel data is applied
    CCPixelDataCompositionReferenceRegion region;
    /// Whether the pixel data should be converted or reinterpreted to match the colour format of the composite
    _Bool reinterpret;
} CCPixelDataCompositionReference;

/*!
 * @brief Create a pixel data container for composite data.
 * @param Allocator The allocator to be used.
 * @param References The pixel data references by the composite. Note: Takes the container takes ownership of all pixel data
 *        references.
 *
 * @param Count The number of pixel data references in the composite.
 * @param Format The colour format of the data.
 * @param Width The width of the data.
 * @param Height The height of the data.
 * @param Depth The depth of the data.
 * @return The pixel data container.
 */
CC_NEW CCPixelData CCPixelDataCompositionCreate(CCAllocatorType Allocator, CCPixelDataCompositionReference *CC_OWN(References), size_t Count, CCColourFormat Format, size_t Width, size_t Height, size_t Depth);

#endif
