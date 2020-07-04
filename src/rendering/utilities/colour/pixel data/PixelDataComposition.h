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

struct {
    /// The pixel data
    CCPixelData data;
    /// The referenced region
    CCPixelDataCompositionReferenceRegion region;
} CCPixelDataCompositionReferenceData;

typedef struct {
    /// The referenced pixel data
    struct {
        /// The pixel data
        CCPixelData data;
        /// The referenced region
        CCPixelDataCompositionReferenceRegion region;
    } pixel;
    /// The region of the composite the pixel data is applied
    size_t x, y, z;
    /// Whether the pixel data should be converted or reinterpreted to match the colour format of the composite
    _Bool reinterpret;
} CCPixelDataCompositionReference;

/*!
 * @brief Create a reference for converted pixel data in a composition.
 * @description The entire pixel data chunk is referenced, and explicit colour conversions
 *              are applied if the formats are different.
 *
 * @warning The pixel data must have a defined size. If it doesn't use @b CCPixelDataCompositionConvertedSubPixelData
 *          instead.
 *
 * @param Pixels The pixel data to be referenced. Ownership only applies if the
 *        reference is passed to a pixel data composition.
 *
 * @param X The x position in the composite that will reference this data.
 * @param Y The y position in the composite that will reference this data.
 * @param Z The z position in the composite that will reference this data.
 * @return The composition pixel data reference.
 */
static inline CCPixelDataCompositionReference CCPixelDataCompositionConvertedPixelData(CCPixelData CC_OWN(Pixels), size_t X, size_t Y, size_t Z);

/*!
 * @brief Create a reference for reinterpreted pixel data in a composition.
 * @description The entire pixel data chunk is referenced, and colour conversions
 *              are not applied if the formats are different, instead the data is
 *              reinterpreted as the new format.
 *
 * @warning The pixel data must have a defined size. If it doesn't use @b CCPixelDataCompositionReinterpretedSubPixelData
 *          instead.
 *
 * @warning The pixel data must have a format that has a binary compatible layout in order to support reinterpreting.
 *          e.g. @b CCColourFormatCompatibleBinaryLayout must return TRUE.
 *
 * @param Pixels The pixel data to be referenced. Ownership only applies if the
 *        reference is passed to a pixel data composition.
 *
 * @param X The x position in the composite that will reference this data.
 * @param Y The y position in the composite that will reference this data.
 * @param Z The z position in the composite that will reference this data.
 * @return The composition pixel data reference.
 */
static inline CCPixelDataCompositionReference CCPixelDataCompositionReinterpretedPixelData(CCPixelData CC_OWN(Pixels), size_t X, size_t Y, size_t Z);

/*!
 * @brief Create a reference for converted pixel data in a composition.
 * @description The entire pixel data chunk is referenced, and explicit colour conversions
 *              are applied if the formats are different.
 *
 * @param Pixels The pixel data to be referenced. Ownership only applies if the
 *        reference is passed to a pixel data composition.
 *
 * @param X The x position in the composite that will reference this data.
 * @param Y The y position in the composite that will reference this data.
 * @param Z The z position in the composite that will reference this data.
 * @param Width The width of the referenced data.
 * @param Height The height of the referenced data.
 * @param Depth The depth of the referenced data.
 * @return The composition pixel data reference.
 */
static inline CCPixelDataCompositionReference CCPixelDataCompositionConvertedSubPixelData(CCPixelData CC_OWN(Pixels), size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);

/*!
 * @brief Create a reference for reinterpreted pixel data in a composition.
 * @description The entire pixel data chunk is referenced, and colour conversions
 *              are not applied if the formats are different, instead the data is
 *              reinterpreted as the new format.
 *
 * @warning The pixel data must have a format that has a binary compatible layout in order to support reinterpreting.
 *          e.g. @b CCColourFormatCompatibleBinaryLayout must return TRUE.
 *
 * @param Pixels The pixel data to be referenced. Ownership only applies if the
 *        reference is passed to a pixel data composition.
 *
 * @param X The x position in the composite that will reference this data.
 * @param Y The y position in the composite that will reference this data.
 * @param Z The z position in the composite that will reference this data.
 * @param Width The width of the referenced data.
 * @param Height The height of the referenced data.
 * @param Depth The depth of the referenced data.
 * @return The composition pixel data reference.
 */
static inline CCPixelDataCompositionReference CCPixelDataCompositionReinterpretedSubPixelData(CCPixelData CC_OWN(Pixels), size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);

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

#pragma mark -

static inline CCPixelDataCompositionReference CCPixelDataCompositionConvertedPixelData(CCPixelData Pixels, size_t X, size_t Y, size_t Z)
{
    size_t Width, Height, Depth;
    CCPixelDataGetSize(Pixels, &Width, &Height, &Depth);
    
    return CCPixelDataCompositionConvertedSubPixelData(Pixels, X, Y, Z, Width, Height, Depth);
}

static inline CCPixelDataCompositionReference CCPixelDataCompositionReinterpretedPixelData(CCPixelData Pixels, size_t X, size_t Y, size_t Z)
{
    size_t Width, Height, Depth;
    CCPixelDataGetSize(Pixels, &Width, &Height, &Depth);
    
    return CCPixelDataCompositionReinterpretedSubPixelData(Pixels, X, Y, Z, Width, Height, Depth);
}

static inline CCPixelDataCompositionReference CCPixelDataCompositionConvertedSubPixelData(CCPixelData Pixels, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth)
{
    return (CCPixelDataCompositionReference){
        .pixel = {
            .data = Pixels,
            .region = {
                .x = 0, .y = 0, .z = 0,
                .width = Width, .height = Height, .depth = Depth
            }
        },
        .x = X,
        .y = Y,
        .z = Z,
        .reinterpret = FALSE
    };
}

static inline CCPixelDataCompositionReference CCPixelDataCompositionReinterpretedSubPixelData(CCPixelData Pixels, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth)
{
    CCPixelDataCompositionReference Reference = CCPixelDataCompositionConvertedSubPixelData(Pixels, X, Y, Z, Width, Height, Depth);
    
    Reference.reinterpret = TRUE;
    
    return Reference;
}

#endif
