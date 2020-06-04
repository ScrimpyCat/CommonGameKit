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

#define CC_QUICK_COMPILE
#include "Colour.h"


static CCColourComponent CCColourRGBGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision);
static CCColourComponent CCColourYUVGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision);
static CCColourComponent CCColourHSGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision);

static size_t CCPackComponentsIntoBuffer(CCColourComponent *Components, size_t ComponentCount, void *Data)
{
    _Static_assert((CCColourFormatChannelBitSizeMask >> CCColourFormatChannelBitSize) <= (sizeof(uint64_t) * 8), "Exceeds limit of packed data");
    
    int ChunkIndex = 0, ChunkSize = 0;
    uint64_t Chunk = 0;
    
    for (int Loop = 0; Loop < ComponentCount && Components[Loop].type; Loop++)
    {
        const unsigned int Bits = CCColourComponentGetBitSize(Components[Loop]);
        
        if ((ChunkSize + Bits) > (sizeof(Chunk) * 8))
        {
            const int Remaining = (ChunkSize + Bits) - (sizeof(Chunk) * 8);
            const int Fit = Bits - Remaining;
            
            Chunk |= (Components[Loop].u64 & CCBitSet(Fit)) << ChunkSize;
            ((typeof(Chunk)*)Data)[ChunkIndex++] = Chunk;
            
            Chunk = (Components[Loop].u64 >> Fit) & CCBitSet(Remaining);
            ChunkSize = Remaining;
        }
        
        else
        {
            Chunk |= (Components[Loop].u64 & CCBitSet(Bits)) << ChunkSize;
            ChunkSize += Bits;
        }
    }
    
    const int Count = (ChunkSize + 7) / 8;
    for (int Loop = 0, Loop2 = ChunkIndex * sizeof(Chunk); Loop < Count; Loop++, Loop2++)
    {
        ((uint8_t*)Data)[Loop2] = ((uint8_t*)&Chunk)[Loop];
    }
    
    return (ChunkIndex * sizeof(Chunk)) + Count;
}

