/*
 *  Copyright (c) 2014 Stefan Johnson
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

#ifndef CommonGL_ColourFormat_h
#define CommonGL_ColourFormat_h

#include <CommonC/Extensions.h>
#include <stdint.h>


/*
 rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr rrrrrrrr ccccccxx ooxxxxpn sssmmmtt
 
 t: Type
 m: Model
 s: Space
 c: Compression
 r: Reserved options
 x: Unused/Reserved for future use
 
 n: Normalized
 p: Premultiplied Alpha
 o: Control option
 
 
 
 Options when using CCColourFormatOptionChannel4
 bbbbbbpp ccbbbbbb ppccbbbb bbppccbb bbbbppcc 00000000 00000000 00000000
 33333333 33222222 22221111 11111100 00000000 xxxxxxxx xxxxxxxx xxxxxxxx
 
 b: bit size (N << CCColourFormatChannelBitSize)
 p: planar index (CCColourFormatChannelPlanarIndexN)
 c: channel (CCColourFormatChannelIndexN)
 0: CCColourFormatChannelOffset0
 1: CCColourFormatChannelOffset1
 2: CCColourFormatChannelOffset2
 3: CCColourFormatChannelOffset3
 */

#define CC_COLOUR_FORMAT_CHANNEL_PLANAR(channel, size, offset, planar) (((CCColourFormat)(channel) | ((CCColourFormat)(size) << (CCColourFormat)CCColourFormatChannelBitSize) | (CCColourFormat)planar) << (CCColourFormat)(offset))
#define CC_COLOUR_FORMAT_CHANNEL(channel, size, offset) CC_COLOUR_FORMAT_CHANNEL_PLANAR(channel, size, offset, CCColourFormatChannelPlanarIndex0)


typedef uint64_t CCColourFormat;
enum {
    //Type
    CCColourFormatTypeMask = (3 << 0),
    CCColourFormatTypeUnsignedInteger = (0 << 0),
    CCColourFormatTypeSignedInteger = (1 << 0),
    CCColourFormatTypeFloat = (2 << 0),
    
    //Colour Model
    CCColourFormatModelMask = (7 << 2),
    CCColourFormatModelRGB = (0 << 2), //whether the format is a RGB colour model (e.g. RGB, sRGB, BGR)
    CCColourFormatModelYUV = (1 << 2), //whether the format is a YUV colour model (e.g. Y'CbCr)
    CCColourFormatModelHS = (2 << 2), //whether the format is a HSV/HSL colour model (e.g. HSL, HSV)
    CCColourFormatModelCMYK = (3 << 2), //whether the format is a CMYK colour model (e.g. CMY, CMYK)
    CCColourFormatModelCIE = (4 << 2), //whether the format is a CIE colour model (e.g. CIELAB, CIELUV)
    CCColourFormatModelMonochrome = (5 << 2),
    
    //Colour Space
    CCColourFormatSpaceMask = CCColourFormatModelMask | (7 << 5),
    //RGB Colour Spaces
    CCColourFormatSpaceRGB_RGB =    CCColourFormatModelRGB | (0 << 5),
    CCColourFormatSpaceRGB_sRGB =   CCColourFormatModelRGB | (1 << 5),
    //YUV Colour Spaces
    CCColourFormatSpaceYUV_YpCbCr = CCColourFormatModelYUV | (0 << 5),
    //HSL/HSV Colour Spaces
    CCColourFormatSpaceHS_HSL =    CCColourFormatModelHS | (0 << 5),
    CCColourFormatSpaceHS_HSV =    CCColourFormatModelHS | (1 << 5),
    CCColourFormatSpaceHS_HSB =    CCColourFormatSpaceHS_HSV,
    CCColourFormatSpaceHS_HSI =    CCColourFormatModelHS | (2 << 5), //component average
    CCColourFormatSpaceHS_HSluma = CCColourFormatModelHS | (3 << 5),
    //CMYK Colour Spaces
    //CIE Colour Spaces
    //Monochrome Colour Spaces
    
    //Compression
    CCColourFormatCompressionMask = (63 << 18),
    CCColourFormatCompressionTypeMask = (7 << 18),
    CCColourFormatCompressionOptionMask = (7 << 21),
    CCColourFormatCompressionNone = (0 << 18),
    CCColourFormatCompressionBPTC = (1 << 18),
    CCColourFormatCompressionS3TC = (2 << 18),
    CCColourFormatCompressionPVRTC = (3 << 18),
    //ASTC
    //ETC2
    //S3TC Compression Options
    CCColourFormatCompressionS3TC_DXT1 = CCColourFormatCompressionS3TC | (0 << 21),
    CCColourFormatCompressionS3TC_DXT2 = CCColourFormatCompressionS3TC | (1 << 21),
    CCColourFormatCompressionS3TC_DXT3 = CCColourFormatCompressionS3TC | (2 << 21),
    CCColourFormatCompressionS3TC_DXT4 = CCColourFormatCompressionS3TC | (3 << 21),
    CCColourFormatCompressionS3TC_DXT5 = CCColourFormatCompressionS3TC | (4 << 21),
    //PVRTC Compression Options
    CCColourFormatCompressionPVRTC_2BPP = CCColourFormatCompressionPVRTC | (0 << 21),
    CCColourFormatCompressionPVRTC_4BPP = CCColourFormatCompressionPVRTC | (1 << 21),
    
