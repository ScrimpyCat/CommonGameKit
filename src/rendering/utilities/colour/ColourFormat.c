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

#define CC_QUICK_COMPILE
#include "ColourFormat.h"
#include <CommonGameKit/Base.h>

//Retrieves channels for the specified plane, and restructures the channel offsets so they're ordered in the plane
size_t CCColourFormatChannelsInPlanar(CCColourFormat ColourFormat, CCColourFormat PlanarIndex, CCColourFormat Channels[4])
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only works on formats that use the channel 4 structure");
    
    memset(Channels, 0, sizeof(CCColourFormat) * 4);
    
    size_t Index = 0;
    for (int Loop = 0; Loop < 4; Loop++)
    {
        CCColourFormat Channel = (ColourFormat >> CCColourFormatLiteralIndexToChannelOffset(Loop)) & CCColourFormatChannelMask;
        if (((Channel & CCColourFormatChannelPlanarIndexMask) == PlanarIndex) && (Channel & CCColourFormatChannelBitSizeMask))
        {
            Channels[Index++] = Channel;
        }
    }
    
    return Index;
}

//_Bool CCColourFormatGLRepresentation(CCColourFormat ColourFormat, unsigned int PlanarIndex, GLenum *InputType, GLenum *InputFormat, GLenum *InternalFormat)
//{
//    if (InputType) *InputType = 0;
//    if (InputFormat) *InputFormat = 0;
//    if (InternalFormat) *InternalFormat = 0;
//    
//    if ((ColourFormat & CCColourFormatModelMask) == CCColourFormatModelRGB)
//    {
//        const _Bool sRGB = (ColourFormat & CCColourFormatSpaceMask) == CCColourFormatSpaceRGB_sRGB, Normalized = ColourFormat & CCColourFormatNormalized;
//#pragma unused(sRGB)
//#pragma unused(Normalized)
//        CCColourFormat Channels[4];
//        
//        CCColourFormatChannelsInPlanar(ColourFormat, PlanarIndex, Channels);
//        
//        int Channel1Size, Channel2Size, Channel3Size, Channel4Size;
//        if ((Channel1Size = (Channels[0] & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize))
//        {
//            /*
//             Table as according to GL docs, however spec says something different :/
//             Format:
//             Version    GL_RED    GL_GREEN     GL_BLUE    GL_ALPHA    GL_RG    GL_RGB    GL_BGR   GL_RGBA    GL_BGRA
//             GL 2       x         x            x          x                    x         x        x           x
//             GL 3       x                                             x        x         x        x           x
//             GL 4       x                                             x        x         x        x           x
//             ES 1                                         x                    x                  x
//             ES 2                                         x                    x                  x
//             ES 3       x                                 x           x        x                  x
//             
//             Version    GL_COLOR_INDEX    GL_LUMINANCE    GL_LUMINANCE_ALPHA    GL_STENCIL_INDEX     GL_DEPTH_COMPONENT    GL_DEPTH_STENCIL
//             GL 2       x                 x               x
//             GL 3
//             GL 4                                                               x                    x                     x
//             ES 1                         x               x
//             ES 2                         x               x
//             ES 3                         x               x                                          x                     x
//             
//             Version    GL_RED_INTEGER   GL_RG_INTEGER    GL_RGB_INTEGER   GL_BGR_INTEGER   GL_RGBA_INTEGER   GL_BGRA_INTEGER
//             GL 2
//             GL 3
//             GL 4       x                x                x                x                x                 x
//             ES 1
//             ES 2
//             ES 3       x                x                x                                 x
//             
//             
//             Type:
//             Version    GL_BITMAP    GL_UNSIGNED_BYTE    GL_BYTE      GL_UNSIGNED_SHORT    GL_SHORT    GL_UNSIGNED_INT      GL_INT
//             GL 2       x            x                   x            x                    x           x                    x
//             GL 3                    x                   x            x                    x           x                    x
//             GL 4                    x                   x            x                    x           x                    x
//             ES 1                    x
//             ES 2                    x
//             ES 3                    x                   x            x                    x           x                    x
//             
//             Version    GL_HALF_FLOAT    GL_FLOAT     GL_UNSIGNED_BYTE_3_3_2   GL_UNSIGNED_BYTE_2_3_3_REV
//             GL 2                        x            x                        x
//             GL 3                        x            x                        x
//             GL 4                        x            x                        x
//             ES 1
//             ES 2
//             ES 3       x                x
//             
//             Version    GL_UNSIGNED_SHORT_5_6_5      GL_UNSIGNED_SHORT_5_6_5_REV     GL_UNSIGNED_SHORT_4_4_4_4    GL_UNSIGNED_SHORT_4_4_4_4_REV
//             GL 2       x                            x                               x                            x
//             GL 3       x                            x                               x                            x
//             GL 4       x                            x                               x                            x
//             ES 1       x                                                            x
//             ES 2       x                                                            x
//             ES 3       x                                                            x
//             
//             Version    GL_UNSIGNED_SHORT_5_5_5_1    GL_UNSIGNED_SHORT_1_5_5_5_REV    GL_UNSIGNED_INT_8_8_8_8     GL_UNSIGNED_INT_8_8_8_8_REV
//             GL 2       x                            x                                x                           x
//             GL 3       x                            x                                x                           x
//             GL 4       x                            x                                x                           x
//             ES 1       x
//             ES 2       x
//             ES 3       x
//             
//             Version    GL_UNSIGNED_INT_10_10_10_2    GL_UNSIGNED_INT_2_10_10_10_REV    GL_UNSIGNED_INT_10F_11F_11F_REV
//             GL 2       x                             x
//             GL 3       x                             x
//             GL 4       x                             x
//             ES 1
//             ES 2
//             ES 3                                     x                                 x
//             
//             Version    GL_UNSIGNED_INT_5_9_9_9_REV     GL_UNSIGNED_INT_24_8     GL_FLOAT_32_UNSIGNED_INT_24_8_REV
//             GL 2
//             GL 3
//             GL 4
//             ES 1
//             ES 2
//             ES 3       x                               x                        x
//             */
//            
//            if (!Channels[1]) //Single Channel
//            {
//                if ((Channel1Size == 8) || (Channel1Size == 16) || (Channel1Size == 32))
//                {
//                    
//                }
//                
//                if (InputFormat)
//                {
//                    switch (Channels[0] & CCColourFormatChannelIndexMask)
//                    {
//                        CC_GL_VERSION_ACTIVE(1_0, NA, 3_0, NA, case CCColourFormatChannelRed:
//                            CC_GL_VERSION_ACTIVE(4_0, NA, 3_0, NA,
//                                if (!Normalized) *InputFormat = GL_RED_INTEGER;
//                                else
//                            );
//                            *InputFormat = GL_RED;
//                            break;
//                        );
//                        
//                        CC_GL_VERSION_ACTIVE(1_0, 2_1, NA, NA, case CCColourFormatChannelGreen:
//                            *InputFormat = GL_GREEN;
//                            break;
//                        );
//                        
//                        CC_GL_VERSION_ACTIVE(1_0, 2_1, NA, NA, case CCColourFormatChannelBlue:
//                            *InputFormat = GL_BLUE;
//                            break;
//                        );
//                        
//                        CC_GL_VERSION_ACTIVE(1_0, 2_1, 1_0, NA, case CCColourFormatChannelAlpha:
//                            *InputFormat = GL_ALPHA;
//                            break;
//                        );
//                    }
//                }
//            }
//            
//            else
//            {
//                
//            }
//            
//            switch (Channels[0] & CCColourFormatChannelIndexMask)
//            {
//                case CCColourFormatChannelRed:
//                    if (Channels[1])
//                    break;
//                    
//                case CCColourFormatChannelGreen:
//                    break;
//                    
//                case CCColourFormatChannelBlue:
//                    break;
//                    
//                case CCColourFormatChannelAlpha:
//                    break;
//            }
//        }
//    }
//    
//    return FALSE;
//}

