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

#ifndef CommonGameKit_GFXTexture_h
#define CommonGameKit_GFXTexture_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/PixelData.h>

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
    GFXTextureHintAddress_R = 8,
    
    
    GFXTextureHintAccessMask = 3,
    GFXTextureHintAccessNone = 0,
    GFXTextureHintAccessOnce = 1,
    GFXTextureHintAccessMany = 2,
    GFXTextureHintCPURead = 10,   //00000000 0000xx00 00000000
    GFXTextureHintCPUWrite = 12, //00000000 00xx0000 00000000
    GFXTextureHintGPURead = 14,  //00000000 xx000000 00000000
    GFXTextureHintGPUWrite = 16, //000000xx 00000000 00000000
    /*
     Access hints are shifted by the operation type.
     e.g. (GFXTextureHintAccessOnce << GFXTextureHintCPUWrite) | (GFXTextureHintAccessMany << GFXTextureHintGPURead)
     Will specify a texture usage where the CPU will write to the texture once, and the GPU will read from
     the texture many times. And the CPU won't read from the texture, and the GPU won't write to the texture.
     */
    GFXTextureHintCPUReadMask = (GFXTextureHintAccessMask << GFXTextureHintCPURead),
    GFXTextureHintCPUReadNone = 0,
    GFXTextureHintCPUReadOnce = (GFXTextureHintAccessOnce << GFXTextureHintCPURead),
    GFXTextureHintCPUReadMany = (GFXTextureHintAccessMany << GFXTextureHintCPURead),
    
    GFXTextureHintCPUWriteMask = (GFXTextureHintAccessMask << GFXTextureHintCPUWrite),
    GFXTextureHintCPUWriteNone = 0,
    GFXTextureHintCPUWriteOnce = (GFXTextureHintAccessOnce << GFXTextureHintCPUWrite),
    GFXTextureHintCPUWriteMany = (GFXTextureHintAccessMany << GFXTextureHintCPUWrite),
    
    GFXTextureHintGPUReadMask = (GFXTextureHintAccessMask << GFXTextureHintGPURead),
    GFXTextureHintGPUReadNone = 0,
    GFXTextureHintGPUReadOnce = (GFXTextureHintAccessOnce << GFXTextureHintGPURead),
    GFXTextureHintGPUReadMany = (GFXTextureHintAccessMany << GFXTextureHintGPURead),
    
    GFXTextureHintGPUWriteMask = (GFXTextureHintAccessMask << GFXTextureHintGPUWrite),
    GFXTextureHintGPUWriteNone = 0,
    GFXTextureHintGPUWriteOnce = (GFXTextureHintAccessOnce << GFXTextureHintGPUWrite),
    GFXTextureHintGPUWriteMany = (GFXTextureHintAccessMany << GFXTextureHintGPUWrite),
    
    /// Equivalent to not passing any access options, an unknown access leaves the interpretation completely up to the implementation.
    GFXTextureHintAccessUnknown = GFXTextureHintCPUReadNone | GFXTextureHintCPUWriteNone | GFXTextureHintGPUReadNone | GFXTextureHintGPUWriteNone,
    
    
    GFXTextureHintUsageMask = (0x3ff << 18),
    GFXTextureHintUsageGeneral = 0,
    GFXTextureHintUsageShaderRead = (1 << 18),
    GFXTextureHintUsageShaderWrite = (1 << 19),
    GFXTextureHintUsageColourAttachmentRead = (1 << 20),
    GFXTextureHintUsageColourAttachmentWrite = (1 << 21),
    GFXTextureHintUsageDepthAttachmentRead = (1 << 22),
    GFXTextureHintUsageDepthAttachmentWrite = (1 << 23),
    GFXTextureHintUsageStencilAttachmentRead = (1 << 24),
    GFXTextureHintUsageStencilAttachmentWrite = (1 << 25),
    GFXTextureHintUsageTransferSource = (1 << 26),
    GFXTextureHintUsageTransferDestination = (1 << 27),
    GFXTextureHintUsageColourRenderTarget = GFXTextureHintUsageColourAttachmentRead | GFXTextureHintUsageColourAttachmentWrite,
    GFXTextureHintUsageDepthRenderTarget = GFXTextureHintUsageDepthAttachmentRead | GFXTextureHintUsageDepthAttachmentWrite,
    GFXTextureHintUsageStencilRenderTarget = GFXTextureHintUsageStencilAttachmentRead | GFXTextureHintUsageStencilAttachmentWrite,
    GFXTextureHintUsageRenderTarget = GFXTextureHintUsageColourRenderTarget
                                    | GFXTextureHintUsageDepthRenderTarget
                                    | GFXTextureHintUsageStencilRenderTarget
} GFXTextureHint;