    //Options
    CCColourFormatNormalized = (1 << 8),
    CCColourFormatPremultipliedAlpha = (1 << 9),
    
    //Control Option
    CCColourFormatOptionMask = (3 << 14),
    CCColourFormatOptionChannel4 = (0 << 14), //whether it uses the 4 channel option/structure
    
    /*
     Four Channel Option Structure
     
     RGB565:
     ((CCColourFormatChannelRed     | (5 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset0) |
     ((CCColourFormatChannelGreen   | (6 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset1) |
     ((CCColourFormatChannelBlue    | (5 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset2)
     
     BGRA32:
     ((CCColourFormatChannelRed     | (8 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset2) |
     ((CCColourFormatChannelGreen   | (8 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset1) |
     ((CCColourFormatChannelBlue    | (8 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset0) |
     ((CCColourFormatChannelAlpha   | (8 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset3)
     
     R8:
     ((CCColourFormatChannelRed     | (5 << CCColourFormatChannelBitSize)) << CCColourFormatChannelOffset0)
     
     Y'CbCr 8-bit Bi-Planar (Y' 4-bit plane0, CbCr 4-bit(2:2) plane1):
     ((CCColourFormatChannelLuma    | (4 << CCColourFormatChannelBitSize) | CCColourFormatChannelPlanarIndex0) << CCColourFormatChannelOffset0) |
     ((CCColourFormatChannelChromaB | (2 << CCColourFormatChannelBitSize) | CCColourFormatChannelPlanarIndex1) << CCColourFormatChannelOffset1) | //is really Offset0
     ((CCColourFormatChannelChromaR | (2 << CCColourFormatChannelBitSize) | CCColourFormatChannelPlanarIndex1) << CCColourFormatChannelOffset2) | //is really Offset1
     
     
     How planar and offset work:
     Offset is for shifting the used component information into the format value. It doesn't correlate to offset location of the channel. Channel location works
     in increments based on the planar. So if planar 0 is used twice, it will have locations 0 and 1, if planar 1 is used once, it will have loations 0.
     */
    
    CCColourFormatChannelMask = 1023, //Must shift the offset back before using mask
    CCColourFormatChannelOffset0 = 24,
    CCColourFormatChannelOffset1 = 34,
    CCColourFormatChannelOffset2 = 44,
    CCColourFormatChannelOffset3 = 54,
    
    CCColourFormatChannelPlanarIndexMask = (3 << 2),
    CCColourFormatChannelPlanarIndex0 = (0 << 2),
    CCColourFormatChannelPlanarIndex1 = (1 << 2),
    CCColourFormatChannelPlanarIndex2 = (2 << 2),
    CCColourFormatChannelPlanarIndex3 = (3 << 2),
    
    CCColourFormatChannelBitSizeMask = (63 << 4),
    CCColourFormatChannelBitSize = 4,
    
    CCColourFormatChannelIndexMask = (3 << 0),
    CCColourFormatChannelIndex0 = (0 << 0),
    CCColourFormatChannelIndex1 = (1 << 0),
    CCColourFormatChannelIndex2 = (2 << 0),
    CCColourFormatChannelIndex3 = (3 << 0),
    
    CCColourFormatChannelAlpha = CCColourFormatChannelIndex3,
    
    //RGB Channels
    CCColourFormatChannelRed = CCColourFormatChannelIndex0,
    CCColourFormatChannelGreen = CCColourFormatChannelIndex1,
    CCColourFormatChannelBlue = CCColourFormatChannelIndex2,
    
    //YUV Channels
    CCColourFormatChannelLuma = CCColourFormatChannelIndex0,
    CCColourFormatChannelChromaU = CCColourFormatChannelIndex1,
    CCColourFormatChannelChromaB = CCColourFormatChannelChromaU,
    CCColourFormatChannelChromaV = CCColourFormatChannelIndex2,
    CCColourFormatChannelChromaR = CCColourFormatChannelChromaV,
    