_Bool CCColourFormatHasChannel(CCColourFormat ColourFormat, CCColourFormat Index)
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only works on formats that use the channel 4 structure");
    
    for (int Loop = 0; Loop < 4; Loop++)
    {
        CCColourFormat Channel = (ColourFormat >> CCColourFormatLiteralIndexToChannelOffset(Loop)) & CCColourFormatChannelMask;
        if ((Channel & CCColourFormatChannelBitSizeMask) && ((Channel & CCColourFormatChannelIndexMask) == Index)) return TRUE;
    }
    
    return FALSE;
}

unsigned int CCColourFormatPlaneCount(CCColourFormat ColourFormat)
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only works on formats that use the channel 4 structure");
    
    unsigned int Count = 0;
    _Bool Planes[4] = { FALSE, FALSE, FALSE, FALSE };
    
    for (int Loop = 0; Loop < 4; Loop++)
    {
        CCColourFormat Channel = (ColourFormat >> CCColourFormatLiteralIndexToChannelOffset(Loop)) & CCColourFormatChannelMask;
        if ((Channel & CCColourFormatChannelBitSizeMask) && (!Planes[(Channel & CCColourFormatChannelPlanarIndexMask) >> 2]))
        {
            Planes[(Channel & CCColourFormatChannelPlanarIndexMask) >> 2] = TRUE;
            Count++;
        }
    }
    
    return Count;
}

CCColourFormat CCColourFormatChannelOffsetForChannelIndex(CCColourFormat ColourFormat, CCColourFormat Index)
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only works on formats that use the channel 4 structure");
    
    for (int Loop = 0; Loop < 4; Loop++)
    {
        CCColourFormat Channel = (ColourFormat >> CCColourFormatLiteralIndexToChannelOffset(Loop)) & CCColourFormatChannelMask;
        if ((Channel & CCColourFormatChannelIndexMask) == Index) return CCColourFormatLiteralIndexToChannelOffset(Loop);
    }
    
    return 0;
}

size_t CCColourFormatSampleSizeForPlanar(CCColourFormat ColourFormat, CCColourFormat PlanarIndex)
{
    CCAssertLog((ColourFormat & CCColourFormatOptionMask) == CCColourFormatOptionChannel4, "Only supports colour formats with 4 channel configuration");
    
    CCColourFormat Channels[4];
    size_t BitCount = 0;
    
    for (size_t Loop = 0, Count = CCColourFormatChannelsInPlanar(ColourFormat, PlanarIndex, Channels); Loop < Count; Loop++)
    {
        BitCount += (Channels[Loop] & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize;
    }
    
    return (BitCount + 7) / 8;
}