/*!
 * @brief The graphics texture.
 * @description Allows @b CCRetain.
 */
typedef struct GFXTexture *GFXTexture;

/*!
 * @brief The graphics texture stream.
 * @description Allows @b CCRetain.
 */
typedef struct GFXTextureStream *GFXTextureStream;


#pragma mark - Texture Stream
/*!
 * @brief Create a texture stream.
 * @param Allocator The allocator to be used for the allocations.
 * @param Hint The required usage for the texture.
 * @param Format The colour format of the texture.
 * @param Width The width of the texture.
 * @param Height The height of the texture.
 * @param Depth The depth of the texture.
 * @return The created texture stream.
 */
CC_NEW GFXTextureStream GFXTextureStreamCreate(CCAllocatorType Allocator, GFXTextureHint Hint, CCColourFormat Format, size_t Width, size_t Height, size_t Depth);

/*!
 * @brief Destroy a texture stream.
 * @description Textures from the stream may remain in use/memory until they are destroyed too.
 * @param Stream The texture stream to be destroyed.
 */
void GFXTextureStreamDestroy(GFXTextureStream CC_DESTROY(Stream));

#pragma mark - Texture

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
 * @brief Create a sub texture reference.
 * @param Allocator The allocator to be used for the allocations.
 * @param Texture The root texture.
 * @param X The x coordinate in the root texture the sub is located.
 * @param Y The y coordinate in the root texture the sub is located.
 * @param Z The z coordinate in the root texture the sub is located.
 * @param Width The width of the texture.
 * @param Height The height of the texture.
 * @param Depth The depth of the texture.
 * @param Data The pixel data to be used for the texture. Note: Takes ownership of the pixel data.
 * @return The created texture.
 */
CC_NEW GFXTexture GFXTextureCreateSubTexture(CCAllocatorType Allocator, GFXTexture CC_RETAIN(Texture), size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData CC_OWN(Data));

/*!
 * @brief Create a texture from a stream.
 * @param Allocator The allocator to be used for the allocations.
 * @param Stream The texture stream.
 * @param Width The width of the texture.
 * @param Height The height of the texture.
 * @param Depth The depth of the texture.
 * @param Data The pixel data to be used for the texture. Note: Takes ownership of the pixel data.
 * @return The created texture, or NULL if the texture could not be created from the stream.
 */
CC_NEW GFXTexture GFXTextureCreateFromStream(CCAllocatorType Allocator, GFXTextureStream Stream, size_t Width, size_t Height, size_t Depth, CCPixelData CC_OWN(Data));

/*!
 * @brief Destroy a texture.
 * @param Texture The texture to be destroyed.
 */
void GFXTextureDestroy(GFXTexture CC_DESTROY(Texture));

/*!
 * @brief Get the parent texture.
 * @param Texture The texture to get the parent of.
 * @return The parent texture or NULL if the texture is already the root texture.
 */
GFXTexture GFXTextureGetParent(GFXTexture Texture);

/*!
 * @brief Get the hints for the texture.
 * @param Texture The texture to get the hints.
 * @return The hints for the texture.
 */
GFXTextureHint GFXTextureGetHints(GFXTexture Texture);

/*!
 * @brief Get the pixel data for the texture.
 * @param Texture The texture to get the pixel data of.
 * @return The pixel data for the texture. May be NULL if there is no current pixel data reference.
 */