    //HSB/HSV Channels
    CCColourFormatChannelHue = CCColourFormatChannelIndex0,
    CCColourFormatChannelSaturation = CCColourFormatChannelIndex1,
    CCColourFormatChannelValue = CCColourFormatChannelIndex2,
    CCColourFormatChannelLightness = CCColourFormatChannelValue,
};

#pragma mark - RGB Colour Formats
#include "ColourFormatRGB.h"

#pragma mark - YUV Colour Formats
//TODO: YUV formats

#pragma mark - HSL/HSV Colour Formats
//TODO: HSL/HSV formats

#pragma mark - CMYK Colour Formats
//TODO: CMYK formats

#pragma mark - CIE Colour Formats
//TODO: CIE formats

#pragma mark - Monochrome Colour Formats
//TODO: Monochrome formats

#pragma mark -


size_t CCColourFormatChannelsInPlanar(CCColourFormat ColourFormat, unsigned int PlanarIndex, CCColourFormat Channels[4]);
//_Bool CCColourFormatGLRepresentation(CCColourFormat ColourFormat, unsigned int PlanarIndex, GLenum *InputType, GLenum *InputFormat, GLenum *InternalFormat);
_Bool CCColourFormatHasChannel(CCColourFormat ColourFormat, CCColourFormat Index);
unsigned int CCColourFormatPlaneCount(CCColourFormat ColourFormat);
CCColourFormat CCColourFormatChannelOffsetForChannelIndex(CCColourFormat ColourFormat, CCColourFormat Index);
size_t CCColourFormatSampleSizeForPlanar(CCColourFormat ColourFormat, CCColourFormat PlanarIndex);

static CC_FORCE_INLINE size_t CCColourFormatChannelOffsetToLiteralIndex(CCColourFormat Index);
static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelOffset(size_t Index);
static CC_FORCE_INLINE size_t CCColourFormatChannelPlanarToLiteralIndex(CCColourFormat Index);
static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelPlanar(size_t Index);
static CC_FORCE_INLINE size_t CCColourFormatChannelIndexToLiteralIndex(CCColourFormat Index);
static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelIndex(size_t Index);

#pragma mark -

static CC_FORCE_INLINE size_t CCColourFormatChannelOffsetToLiteralIndex(CCColourFormat Index)
{
    _Static_assert(CCColourFormatChannelOffset0 == 24 &&
                   CCColourFormatChannelOffset1 == 34 &&
                   CCColourFormatChannelOffset2 == 44 &&
                   CCColourFormatChannelOffset3 == 54, "Expects values to equal these exactly");
    
    return (Index - 24) / 10;
}

static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelOffset(size_t Index)
{
    _Static_assert(CCColourFormatChannelOffset0 == 24 &&
                   CCColourFormatChannelOffset1 == 34 &&
                   CCColourFormatChannelOffset2 == 44 &&
                   CCColourFormatChannelOffset3 == 54, "Expects values to equal these exactly");
    
    return (Index * 10) + 24;
}

static CC_FORCE_INLINE size_t CCColourFormatChannelPlanarToLiteralIndex(CCColourFormat Index)
{
    _Static_assert(CCColourFormatChannelPlanarIndex0 == (0 << 2) &&
                   CCColourFormatChannelPlanarIndex1 == (1 << 2) &&
                   CCColourFormatChannelPlanarIndex2 == (2 << 2) &&
                   CCColourFormatChannelPlanarIndex3 == (3 << 2), "Expects values to equal these exactly");
    
    return Index >> 2;
}

static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelPlanar(size_t Index)
{
    _Static_assert(CCColourFormatChannelPlanarIndex0 == (0 << 2) &&
                   CCColourFormatChannelPlanarIndex1 == (1 << 2) &&
                   CCColourFormatChannelPlanarIndex2 == (2 << 2) &&
                   CCColourFormatChannelPlanarIndex3 == (3 << 2), "Expects values to equal these exactly");
    
    return Index << 2;
}

static CC_FORCE_INLINE size_t CCColourFormatChannelIndexToLiteralIndex(CCColourFormat Index)
{
    _Static_assert(CCColourFormatChannelIndex0 == (0 << 0) &&
                   CCColourFormatChannelIndex1 == (1 << 0) &&
                   CCColourFormatChannelIndex2 == (2 << 0) &&
                   CCColourFormatChannelIndex3 == (3 << 0), "Expects values to equal these exactly");
    
    return Index;
}

static CC_FORCE_INLINE CCColourFormat CCColourFormatLiteralIndexToChannelIndex(size_t Index)
{
    _Static_assert(CCColourFormatChannelIndex0 == (0 << 0) &&
                   CCColourFormatChannelIndex1 == (1 << 0) &&
                   CCColourFormatChannelIndex2 == (2 << 0) &&
                   CCColourFormatChannelIndex3 == (3 << 0), "Expects values to equal these exactly");
    
    return Index;
}

#endif
