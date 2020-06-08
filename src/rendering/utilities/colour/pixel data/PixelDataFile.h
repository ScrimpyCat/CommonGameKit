/*
 *  Copyright (c) 2016 Stefan Johnson
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

#ifndef CommonGameKit_PixelDataFile_h
#define CommonGameKit_PixelDataFile_h

#include <CommonGameKit/PixelData.h>

extern const CCPixelDataInterface * const CCPixelDataFile;

/*!
 * @brief Create a pixel data container for a image file.
 * @description Supports PNG files.
 * @param Allocator The allocator to be used.
 * @param Path The path to the image file.
 * @return The pixel data container.
 */
CC_NEW CCPixelData CCPixelDataFileCreate(CCAllocatorType Allocator, FSPath CC_COPY(Path));

/*!
 * @brief Write a pixel data container to an image file.
 * @description Supports PNG files.
 * @param Pixel The pixel data container to create the PNG image file from.
 * @param x The x coordinate to begin the image.
 * @param y The y coordinate to begin the image.
 * @param z The z coordinate to begin the image.
 * @param Width The width of the image.
 * @param Height The height of the image.
 * @param Depth The depth of the image.
 * @param Path The path to the image file.
 */
void CCPixelDataFileWrite(CCPixelData Pixels, size_t x, size_t y, size_t z, size_t Width, size_t Height, size_t Depth, FSPath Path);

#endif