CCPixelData GFXTextureGetData(GFXTexture Texture);

/*!
 * @brief Get the offsets of the texture.
 * @description The coordinates the sub texture was created at.
 * @param Texture The texture get the offsets for.
 * @param X The x coordinate of the texture. May be NULL if not needed.
 * @param Y The y coordinate of the texture. May be NULL if not needed.
 * @param Z The z coordinate of the texture. May be NULL if not needed.
 */
void GFXTextureGetOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z);

/*!
 * @brief Get the real offsets of the texture.
 * @description The coordinates of the texture in the root texture.
 * @param Texture The texture get the real offsets for.
 * @param X The real x coordinate of the texture. May be NULL if not needed.
 * @param Y The real y coordinate of the texture. May be NULL if not needed.
 * @param Z The real z coordinate of the texture. May be NULL if not needed.
 */
void GFXTextureGetInternalOffset(GFXTexture Texture, size_t *X, size_t *Y, size_t *Z);

/*!
 * @brief Get the dimensions of the texture.
 * @param Texture The texture get the dimension sizes for.
 * @param Width The width of the texture. May be NULL if not needed.
 * @param Height The height of the texture. May be NULL if not needed.
 * @param Depth The depth of the texture. May be NULL if not needed.
 */
void GFXTextureGetSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);

/*!
 * @brief Get the dimensions of the root texture/backing store.
 * @param Texture The texture get the internal sizes for.
 * @param Width The width of the texture. May be NULL if not needed.
 * @param Height The height of the texture. May be NULL if not needed.
 * @param Depth The depth of the texture. May be NULL if not needed.
 */
void GFXTextureGetInternalSize(GFXTexture Texture, size_t *Width, size_t *Height, size_t *Depth);

/*!
 * @brief Get the normalized coordinates of the texture.
 * @param Texture The texture get the coordinates for.
 * @param Bottom The bottom coords of the texture. May be NULL if not needed.
 * @param Top The top coords of the texture. May be NULL if not needed.
 */
void GFXTextureGetBounds(GFXTexture Texture, CCVector3D *Bottom, CCVector3D *Top);

/*!
 * @brief Get the normalization multiplier of a texture.
 * @description This multiplier can be used to convert a fixed unit into a normalized unit.
 * @param Texture The texture to get multiplier for.
 * @return The normalization multiplier.
 */
CCVector3D GFXTextureGetMultiplier(GFXTexture Texture);

/*!
 * @brief Normalize a coordinate in the texture.
 * @param Texture The texture to normalize the coordinate of.
 * @return The normalized coordinate.
 */
CCVector3D GFXTextureNormalizePoint(GFXTexture Texture, size_t X, size_t Y, size_t Z);

/*!
 * @brief Read the pixel data from the internal texture storage.
 * @description Does not set the texture's pixel data source reference.
 * @param Texture The texture to be read.
 * @param Allocator The allocator to be used for the pixel data.
 * @param Format The colour format to retrieve the pixel data as.
 * @param X The x coordinate to begin reading.
 * @param Y The y coordinate to begin reading.
 * @param Z The z coordinate to begin reading.
 * @param Width The width of the region to be read.
 * @param Height The height of the region to be read.
 * @param Depth The depth of the region to be read.
 * @return The pixel data.
 */
CC_NEW CCPixelData GFXTextureRead(GFXTexture Texture, CCAllocatorType Allocator, CCColourFormat Format, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth);

/*!
 * @brief Write the pixel data to the internal texture storage.
 * @param Texture The texture to be written to.
 * @param X The x coordinate to begin reading.
 * @param Y The y coordinate to begin reading.
 * @param Z The z coordinate to begin reading.
 * @param Width The width of the region to be read.
 * @param Height The height of the region to be read.
 * @param Depth The depth of the region to be read.
 * @param Data The pixel data to be copied from.
 */
void GFXTextureWrite(GFXTexture Texture, size_t X, size_t Y, size_t Z, size_t Width, size_t Height, size_t Depth, CCPixelData Data);

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
