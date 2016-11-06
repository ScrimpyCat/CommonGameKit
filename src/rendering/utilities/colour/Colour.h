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

#ifndef CommonGameKit_Colour_h
#define CommonGameKit_Colour_h

#include "ColourFormat.h"
#include "ColourComponent.h"
#include <CommonC/Common.h>

typedef struct {
    CCColourFormat type;
    CCColourComponent channel[4];
} CCColour;

#pragma mark -

/*!
 * @brief Packs the pixel colour into the buffer.
 * @discussion Combines all bit planes.
 * @param Colour The pixel colour to be packed into the buffer.
 * @param Data The buffer to store the colour.
 * @return The amount of bytes written to the buffer (bits rounded up to the next byte).
 */
size_t CCColourPackIntoBuffer(CCColour Colour, void *Data);

/*!
 * @brief Packs the pixel colour of a certain plane into the buffer.
 * @param Colour The pixel colour to be packed into the buffer.
 * @param PlanarIndex The plane to be used.
 *        @b CCColourFormatChannelPlanarIndex0
 *        @b CCColourFormatChannelPlanarIndex1
 *        @b CCColourFormatChannelPlanarIndex2
 *        @b CCColourFormatChannelPlanarIndex3
 *
 * @param Data The buffer to store the colour.
 * @return The amount of bytes written to the buffer (bits rounded up to the next byte).
 */
size_t CCColourPackIntoBufferInPlanar(CCColour Colour, CCColourFormat PlanarIndex, void *Data);

/*!
 * @brief Unpack the pixel colour from a buffer or multi-planar buffer.
 * @param ColourFormat The colour format of the data.
 * @param Data The multi-planar buffer to unpack the colour from.
 * @return The colour from the data.
 */
CCColour CCColourUnpackFromBuffer(CCColourFormat ColourFormat, const void *Data[4]);

/*!
 * @brief Retrieve the channels in a plane.
 * @param Colour The pixel colour to get the channels of.
 * @param PlanarIndex The plane to be used.
 *        @b CCColourFormatChannelPlanarIndex0
 *        @b CCColourFormatChannelPlanarIndex1
 *        @b CCColourFormatChannelPlanarIndex2
 *        @b CCColourFormatChannelPlanarIndex3
 *
 * @param Channels The channels on that plane.
 * @return The amount of channels found.
 */
size_t CCColourGetChannelsInPlanar(CCColour Colour, CCColourFormat PlanarIndex, CCColourComponent Channels[4]);

/*!
 * @brief Get the channel index for a given channel.
 * @discussion The index of Colour.channel[i].
 * @param Colour The pixel colour to get the channel index of.
 * @param Index The channel index.
 *        @b CCColourFormatChannelIndex0
 *        @b CCColourFormatChannelIndex1
 *        @b CCColourFormatChannelIndex2
 *        @b CCColourFormatChannelIndex3
 *
 * @return The channel index or SIZE_MAX on not found.
 */
size_t CCColourGetComponentChannelIndex(CCColour Colour, CCColourFormat Index);

/*!
 * @brief Get the colour component for a given channel.
 * @param Colour The pixel colour to get the colour component for.
 * @param Index The channel index.
 *        @b CCColourFormatChannelIndex0
 *        @b CCColourFormatChannelIndex1
 *        @b CCColourFormatChannelIndex2
 *        @b CCColourFormatChannelIndex3
 *
 * @return The colour component.
 */
CCColourComponent CCColourGetComponent(CCColour Colour, CCColourFormat Index);

/*!
 * @brief Get the colour component for a given channel in the specified type and precision.
 * @param Colour The pixel colour to get the colour component for.
 * @param Index The channel index.
 *        @b CCColourFormatChannelIndex0
 *        @b CCColourFormatChannelIndex1
 *        @b CCColourFormatChannelIndex2
 *        @b CCColourFormatChannelIndex3
 *
 * @param Type The new type of the component.
 *        @b CCColourFormatTypeUnsignedInteger
 *        @b CCColourFormatTypeSignedInteger
 *        @b CCColourFormatTypeFloat
 *
 * @param Precision The precision for the new type.
 * @return The converted colour component.
 */
CCColourComponent CCColourGetComponentWithPrecision(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision);

/*!
 * @brief Convert a colour to another format.
 * @param Colour The pixel colour to be converted.
 * @param NewFormat The new colour format.
 * @return The converted colour.
 */
CCColour CCColourConversion(CCColour Colour, CCColourFormat NewFormat);

static CC_FORCE_INLINE CCColour CCColourCreateFromRGB(CCColourRGB Colour);
static CC_FORCE_INLINE CCColour CCColourCreateFromRGBA(CCColourRGBA Colour);
static CC_FORCE_INLINE CCColourRGB CCColourToRGB(CCColour Colour);
static CC_FORCE_INLINE CCColourRGBA CCColourToRGBA(CCColour Colour);


#pragma mark -

static CC_FORCE_INLINE CCColour CCColourCreateFromRGB(CCColourRGB Colour)
{
    return (CCColour){
        .type = CCColourFormatRGB32Float,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (32 << CCColourFormatChannelBitSize), .f32 = Colour.r },
            [1] = { .type = CCColourFormatChannelGreen  | (32 << CCColourFormatChannelBitSize), .f32 = Colour.g },
            [2] = { .type = CCColourFormatChannelBlue   | (32 << CCColourFormatChannelBitSize), .f32 = Colour.b }
        }
    };
}

static CC_FORCE_INLINE CCColour CCColourCreateFromRGBA(CCColourRGBA Colour)
{
    return (CCColour){
        .type = CCColourFormatRGBA32Float,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (32 << CCColourFormatChannelBitSize), .f32 = Colour.r },
            [1] = { .type = CCColourFormatChannelGreen  | (32 << CCColourFormatChannelBitSize), .f32 = Colour.g },
            [2] = { .type = CCColourFormatChannelBlue   | (32 << CCColourFormatChannelBitSize), .f32 = Colour.b },
            [3] = { .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = Colour.a }
        }
    };
}

#endif
