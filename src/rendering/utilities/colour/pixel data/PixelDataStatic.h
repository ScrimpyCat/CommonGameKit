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

#ifndef CommonGameKit_PixelDataStatic_h
#define CommonGameKit_PixelDataStatic_h

#include <CommonGameKit/PixelData.h>

extern const CCPixelDataInterface * const CCPixelDataStatic;

/*!
 * @brief Create a pixel data container for static data.
 * @param Allocator The allocator to be used.
 * @param Data The data to be used. Note: Takes the container takes ownership of the data.
 * @param Format The colour format of the data.
 * @param Width The width of the data.
 * @param Height The height of the data.
 * @param Depth The depth of the data.
 * @return The pixel data container.
 */
CC_NEW CCPixelData CCPixelDataStaticCreate(CCAllocatorType Allocator, CCData CC_OWN(Data), CCColourFormat Format, size_t Width, size_t Height, size_t Depth);

/*!
 * @brief Create a pixel data container for multi-planar static data.
 * @param Allocator The allocator to be used.
 * @param Data The multi-planar data to be used. Note: Takes the container takes ownership of
 *        the data.
 *
 * @param Format The colour format of the data.
 * @param Width The width of the data.
 * @param Height The height of the data.
 * @param Depth The depth of the data.
 * @return The pixel data container.
 */
CC_NEW CCPixelData CCPixelDataStaticCreateWithMultiPlanar(CCAllocatorType Allocator, CCData CC_OWN(Data)[4], CCColourFormat Format, size_t Width, size_t Height, size_t Depth);

#endif
