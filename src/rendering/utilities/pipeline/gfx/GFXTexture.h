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

#ifndef Blob_Game_GFXTexture_h
#define Blob_Game_GFXTexture_h

#include <CommonC/Common.h>
#include "PixelData.h"

typedef enum {
    GFXTextureHintDimensionMask = 3,
    GFXTextureHintDimension1D = 0,
    GFXTextureHintDimension2D,
    GFXTextureHintDimension3D,
    
    GFXTextureHintAddressModeMask = 3,
    GFXTextureHintAddressModeClampToEdge = 0,
    GFXTextureHintAddressModeRepeat,
    GFXTextureHintAddressModeRepeatMirror,
    
    GFXTextureHintFilterModeMask = 1,
    GFXTextureHintFilterModeNearest = 0,
    GFXTextureHintFilterModeLinear,
    
    GFXTextureHintFilterMin = 2,
    GFXTextureHintFilterMag = 3,
    GFXTextureHintAddress_S = 4,
    GFXTextureHintAddress_T = 6,
    GFXTextureHintAddress_R = 8
} GFXTextureHint;

/*!
 * @brief The graphics texture.
 * @description Allows @b CCRetain.
 */
typedef struct GFXTexture *GFXTexture;


/*!
 * @brief Create a texture.
 * @param Allocator The allocator to be used for the allocations.
 * @param Hint The required usage for the texture.
 * @param Format The colour format of the texture.
 * @param Width The width of the texture.
 * @param Height The height of the texture.
 * @param Depth The depth of the texture.
 * @param Data The pixel data to be used for the texture. Note: Takes ownership of the pixel data.
 * @return The created texture.
 */
CC_NEW GFXTexture GFXTextureCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth, CCPixelData CC_OWN(Data));

/*!
 * @brief Destroy a texture.
 * @param Texture The texture to be destroyed.
 */
void GFXTextureDestroy(GFXTexture CC_DESTROY(Texture));

/*!
 * @brief Get the hints for the texture.
 * @param Texture The texture to get the hints.
 * @return The hints for the texture.
 */
GFXTextureHint GFXTextureGetHints(GFXTexture Texture);

/*!
 * @brief Get the dimensions of the texture.
 * @param Texture The texture get the dimension sizes for.
 * @param Width The width of the texture. May be NULL if not needed.
 * @param Height The height of the texture. May be NULL if not needed.
 * @param Depth The depth of the texture. May be NULL if not needed.
 */
void GFXTextureGetSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);

/*!
 * @brief Get the filter mode for the filter type of the texture.
 * @param Texture The texture get the filter mode for.
 * @param FilterType The filter type to get the filter mode for.
 *        @b GFXTextureHintFilterMin
 *        @b GFXTextureHintFilterMag
 *
 * @return The filter mode for that type.
 */
static inline GFXTextureHint GFXTextureGetFilterMode(GFXTexture Texture, GFXTextureHint FilterType);

/*!
 * @brief Set the filter mode for the filter type of the texture.
 * @param Texture The texture to set the filter mode of.
 * @param FilterType The filter type to set the filter mode of.
 *        @b GFXTextureHintFilterMin
 *        @b GFXTextureHintFilterMag
 *
 * @param FilterMode The filter mode for that type.
 *        @b GFXTextureHintFilterModeNearest
 *        @b GFXTextureHintFilterModeLinear
 *
 */
void GFXTextureSetFilterMode(GFXTexture Texture, GFXTextureHint FilterType, GFXTextureHint FilterMode);

/*!
 * @brief Get the address mode for the coordinate of the texture.
 * @param Texture The texture get the address mode for.
 * @param Coordinate The coordinate to get the address mode for.
 *        @b GFXTextureHintAddress_S
 *        @b GFXTextureHintAddress_T
 *        @b GFXTextureHintAddress_R
 *
 * @return The address mode for that coordinate.
 */
static inline GFXTextureHint GFXTextureGetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate);

/*!
 * @brief Set the address mode for the coordinate of the texture.
 * @param Texture The texture to set the address mode of.
 * @param Coordinate The coordinate to set the address mode of.
 *        @b GFXTextureHintAddress_S
 *        @b GFXTextureHintAddress_T
 *        @b GFXTextureHintAddress_R
 *
 * @param AddressMode The address mode for that coordinate.
 *        @b GFXTextureHintAddressModeClampToEdge
 *        @b GFXTextureHintAddressModeRepeat
 *        @b GFXTextureHintAddressModeRepeatMirror
 *
 */
void GFXTextureSetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate, GFXTextureHint AddressMode);

/*!
 * @brief Invalidate the texture, replacing it with a new texture.
 * @param Texture The texture to be invalidated.
 */
void GFXTextureInvalidate(GFXTexture Texture);



static inline GFXTextureHint GFXTextureGetFilterMode(GFXTexture Texture, GFXTextureHint FilterType)
{
    return (GFXTextureGetHints(Texture) >> FilterType) & GFXTextureHintFilterModeMask;
}

static inline GFXTextureHint GFXTextureGetAddressMode(GFXTexture Texture, GFXTextureHint Coordinate)
{
    return (GFXTextureGetHints(Texture) >> Coordinate) & GFXTextureHintAddressModeMask;
}

#endif
