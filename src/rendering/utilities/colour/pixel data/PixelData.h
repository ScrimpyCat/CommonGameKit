/*
 *  Copyright (c) 2015 Stefan Johnson
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

#ifndef Blob_Game_PixelData_h
#define Blob_Game_PixelData_h

#include "Colour.h"
#include <CommonC/Common.h>


/*!
 * @brief The pixel data container.
 * @description Allows @b CCRetain.
 */
typedef struct CCPixelDataInfo *CCPixelData;

typedef CC_NEW void *(*CCPixelDataConstructorCallback)(CCAllocatorType Allocator);
typedef void (*CCPixelDataDestructorCallback)(void *CC_DESTROY(Internal));
typedef CCColour (*CCPixelDataGetColourCallback)(CCPixelData Pixels, size_t x, size_t y, size_t z);
typedef void (*CCPixelDataGetSizeCallback)(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth);
typedef _Bool (*CCPixelDataGetPackedDataCallback)(CCPixelData Pixels, CCColourFormat Type, size_t Width, size_t Height, size_t Depth, void *Data);


typedef struct {
    CCPixelDataConstructorCallback create;
    CCPixelDataDestructorCallback destroy;
    CCPixelDataGetColourCallback colour;
    struct {
        CCPixelDataGetSizeCallback size;
        CCPixelDataGetPackedDataCallback packedData;
    } optional;
} CCPixelDataInterface;

typedef struct CCPixelDataInfo {
    const CCPixelDataInterface *interface;
    CCAllocatorType allocator;
    CCColourFormat format;
    void *internal;
} CCPixelDataInfo, *CCPixelData;


/*!
 * @brief Create a pixel data container.
 * @discussion Typically do not use this function directly, but instead a creator provided
 *             by an impementation.
 *
 * @param Allocator The allocator to be used.
 * @param Format The colour format of the data.
 * @param Interface The interface to the pixel data container implementation.
 * @return The pixel data container.
 */
CC_NEW CCPixelData CCPixelDataCreate(CCAllocatorType Allocator, CCColourFormat Format, const CCPixelDataInterface *Interface);

/*!
 * @brief Destroy a pixel data container.
 * @param Pixels The pixel data container to be destroyed.
 */
void CCPixelDataDestroy(CCPixelData CC_DESTROY(Pixels));

/*!
 * @brief Get the colour at a point in the pixel data.
 * @param Pixels The pixel data container to get the colour for.
 * @param x The x axis to sample.
 * @param y The y axis to sample.
 * @param z The z axis to sample.
 * @return The colour at that point.
 */
CCColour CCPixelDataGetColour(CCPixelData Pixels, size_t x, size_t y, size_t z);

/*!
 * @brief Check if pixel data has a fixed size.
 * @param Pixels The pixel data container.
 * @return Whether it has a fixed size or not.
 */
_Bool CCPixelDataHasSize(CCPixelData Pixels);

/*!
 * @brief Retrieve the size of the pixel data.
 * @param Pixels The pixel data container to get the size for.
 * @param Width The width for the pixel data, or NULL if no width is needed.
 * @param Height The height for the pixel data, or NULL if no height is needed.
 * @param Depth The depth for the pixel data, or NULL if no depth is needed.
 */
void CCPixelDataGetSize(CCPixelData Pixels, size_t *Width, size_t *Height, size_t *Depth);

/*!
 * @brief Get the packed data from the pixel data container.
 * @discussion Combines all bit planes.
 * @param Pixels The pixel data container to get the data from.
 * @param Width The width of the sample.
 * @param Height The height of the sample.
 * @param Depth The depth of the sample.
 * @param Data The data to store the pixel data.
 */
void CCPixelDataGetPackedData(CCPixelData Pixels, size_t Width, size_t Height, size_t Depth, void *Data);

/*!
 * @brief Get the packed data from the pixel data container.
 * @discussion Combines all bit planes.
 * @param Pixels The pixel data container to get the data from.
 * @param Type The colour format to store the pixel data as.
 * @param Width The width of the sample.
 * @param Height The height of the sample.
 * @param Depth The depth of the sample.
 * @param Data The data to store the pixel data.
 */
void CCPixelDataGetPackedDataWithFormat(CCPixelData Pixels, CCColourFormat Type, size_t Width, size_t Height, size_t Depth, void *Data);

/*!
 * @brief Check if the point is inside the bounds of the pixel data.
 * @param Pixels The pixel data container check the bounds of.
 * @param x The x axis to check.
 * @param y The y axis to check.
 * @param z The z axis to check.
 * @return Whether the point is valid or not.
 */
_Bool CCPixelDataInsideBounds(CCPixelData Pixels, size_t x, size_t y, size_t z);

/*!
 * @brief Get the plane count for the pixel data.
 * @param Pixels The pixel data container to get the plane count of.
 * @return The number of planes.
 */
unsigned int CCPixelDataGetPlaneCount(CCPixelData Pixels);

#endif