size_t CCColourPackIntoBuffer(CCColour Colour, void *Data)
{
    CCAssertLog((Colour.type & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only supports colour formats with 4 channel configuration");
    return CCPackComponentsIntoBuffer(Colour.channel, 4, Data);
}

size_t CCColourPackIntoBufferInPlanar(CCColour Colour, CCColourFormat PlanarIndex, void *Data)
{
    CCAssertLog((Colour.type & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only supports colour formats with 4 channel configuration");
    _Static_assert((CCColourFormatChannelBitSizeMask >> CCColourFormatChannelBitSize) <= (sizeof(uint64_t) * 8), "Exceeds limit of packed data");
    
    CCColourComponent Channels[4];
    size_t Count = CCColourGetChannelsInPlanar(Colour, PlanarIndex, Channels);
    
    return CCPackComponentsIntoBuffer(Channels, Count, Data);
}

CCColour CCColourUnpackFromBuffer(CCColourFormat ColourFormat, const void *Data[4])
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only supports colour formats with 4 channel configuration");
    _Static_assert((CCColourFormatChannelBitSizeMask >> CCColourFormatChannelBitSize) <= (sizeof(uint64_t) * 8), "Exceeds limit of packed data");
    
    CCColour Colour = { .type = ColourFormat };
    for (int Loop = 0; Loop < 4; Loop++)
    {
        CCColourFormat Formats[4];
        size_t Count = CCColourFormatChannelsInPlanar(ColourFormat, CCColourFormatLiteralIndexToChannelPlanar(Loop), Formats);
        
        const void *Buffer = Data[Loop];
        unsigned int BitCount = 0;
        for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
        {
            const unsigned int Bits = (Formats[Loop2] & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize;
            if (Bits)
            {
                uint64_t Chunk = 0;
                const int Count = Bits / 8;
                for (int Loop3 = 0; Loop3 < Count; Loop3++)
                {
                    ((uint8_t*)&Chunk)[Loop3] = ((uint8_t*)Buffer)[Loop3];
                }
                
                Buffer += Count;
                
                const int ChunkSize = Count? (Count * 8) - BitCount : 0;
                const int Remaining = Bits - ChunkSize;
                
                Chunk >>= BitCount;
                if (Count) BitCount = 0;
                
                Chunk |= ((*(uint8_t*)Buffer >> BitCount) & CCBitSet(Remaining)) << (Bits - Remaining);
                
                Colour.channel[CCColourFormatChannelOffsetToLiteralIndex(CCColourFormatChannelOffsetForChannelIndex(ColourFormat, Formats[Loop2] & CCColourFormatChannelIndexMask))] = (CCColourComponent){
                    .type = Formats[Loop2],
                    .u64 = Chunk
                };
                
                BitCount += Remaining;
                
                if (BitCount >= 8)
                {
                    Buffer++;
                    BitCount -= 8;
                }
            }
        }
    }
    
    return Colour;
}

size_t CCColourGetChannelsInPlanar(CCColour Colour, CCColourFormat PlanarIndex, CCColourComponent Channels[4])
{
    CCColourFormat Formats[4];
    size_t Count = CCColourFormatChannelsInPlanar(Colour.type, PlanarIndex, Formats);
    
    for (size_t Loop = 0, Index = 0; Loop < 4 && Colour.channel[Loop].type; Loop++)
    {
        for (size_t Loop2 = 0; Loop2 < Count; Loop2++)
        {
            if ((Formats[Loop2] & CCColourFormatChannelIndexMask) == (Colour.channel[Loop].type & CCColourFormatChannelIndexMask))
            {
                Channels[Index++] = Colour.channel[Loop];
                break;
            }
        }
    }
    
    return Count;
}

size_t CCColourGetComponentChannelIndex(CCColour Colour, CCColourFormat Index)
{
    if (CCColourFormatHasChannel(Colour.type, Index))
    {
        for (size_t Loop = 0; Loop < 4 && Colour.channel[Loop].type; Loop++)
        {
            if ((Colour.channel[Loop].type & CCColourFormatChannelIndexMask) == Index)
            {
                return Loop;
            }
        }
    }
    
    return SIZE_MAX;
}

CCColourComponent CCColourGetComponent(CCColour Colour, CCColourFormat Index)
{
    const size_t ChannelIndex = CCColourGetComponentChannelIndex(Colour, Index);
    
    return ChannelIndex == SIZE_MAX ? (CCColourComponent){ .type = 0, .u64 = 0 } : Colour.channel[ChannelIndex];
}

CCColourComponent CCColourGetComponentWithPrecision(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision)
{
    static CCColourComponent (* const Getters[CCColourFormatModelMask >> 2])(CCColour, CCColourFormat, CCColourFormat, int) = {
        [CCColourFormatModelRGB >> 2] = CCColourRGBGetComponent,
        [CCColourFormatModelYUV >> 2] = CCColourYUVGetComponent,
        [CCColourFormatModelHS >> 2] = CCColourHSGetComponent
    };
    
    CCColourComponent (* const Getter)(CCColour, CCColourFormat, CCColourFormat, int) = Getters[(Colour.type & CCColourFormatModelMask) >> 2];
    
    return Getter? Getter(Colour, Index, Type, Precision) : (CCColourComponent){ .type = 0, .u64 = 0 };
}

#pragma mark - Component Precision Conversions

static CCColourComponent CCColourComponentRGBPrecisionConversion(CCColourComponent Component, CCColourFormat OldType, CCColourFormat NewType, int NewPrecision)
{
    CCAssertLog((OldType & CCColourFormatModelMask) == CCColourFormatModelRGB, "Must be a colour space within the RGB model");
    
    if ((OldType == NewType) && (((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize) == NewPrecision)) return Component;
    
    if (!CCColourFormatHasChannel(Component.type, CCColourFormatChannelAlpha))
    {
        if (((OldType & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_RGB) && ((NewType & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_sRGB))
        {
            CCColourFormat TempType = 0;
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset0);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset1);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset2);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset3);
            TempType = (TempType & NewType) | CCColourFormatTypeFloat | CCColourFormatNormalized | CCColourFormatSpaceRGB_RGB | CCColourFormatOptionChannel4;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset0;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset1;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset2;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset3;
            
            Component = CCColourComponentLinearPrecisionConversion(Component, OldType, TempType, NewPrecision);
            
            if (Component.f32 <= 0.0031308f) Component.f32 *= 12.92f;
            else Component.f32 = (1.055f * powf(Component.f32, 2.4f)) - 0.055f;
            
            OldType = TempType;
        }
        
        else if (((OldType & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_sRGB) && ((NewType & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_RGB))
        {
            CCColourFormat TempType = 0;
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset0);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset1);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset2);
            TempType |= ((CCColourFormat)(CCColourFormatChannelMask ^ CCColourFormatChannelBitSizeMask) << (CCColourFormat)CCColourFormatChannelOffset3);
            TempType = (TempType & NewType) | CCColourFormatTypeFloat | CCColourFormatNormalized | CCColourFormatSpaceRGB_sRGB | CCColourFormatOptionChannel4;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset0;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset1;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset2;
            TempType |= (CCColourFormat)(32 << CCColourFormatChannelBitSize) << (CCColourFormat)CCColourFormatChannelOffset3;
            
            Component = CCColourComponentLinearPrecisionConversion(Component, OldType, TempType, NewPrecision);
            
            if (Component.f32 <= 0.04045f) Component.f32 /= 12.92f;
            else Component.f32 = powf((Component.f32 + 0.055f) / 1.055f, 2.4f);
            
            OldType = TempType;
        }
    }
    
    return CCColourComponentLinearPrecisionConversion(Component, OldType, NewType, NewPrecision);
}

static CCColourComponent CCColourComponentYUVPrecisionConversion(CCColourComponent Component, CCColourFormat OldType, CCColourFormat NewType, int NewPrecision)
{
    CCAssertLog((OldType & CCColourFormatModelMask) == CCColourFormatModelYUV, "Must be a colour space within the YUV model");
    
    if ((OldType == NewType) && (((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize) == NewPrecision)) return Component;
    
    CCAssertLog((OldType & CCColourFormatSpaceMask) == CCColourFormatSpaceYUV_YpCbCr, "Only supports YpCbCr currently");
    
    return CCColourComponentLinearPrecisionConversion(Component, OldType, NewType, NewPrecision);
}

static CCColourComponent CCColourComponentHSPrecisionConversion(CCColourComponent Component, CCColourFormat OldType, CCColourFormat NewType, int NewPrecision)
{
    CCAssertLog((OldType & CCColourFormatModelMask) == CCColourFormatModelHS, "Must be a colour space within the HS model");
    
    if ((OldType == NewType) && (((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize) == NewPrecision)) return Component;
    
    return CCColourComponentLinearPrecisionConversion(Component, OldType, NewType, NewPrecision);
}

#pragma mark - Component Getters

static CCColourComponent CCColourRGBGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision)
{
    CCColourComponent Component = CCColourGetComponent(Colour, Index);
    if (Component.type)
    {
        Component = CCColourComponentRGBPrecisionConversion(Component, Colour.type, Type, Precision);
    }
    
    else if (Index == CCColourFormatChannelAlpha)
    {
        //default to opaque alpha
        Component = CCColourComponentRGBPrecisionConversion((CCColourComponent){ .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = 1.0f }, CCColourFormatTypeFloat | CCColourFormatNormalized, Type, Precision);
    }
    
    return Component;
}

static CCColourComponent CCColourYUVGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision)
{
    CCColourComponent Component = CCColourGetComponent(Colour, Index);
    if (Component.type)
    {
        Component = CCColourComponentYUVPrecisionConversion(Component, Colour.type, Type, Precision);
    }
    
    else if (Index == CCColourFormatChannelAlpha)
    {
        //default to opaque alpha
        Component = CCColourComponentRGBPrecisionConversion((CCColourComponent){ .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = 1.0f }, CCColourFormatTypeFloat | CCColourFormatNormalized, Type, Precision);
    }
    
    return Component;
}

static CCColourComponent CCColourHSGetComponent(CCColour Colour, CCColourFormat Index, CCColourFormat Type, int Precision)
{
    CCColourComponent Component = CCColourGetComponent(Colour, Index);
    if (Component.type)
    {
        Component = CCColourComponentHSPrecisionConversion(Component, Colour.type, Type, Precision);
    }
    
    else if (Index == CCColourFormatChannelAlpha)
    {
        //default to opaque alpha
        Component = CCColourComponentRGBPrecisionConversion((CCColourComponent){ .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = 1.0f }, CCColourFormatTypeFloat | CCColourFormatNormalized, Type, Precision);
    }
    
    return Component;
}

#pragma mark - Colour Conversions

static CCColour CCColourHSConvertToRGB(CCColour Colour, CCColourFormat ColourSpace)
{
#define CC_COLOUR_CREATE_RGB_32F(r, g, b) (CCColour){ \
    .type = (CCColourFormatSpaceRGB_RGB | CCColourFormatTypeFloat \
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   32, CCColourFormatChannelOffset0) \
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 32, CCColourFormatChannelOffset1) \
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  32, CCColourFormatChannelOffset2) \
        | (AlphaComponent << CCColourFormatChannelOffset3)) | CCColourFormatNormalized, \
    .channel = { \
        [0] = { .type = CCColourFormatChannelRed   | (32 << CCColourFormatChannelBitSize), .f32 = r }, \
        [1] = { .type = CCColourFormatChannelGreen | (32 << CCColourFormatChannelBitSize), .f32 = g }, \
        [2] = { .type = CCColourFormatChannelBlue  | (32 << CCColourFormatChannelBitSize), .f32 = b }, \
        [3] = { .type = CCColourFormatChannelAlpha | (32 << CCColourFormatChannelBitSize), .f32 = a.f32 } \
    } \
}
    
    CCAssertLog((Colour.type & CCColourFormatModelMask) == CCColourFormatModelHS, "Must belong to the HS* model");
    
    CCColourComponent h = CCColourHSGetComponent(Colour, CCColourFormatChannelHue, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent s = CCColourHSGetComponent(Colour, CCColourFormatChannelSaturation, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent v = CCColourHSGetComponent(Colour, CCColourFormatChannelValue, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent a = CCColourHSGetComponent(Colour, CCColourFormatChannelAlpha, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    
    float C = 0.0f, m = 0.0f;
    switch (Colour.type & CCColourFormatSpaceMask)
    {
        case CCColourFormatSpaceHS_HSL:
            C = (1.0f - fabsf((2.0f * v.f32) - 1.0f)) * s.f32;
            m = v.f32 - (0.5f * C);
            break;
            
        case CCColourFormatSpaceHS_HSV:
            C = v.f32 * s.f32;
            m = v.f32 - C;
            break;
            
        case CCColourFormatSpaceHS_HSI:
            //TODO: add HSI
            CCAssertLog(0, "Conversion not currently supported");
            break;
            
        case CCColourFormatSpaceHS_HSluma:
            //TODO: add luma
            CCAssertLog(0, "Conversion not currently supported");
            break;
            
        default:
            return (CCColour){ .type = 0 };
    }
    
    const float H = h.f32 * 6.0f;
    const float X = H == 0.0f ? 0.0f : C * (1.0f - fabsf(fmodf(H, 2.0f) - 1.0f));
    
    const CCColourFormat AlphaComponent = !CCColourFormatHasChannel(ColourSpace, CCColourFormatChannelAlpha) ? 0 : (CCColourFormatChannelAlpha     | (32 << CCColourFormatChannelBitSize));
    
    switch ((int)floorf(H))
    {
        case 0: return CC_COLOUR_CREATE_RGB_32F(C + m, X + m, 0.0f + m);
        case 1: return CC_COLOUR_CREATE_RGB_32F(X + m, C + m, 0.0f + m);
        case 2: return CC_COLOUR_CREATE_RGB_32F(0.0f + m, C + m, X + m);
        case 3: return CC_COLOUR_CREATE_RGB_32F(0.0f + m, X + m, C + m);
        case 4: return CC_COLOUR_CREATE_RGB_32F(X + m, 0.0f + m, C + m);
        case 5: return CC_COLOUR_CREATE_RGB_32F(C + m, 0.0f + m, X + m);
    }
    
    return (CCColour){ .type = 0 };
}

static CCColour CCColourRGBConvertToHS(CCColour Colour, CCColourFormat ColourSpace)
{
    CCAssertLog((Colour.type & CCColourFormatModelMask) == CCColourFormatModelRGB, "Must belong to the RGB model");
    
    CCColourComponent r = CCColourRGBGetComponent(Colour, CCColourFormatChannelRed, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent g = CCColourRGBGetComponent(Colour, CCColourFormatChannelGreen, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent b = CCColourRGBGetComponent(Colour, CCColourFormatChannelBlue, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    CCColourComponent a = CCColourRGBGetComponent(Colour, CCColourFormatChannelAlpha, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    
    const float M = fmaxf(fmaxf(r.f32, g.f32), b.f32);
    const float m = fminf(fminf(r.f32, g.f32), b.f32);
    const float C = M - m;
    
    float H = 0.0f; //undefined if C == 0.0f
    if (M == r.f32) H = fmodf((g.f32 - b.f32) / C, 6.0f);
    else if (M == g.f32) H = ((b.f32 - r.f32) / C) + 2.0f;
    else if (M == b.f32) H = ((r.f32 - g.f32) / C) + 4.0f;
    
    H /= 6.0f;
    
    float S = 0.0f, V = 0.0f;
    switch (ColourSpace & CCColourFormatSpaceMask)
    {
        case CCColourFormatSpaceHS_HSL:
            V = 0.5f * (M + m);
            S = ((V == 0.0f) || (V == 1.0f)) ? 0.0f : C / (1.0f - fabsf(2.0f * V - 1.0f));
            break;
            
        case CCColourFormatSpaceHS_HSV:
            V = M;
            S = V == 0.0f ? 0.0f : C / V; //though wiki says: V / C
            break;
            
        case CCColourFormatSpaceHS_HSI:
            V = (r.f32 + g.f32 + b.f32) / 3.0f;
            S = V == 0.0f ? 0.0f : 1.0f - (m / V);
            break;
            
        case CCColourFormatSpaceHS_HSluma:
            //TODO: add luma
            CCAssertLog(0, "Conversion not currently supported");
            break;
            
        default:
            return (CCColour){ .type = 0 };
    }
    
    
    const CCColourFormat AlphaComponent = !CCColourFormatHasChannel(ColourSpace, CCColourFormatChannelAlpha) ? 0 : (CCColourFormatChannelAlpha     | (32 << CCColourFormatChannelBitSize));
    
    return (CCColour){
        .type = (ColourSpace & CCColourFormatSpaceMask) | CCColourFormatTypeFloat | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2)
        | (AlphaComponent << CCColourFormatChannelOffset3),
        .channel = {
            [0] = { .type = CCColourFormatChannelHue        | (32 << CCColourFormatChannelBitSize), .f32 = H < 0.0f ? H + 1.0f : H },
            [1] = { .type = CCColourFormatChannelSaturation | (32 << CCColourFormatChannelBitSize), .f32 = S },
            [2] = { .type = CCColourFormatChannelValue      | (32 << CCColourFormatChannelBitSize), .f32 = V },
            [3] = { .type = CCColourFormatChannelAlpha      | (32 << CCColourFormatChannelBitSize), .f32 = a.f32 }
        }
    };
}

CCColour CCColourConversion(CCColour Colour, CCColourFormat NewFormat)
{
    if (Colour.type == NewFormat) return Colour;
    
    static CCColour (* const Converters[CCColourFormatModelMask >> 2][CCColourFormatModelMask >> 2])(CCColour, CCColourFormat) = {
        [CCColourFormatModelRGB >> 2] = {
            [CCColourFormatModelHS >> 2] = CCColourRGBConvertToHS
        },
        [CCColourFormatModelHS >> 2] = {
            [CCColourFormatModelRGB >> 2] = CCColourHSConvertToRGB
        }
    };
    
    CCColour (* const Converter)(CCColour, CCColourFormat) = Converters[(Colour.type & CCColourFormatModelMask) >> 2][(NewFormat & CCColourFormatModelMask) >>  2];
    if (Converter) Colour = Converter(Colour, NewFormat);
    else if ((Colour.type & CCColourFormatModelMask) != (NewFormat & CCColourFormatModelMask)) return (CCColour){ .type = 0 };
    
    
    CCColour OldColour = Colour;
    for (int Loop = 0, Index = 0; Loop < 4; Loop++)
    {
        const CCColourFormat ChannelFormat = (NewFormat >> CCColourFormatLiteralIndexToChannelOffset(Loop)) & CCColourFormatChannelMask;
        const CCColourFormat ChannelType = ChannelFormat & CCColourFormatChannelIndexMask;
        const int Precision = (ChannelFormat & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize;
        if (Precision)
        {
            Colour.channel[Index++] = CCColourGetComponentWithPrecision(OldColour, ChannelType, NewFormat, Precision);
        }
    }
    
    Colour.type = NewFormat;
    
    return Colour;
}
