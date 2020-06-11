/*
 *  Copyright (c) 2014, Stefan Johnson
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

@import Cocoa;
@import XCTest;
#import "ColourFormat.h"
#import "ColourComponent.h"
#import "Colour.h"

@interface ColourFormatTests : XCTestCase

@end

@implementation ColourFormatTests

-(void) setUp
{
    [super setUp];
}

-(void) tearDown
{
    [super tearDown];
}

-(void) testChannelsInPlanar
{
    CCColourFormat Channels[4];
    CCColourFormatChannelsInPlanar(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 8, CCColourFormatChannelOffset0)
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 8, CCColourFormatChannelOffset1),
                                   CCColourFormatChannelPlanarIndex0, Channels);
    
    XCTAssertEqual(Channels[0], (CCColourFormatChannelRed  | (8 << CCColourFormatChannelBitSize)), @"Should contain the red channel");
    XCTAssertEqual(Channels[1], (CCColourFormatChannelBlue | (8 << CCColourFormatChannelBitSize)), @"Should contain the blue channel");
    XCTAssertEqual(Channels[2], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[3], 0, @"Should not contain any element");
    
    
    
    CCColourFormatChannelsInPlanar(0, CCColourFormatChannelPlanarIndex0, Channels);
    
    XCTAssertEqual(Channels[0], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[1], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[2], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[3], 0, @"Should not contain any element");
    
    
    
    CCColourFormatChannelsInPlanar(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 8, CCColourFormatChannelOffset1)
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 8, CCColourFormatChannelOffset0),
                                   CCColourFormatChannelPlanarIndex0, Channels);
    
    XCTAssertEqual(Channels[1], (CCColourFormatChannelRed  | (8 << CCColourFormatChannelBitSize)), @"Should contain the red channel");
    XCTAssertEqual(Channels[0], (CCColourFormatChannelBlue | (8 << CCColourFormatChannelBitSize)), @"Should contain the blue channel");
    XCTAssertEqual(Channels[2], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[3], 0, @"Should not contain any element");
    
    
    
    CCColourFormatChannelsInPlanar(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 8, CCColourFormatChannelOffset0)
                                   | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1, CCColourFormatChannelPlanarIndex1)
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 8, CCColourFormatChannelOffset2),
                                   CCColourFormatChannelPlanarIndex0, Channels);
    
    XCTAssertEqual(Channels[0], (CCColourFormatChannelRed  | (8 << CCColourFormatChannelBitSize)), @"Should contain the red channel");
    XCTAssertEqual(Channels[1], (CCColourFormatChannelBlue | (8 << CCColourFormatChannelBitSize)), @"Should contain the blue channel");
    XCTAssertEqual(Channels[2], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[3], 0, @"Should not contain any element");
    
    
    
    CCColourFormatChannelsInPlanar(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 8, CCColourFormatChannelOffset0)
                                   | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1, CCColourFormatChannelPlanarIndex1)
                                   | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 8, CCColourFormatChannelOffset2),
                                   CCColourFormatChannelPlanarIndex1, Channels);
    
    XCTAssertEqual(Channels[0], (CCColourFormatChannelGreen | (8 << CCColourFormatChannelBitSize) | CCColourFormatChannelPlanarIndex1), @"Should contain the green channel");
    XCTAssertEqual(Channels[1], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[2], 0, @"Should not contain any element");
    XCTAssertEqual(Channels[3], 0, @"Should not contain any element");
}

-(void) testPackIntoBuffer
{
    uint8_t Data[64] = {0};
    CCColour Pixel = {
        .type = (CCColourFormat)CCColourFormatR5G6B5Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (5 << CCColourFormatChannelBitSize), .u8 = 0 },
            [1] = { .type = CCColourFormatChannelGreen  | (6 << CCColourFormatChannelBitSize), .u8 = 1 },
            [2] = { .type = CCColourFormatChannelBlue   | (5 << CCColourFormatChannelBitSize), .u8 = 1 },
            [3] = { .type = 0 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 2, @"Should only write out 2 bytes");
    XCTAssertEqual(*(uint16_t*)Data, 2080, @"Should contain the correct value");
    
    
    Pixel = (CCColour){
        .type = (CCColourFormat)CCColourFormatR32Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed | (32 << CCColourFormatChannelBitSize), .u32 = 0xdeadbeef },
            [1] = { .type = 0 },
            [2] = { .type = 0 },
            [3] = { .type = 0 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 4, @"Should only write out 4 bytes");
    XCTAssertEqual(*(uint32_t*)Data, 0xdeadbeef, @"Should contain the correct value");
    
    
    Pixel = (CCColour){
        .type = (CCColourFormat)CCColourFormatRGBA8Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (8 << CCColourFormatChannelBitSize), .u8 = 1 },
            [1] = { .type = CCColourFormatChannelGreen  | (8 << CCColourFormatChannelBitSize), .u8 = 2 },
            [2] = { .type = CCColourFormatChannelBlue   | (8 << CCColourFormatChannelBitSize), .u8 = 3 },
            [3] = { .type = CCColourFormatChannelAlpha  | (8 << CCColourFormatChannelBitSize), .u8 = 4 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 4, @"Should only write out 4 bytes");
    XCTAssertEqual(((uint8_t*)Data)[0], 1, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[1], 2, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[2], 3, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[3], 4, @"Should contain the correct value");
    
    
    Pixel = (CCColour){
        .type = (CCColourFormat)CCColourFormatRGBA32Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (32 << CCColourFormatChannelBitSize), .u32 = 0x10000002 },
            [1] = { .type = CCColourFormatChannelGreen  | (32 << CCColourFormatChannelBitSize), .u32 = 0x30000004 },
            [2] = { .type = CCColourFormatChannelBlue   | (32 << CCColourFormatChannelBitSize), .u32 = 0x50000006 },
            [3] = { .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .u32 = 0x70000008 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 16, @"Should only write out 16 bytes");
    XCTAssertEqual(((uint32_t*)Data)[0], 0x10000002, @"Should contain the correct value");
    XCTAssertEqual(((uint32_t*)Data)[1], 0x30000004, @"Should contain the correct value");
    XCTAssertEqual(((uint32_t*)Data)[2], 0x50000006, @"Should contain the correct value");
    XCTAssertEqual(((uint32_t*)Data)[3], 0x70000008, @"Should contain the correct value");
    
    
    Pixel = (CCColour){
        .type = CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
            | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 50, CCColourFormatChannelOffset0)
            | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 50, CCColourFormatChannelOffset1),
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (50 << CCColourFormatChannelBitSize), .u64 = 0x3000000000001 },
            [1] = { .type = CCColourFormatChannelBlue   | (50 << CCColourFormatChannelBitSize), .u64 = 0x2000000000003 },
            [2] = { .type = 0 },
            [3] = { .type = 0 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 13, @"Should only write out 13 bytes");
    XCTAssertEqual((((uint64_t*)Data)[0] & 0x3ffffffffffff), 0x3000000000001, @"Should contain the correct value");
    XCTAssertEqual(((((uint64_t*)Data)[0] & 0xfffc000000000000) >> 50) + ((((uint64_t*)Data)[1] & 0xfffffffff) << 14), 0x2000000000003, @"Should contain the correct value");
    
    
    Pixel = (CCColour){
        .type = CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
            | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelRed, 8, CCColourFormatChannelOffset0, CCColourFormatChannelPlanarIndex0)
            | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1, CCColourFormatChannelPlanarIndex1)
            | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelBlue, 8, CCColourFormatChannelOffset2, CCColourFormatChannelPlanarIndex0)
            | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelAlpha, 8, CCColourFormatChannelOffset3, CCColourFormatChannelPlanarIndex0),
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (8 << CCColourFormatChannelBitSize), .u8 = 1 },
            [1] = { .type = CCColourFormatChannelGreen  | (8 << CCColourFormatChannelBitSize), .u8 = 2 },
            [2] = { .type = CCColourFormatChannelBlue   | (8 << CCColourFormatChannelBitSize), .u8 = 3 },
            [3] = { .type = CCColourFormatChannelAlpha  | (8 << CCColourFormatChannelBitSize), .u8 = 4 }
        }
    };
    
    XCTAssertEqual(CCColourPackIntoBuffer(Pixel, Data), 4, @"Should only write out 4 bytes");
    XCTAssertEqual(((uint8_t*)Data)[0], 1, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[1], 2, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[2], 3, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[3], 4, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Pixel, CCColourFormatChannelPlanarIndex0, Data), 3, @"Should only write out 3 bytes");
    XCTAssertEqual(((uint8_t*)Data)[0], 1, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[1], 3, @"Should contain the correct value");
    XCTAssertEqual(((uint8_t*)Data)[2], 4, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Pixel, CCColourFormatChannelPlanarIndex1, Data), 1, @"Should only write out 1 byte");
    XCTAssertEqual(((uint8_t*)Data)[0], 2, @"Should contain the correct value");
}

-(void) testUnpackFromBuffer
{
    uint8_t Buffer1[128], Buffer2[128];
    CCColour Colour = CCColourUnpackFromBuffer(CCColourFormatR5G6B5Uint, (const void*[4]){
        &(uint16_t){ 2080 }, NULL, NULL, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 2, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint16_t*)Buffer1, 2080, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelGreen).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u8, 1, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelGreen), 1, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 2, @"Should be at the correct position");
    
    
    
    Colour = CCColourUnpackFromBuffer(CCColourFormatRGBA8Uint, (const void*[4]){
        &(uint32_t){ 0x04030201 }, NULL, NULL, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 4, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint32_t*)Buffer1, 0x04030201, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelGreen).u8, 2, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u8, 3, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelAlpha).u8, 4, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelGreen), 1, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 2, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelAlpha), 3, @"Should be at the correct position");
    
    
    
    Colour = CCColourUnpackFromBuffer(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 50, CCColourFormatChannelOffset0)
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 50, CCColourFormatChannelOffset1), (const void*[4]){
        (uint64_t[]){ 0xf000000000001, 0x800000000 }, NULL, NULL, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 13, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint64_t*)Buffer1, 0xf000000000001, @"Packed the correct values");
    XCTAssertEqual(((uint64_t*)Buffer1)[1] & 0xffffffffff, 0x800000000, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u64, 0x3000000000001, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u64, 0x2000000000003, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 1, @"Should be at the correct position");
    
    
    
    Colour = CCColourUnpackFromBuffer(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 12, CCColourFormatChannelOffset0)
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 12, CCColourFormatChannelOffset1), (const void*[4]){
        &(uint32_t){ 0x123456 }, NULL, NULL, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 3, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint32_t*)Buffer1 & 0xffffff, 0x123456, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u16, 0x456, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u16, 0x123, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 1, @"Should be at the correct position");
    
    
    
    Colour = CCColourUnpackFromBuffer(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 2, CCColourFormatChannelOffset0)
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 2, CCColourFormatChannelOffset2)
                             | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue, 2, CCColourFormatChannelOffset1), (const void*[4]){
        &(uint8_t){ 0x39 }, NULL, NULL, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 1, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint8_t*)Buffer1, 0x39, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelGreen).u8, 3, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u8, 2, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelGreen), 2, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 1, @"Should be at the correct position");
    
    
    
    Colour = CCColourUnpackFromBuffer(CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelRed, 2, CCColourFormatChannelOffset0, CCColourFormatChannelPlanarIndex0)
                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelGreen, 2, CCColourFormatChannelOffset2, CCColourFormatChannelPlanarIndex0)
                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelBlue, 2, CCColourFormatChannelOffset1, CCColourFormatChannelPlanarIndex2), (const void*[4]){
        &(uint8_t){ 0xd }, NULL, &(uint8_t){ 0x2 }, NULL
    });
    
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex0, Buffer1), 1, @"Packed the correct amount of bytes");
    XCTAssertEqual(CCColourPackIntoBufferInPlanar(Colour, CCColourFormatChannelPlanarIndex2, Buffer2), 1, @"Packed the correct amount of bytes");
    XCTAssertEqual(*(uint8_t*)Buffer1, 0xd, @"Packed the correct values");
    XCTAssertEqual(*(uint8_t*)Buffer2, 0x2, @"Packed the correct values");
    
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelGreen).u8, 3, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Colour, CCColourFormatChannelBlue).u8, 2, @"Should contain the correct value");
    
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelRed), 0, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelGreen), 2, @"Should be at the correct position");
    XCTAssertEqual(CCColourGetComponentChannelIndex(Colour, CCColourFormatChannelBlue), 1, @"Should be at the correct position");
}

-(void) testGetComponent
{
    CCColour Pixel = {
        .type = CCColourFormatR5G6B5Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (5 << CCColourFormatChannelBitSize), .u8 = 1 },
            [1] = { .type = CCColourFormatChannelGreen  | (6 << CCColourFormatChannelBitSize), .u8 = 2 },
            [2] = { .type = CCColourFormatChannelBlue   | (5 << CCColourFormatChannelBitSize), .u8 = 3 },
            [3] = { .type = 0 }
        }
    };
    
    CCColourComponent Component = CCColourGetComponent(Pixel, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 1, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(Pixel, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 2, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(Pixel, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 3, @"Should retrieve the correct component value");
}

-(void) testGetComponentWithPrecision
{
    CCColour Pixel = {
        .type = CCColourFormatR5G6B5Uint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (5 << CCColourFormatChannelBitSize), .u8 = 1 },
            [1] = { .type = CCColourFormatChannelGreen  | (6 << CCColourFormatChannelBitSize), .u8 = 63 },
            [2] = { .type = CCColourFormatChannelBlue   | (5 << CCColourFormatChannelBitSize), .u8 = 31 },
            [3] = { .type = 0 }
        }
    };
    
    CCColourComponent Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeUnsignedInteger, 1);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 1, @"Should change the type to the new type");
    XCTAssertEqual(Component.u8, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeUnsignedInteger, 1);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 1, @"Should change the type to the new type");
    XCTAssertEqual(Component.u8, 1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeUnsignedInteger, 6);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 6, @"Should change the type to the new type");
    XCTAssertEqual(Component.u8, 63, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 63.0f, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 1.0f, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, UINT_MAX, @"When no alpha channel is available should default to opaque");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 1.0f, @"When no alpha channel is available should default to opaque");
    
    
    Pixel = (CCColour){
        .type = CCColourFormatRGBA8Sint,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (8 << CCColourFormatChannelBitSize), .i8 = 0 },
            [1] = { .type = CCColourFormatChannelGreen  | (8 << CCColourFormatChannelBitSize), .i8 = -1 },
            [2] = { .type = CCColourFormatChannelBlue   | (8 << CCColourFormatChannelBitSize), .i8 = 1 },
            [3] = { .type = CCColourFormatChannelAlpha  | (8 << CCColourFormatChannelBitSize), .i8 = INT8_MIN }
        }
    };
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, -1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, 1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, INT8_MIN, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeSignedInteger, 16);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 16, @"Should change the type to the new type");
    XCTAssertEqual(Component.i16, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeSignedInteger, 16);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 16, @"Should change the type to the new type");
    XCTAssertEqual(Component.i16, -129, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeSignedInteger, 16);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 16, @"Should change the type to the new type");
    XCTAssertEqual(Component.i16, 385, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeSignedInteger, 16);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 16, @"Should change the type to the new type");
    XCTAssertEqual(Component.i16, INT16_MIN, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeSignedInteger, 7);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 7, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeSignedInteger, 7);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 7, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, -1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeSignedInteger, 7);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 7, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeSignedInteger, 7);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 7, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, -64, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 0.0f, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, -1.0f, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 1.0f, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, (float)INT8_MIN, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, -1.0f, @"Should convert value to the new representation");
    
    Pixel.channel[3].i8 = INT8_MAX;
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeFloat | CCColourFormatNormalized, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.f32, 1.0f, @"Should convert value to the new representation");
    
    Pixel = (CCColour){
        .type = CCColourFormatRGBA32Float,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (32 << CCColourFormatChannelBitSize), .f32 = 0.5f },
            [1] = { .type = CCColourFormatChannelGreen  | (32 << CCColourFormatChannelBitSize), .f32 = 1.0f },
            [2] = { .type = CCColourFormatChannelBlue   | (32 << CCColourFormatChannelBitSize), .f32 = 10.0f },
            [3] = { .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = -1.0f }
        }
    };
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, 1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, 10, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeSignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.i32, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeSignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.i32, 1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelBlue, CCColourFormatTypeSignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.i32, 10, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeSignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.i32, -1, @"Should convert value to the new representation");
    
    Pixel.type |= CCColourFormatNormalized;
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, (UINT32_MAX / 2) + 1, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, UINT32_MAX, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeUnsignedInteger, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 32, @"Should change the type to the new type");
    XCTAssertEqual(Component.u32, 0, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelRed, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, (INT8_MAX / 2), @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelGreen, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, INT8_MAX, @"Should convert value to the new representation");
    
    Component = CCColourGetComponentWithPrecision(Pixel, CCColourFormatChannelAlpha, CCColourFormatTypeSignedInteger, 8);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual((Component.type & CCColourFormatChannelBitSizeMask) >> CCColourFormatChannelBitSize, 8, @"Should change the type to the new type");
    XCTAssertEqual(Component.i8, INT8_MIN, @"Should convert value to the new representation");
}

-(void) testConversion
{
    CCColour Pixel = {
        .type = CCColourFormatRGBA32Float | CCColourFormatNormalized,
        .channel = {
            [0] = { .type = CCColourFormatChannelRed    | (32 << CCColourFormatChannelBitSize), .f32 = 1.0f },
            [1] = { .type = CCColourFormatChannelGreen  | (32 << CCColourFormatChannelBitSize), .f32 = 0.0f },
            [2] = { .type = CCColourFormatChannelBlue   | (32 << CCColourFormatChannelBitSize), .f32 = 0.0f },
            [3] = { .type = CCColourFormatChannelAlpha  | (32 << CCColourFormatChannelBitSize), .f32 = 0.25f }
        }
    };
    
    CCColour PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2));
    
    CCColourComponent Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelHue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 0.0f, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelSaturation);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelValue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelAlpha);
    XCTAssertNotEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should not retrieve the correct component");
    XCTAssertNotEqual(Component.f32, 1.0f, @"Should not retrieve the correct component value");
    
    Component = CCColourGetComponentWithPrecision(PixelHSV, CCColourFormatChannelAlpha, CCColourFormatTypeFloat, 32);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the correct component value");
    
    
    
    
    Pixel.channel[0].f32 = 0.5f;
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2));
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelHue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 0.0f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[0].type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should be in the correct position");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelSaturation);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[1].type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should be in the correct position");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelValue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 0.5f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[2].type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should be in the correct position");
    
    
    
    
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset2)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset0));
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelHue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 0.0f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[2].type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should be in the correct position");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelSaturation);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[1].type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should be in the correct position");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelValue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 0.5f, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelHSV.channel[0].type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should be in the correct position");
    
    
    
    
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        8, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 8, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      8, CCColourFormatChannelOffset2));
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelHue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelHue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 0, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelSaturation);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelSaturation, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, UINT8_MAX, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelHSV, CCColourFormatChannelValue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelValue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, UINT8_MAX / 2, @"Should retrieve the correct component value");
    
    
    
    
    CCColour PixelRGB = CCColourConversion(Pixel, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   8, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  8, CCColourFormatChannelOffset2));
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, UINT8_MAX / 2, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 0, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u8, 0, @"Should retrieve the correct component value");
    
    
    
    
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2));
    PixelRGB = CCColourConversion(PixelHSV, CCColourFormatRGBA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelAlpha);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, 1.0f, @"Should retrieve the default component value");
    
    
    
    
    Pixel.channel[0].f32 = 1.0f;
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2));
    PixelRGB = CCColourConversion(PixelHSV, CCColourFormatRGBA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    
    
    
    
    Pixel.channel[0].f32 = 1.0f;
    Pixel.channel[1].f32 = 0.5f;
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset0)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset1)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset2));
    PixelRGB = CCColourConversion(PixelHSV, CCColourFormatRGBA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    
    
    
    
    PixelHSV = CCColourConversion(Pixel, CCColourFormatSpaceHS_HSB | CCColourFormatTypeFloat | CCColourFormatNormalized
                                  | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelHue,        32, CCColourFormatChannelOffset2)
                                  | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelSaturation, 32, CCColourFormatChannelOffset0)
                                  | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelValue,      32, CCColourFormatChannelOffset1));
    PixelRGB = CCColourConversion(PixelHSV, CCColourFormatRGBA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[0].f32, Pixel.channel[0].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[1].f32, Pixel.channel[1].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[2].f32, Pixel.channel[2].f32, @"Should be correctly positioned");
    
    
    
    
    PixelRGB = CCColourConversion(Pixel, CCColourFormatRGBA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[0].f32, Pixel.channel[0].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[1].f32, Pixel.channel[1].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[2].f32, Pixel.channel[2].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelAlpha);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[3].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[3].f32, Pixel.channel[3].f32, @"Should be correctly positioned");
    
    
    
    
    PixelRGB = CCColourConversion(Pixel, CCColourFormatBGRA32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[2].f32, Pixel.channel[0].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[1].f32, Pixel.channel[1].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[0].f32, Pixel.channel[2].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelAlpha);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[3].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[3].f32, Pixel.channel[3].f32, @"Should be correctly positioned");
    
    
    
    PixelRGB = CCColourConversion(Pixel, CCColourFormatBGR32Float | CCColourFormatNormalized);
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[2].f32, Pixel.channel[0].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelGreen, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[1].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[1].f32, Pixel.channel[1].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelBlue, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[2].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[0].f32, Pixel.channel[2].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelAlpha);
    XCTAssertEqual(Component.type, 0, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u64, 0, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[3].type, 0, @"Should be unset");
    XCTAssertEqual(PixelRGB.channel[3].u64, 0, @"Should be unset");
    
    
    
    PixelRGB = CCColourConversion(Pixel, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeFloat | CCColourFormatNormalized
                                  | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed, 32, CCColourFormatChannelOffset1)
                                  | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelAlpha, 32, CCColourFormatChannelOffset0));
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelRed);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelRed, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[0].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[1].f32, Pixel.channel[0].f32, @"Should be correctly positioned");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelGreen);
    XCTAssertEqual(Component.type, 0, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u64, 0, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[2].type, 0, @"Should be unset");
    XCTAssertEqual(PixelRGB.channel[2].u64, 0, @"Should be unset");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelBlue);
    XCTAssertEqual(Component.type, 0, @"Should retrieve the correct component");
    XCTAssertEqual(Component.u64, 0, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[3].type, 0, @"Should be unset");
    XCTAssertEqual(PixelRGB.channel[3].u64, 0, @"Should be unset");
    
    Component = CCColourGetComponent(PixelRGB, CCColourFormatChannelAlpha);
    XCTAssertEqual(Component.type & CCColourFormatChannelIndexMask, CCColourFormatChannelAlpha, @"Should retrieve the correct component");
    XCTAssertEqual(Component.f32, Pixel.channel[3].f32, @"Should retrieve the correct component value");
    XCTAssertEqual(PixelRGB.channel[0].f32, Pixel.channel[3].f32, @"Should be correctly positioned");
}

@end
