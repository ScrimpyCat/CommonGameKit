/*
 *  Copyright (c) 2020, Stefan Johnson
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
#import "PixelDataComposite.h"
#import "PixelDataStatic.h"

@interface PixelDataCompositeTests : XCTestCase

@end

@implementation PixelDataCompositeTests

-(void) testConversion
{
    static uint8_t PixelData[6] = { //rgb1
        1, 4, 7, //red, blue, white
        2, 3, 0 //green, yellow, black
    };
    
    CCPixelData Base = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelData), PixelData, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   1, CCColourFormatChannelOffset0)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 1, CCColourFormatChannelOffset1)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  1, CCColourFormatChannelOffset2), 3, 2, 1);
    
    CCPixelData Pixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[1]){ CCPixelDataCompositeConvertedPixelData(Base, 0, 0, 0) }, 1, CCColourFormatRGB8Unorm, 3, 2, 1);
    
    CCColour Pixel = CCPixelDataGetColour(Pixels, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 255, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 255, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
    
    
    CCPixelData RePixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[1]){ CCPixelDataCompositeReinterpretedPixelData(CCRetain(Pixels), 0, 0, 0) }, 1, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                                      | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   16, CCColourFormatChannelOffset0)
                                                      | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1), 3, 2, 1);
    
    Pixel = CCPixelDataGetColour(RePixels, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xffff, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xff00, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xffff, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(RePixels, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
    
    CCPixelDataDestroy(RePixels);
    
    
    CCPixelData CoPixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[1]){ CCPixelDataCompositeConvertedPixelData(CCRetain(Pixels), 0, 0, 0) }, 1, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                                      | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   16, CCColourFormatChannelOffset0)
                                                      | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1), 3, 2, 1);
    
    Pixel = CCPixelDataGetColour(CoPixels, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xffff, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xffff, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0xffff, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u16, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(CoPixels, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
    
    CCPixelDataDestroy(CoPixels);
    
    CCPixelDataDestroy(Pixels);
}

-(void) testMultiPlanarConversion
{
    static uint8_t PixelDataR8[6] = {
        0xff, 0x00, 0xff, //red, blue, white
        0x00, 0xff, 0x00 //green, yellow, black
    };
    
    static uint16_t PixelDataGB8[6] = {
        0x0000, 0x00ff, 0xffff, //red, blue, white
        0xff00, 0xff00, 0x0000 //green, yellow, black
    };
    
    CCPixelData Base1 = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelDataR8), PixelDataR8, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                                | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   8, CCColourFormatChannelOffset0), 3, 2, 1);
    CCPixelData Base2 = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelDataGB8), PixelDataGB8, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                                | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset0)
                                                | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  8, CCColourFormatChannelOffset1), 3, 2, 1);
    
    CCPixelData Pixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[1]){ CCPixelDataCompositeConvertedMultiPlanarPixelData((CCPixelData[4]){ Base1, Base2, NULL, NULL }, 0, 0, 0) }, 1, CCColourFormatRGB8Unorm, 3, 2, 1);
    CCPixelData Ref = CCPixelDataStaticCreateWithMultiPlanar(CC_STD_ALLOCATOR, (CCData[4]){
        CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelDataR8), PixelDataR8, NULL, NULL),
        CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelDataGB8), PixelDataGB8, NULL, NULL),
        NULL,
        NULL
    }, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelRed,   8, CCColourFormatChannelOffset0, CCColourFormatChannelPlanarIndex0)
                                                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelGreen, 8, CCColourFormatChannelOffset1, CCColourFormatChannelPlanarIndex1)
                                                             | CC_COLOUR_FORMAT_CHANNEL_PLANAR(CCColourFormatChannelBlue,  8, CCColourFormatChannelOffset2, CCColourFormatChannelPlanarIndex1), 3, 2, 1);
    
    CCColour Pixel = CCPixelDataGetColour(Pixels, 0, 0, 0), RefPixel = CCPixelDataGetColour(Ref, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 0, 0);
    RefPixel = CCPixelDataGetColour(Ref, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 0, 0);
    RefPixel = CCPixelDataGetColour(Ref, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 1, 0);
    RefPixel = CCPixelDataGetColour(Ref, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 1, 0);
    RefPixel = CCPixelDataGetColour(Ref, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 1, 0);
    RefPixel = CCPixelDataGetColour(Ref, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelRed).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelGreen).u8, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, CCColourGetComponent(RefPixel, CCColourFormatChannelBlue).u8, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
    
    CCPixelDataDestroy(Pixels);
    CCPixelDataDestroy(Ref);
}

-(void) testRegions
{
    static uint8_t PixelData[6] = { //rgb1
        1, 4, 7, //red, blue, white
        2, 3, 0 //green, yellow, black
    };
    
    CCPixelData Base = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelData), PixelData, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   1, CCColourFormatChannelOffset0)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 1, CCColourFormatChannelOffset1)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  1, CCColourFormatChannelOffset2), 3, 2, 1);
    
    CCPixelData Pixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[2]){
        CCPixelDataCompositeConvertedSubPixelData(CCRetain(Base), 2, 0, 0, (CCPixelDataCompositeReferenceRegion){ .x = 0, .y = 0, .z = 0, .width = 1, .height = 2, .depth = 1 }),
        CCPixelDataCompositeConvertedPixelData(Base, 0, 0, 0)
    }, 2, CCColourFormatRGB8Unorm, 3, 2, 1);
    
    CCColour Pixel = CCPixelDataGetColour(Pixels, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 255, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 255, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(Pixels, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
    
    CCPixelDataDestroy(Pixels);
}

-(void) testPackedData
{
    static uint8_t PixelData[6] = { //rgb1
        1, 4, 7, //red, blue, white
        2, 3, 0 //green, yellow, black
    };
    
    CCPixelData Base = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelData), PixelData, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   1, CCColourFormatChannelOffset0)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 1, CCColourFormatChannelOffset1)
                                               | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  1, CCColourFormatChannelOffset2), 3, 2, 1);
    
    CCPixelData Pixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[2]){
        CCPixelDataCompositeConvertedSubPixelData(CCRetain(Base), 2, 0, 0, (CCPixelDataCompositeReferenceRegion){ .x = 0, .y = 0, .z = 0, .width = 1, .height = 2, .depth = 1 }),
        CCPixelDataCompositeConvertedPixelData(CCRetain(Base), 0, 0, 0)
    }, 2, Base->format, 3, 2, 1);
    
    uint8_t Data[6];
    CCPixelDataGetPackedData(Pixels, 0, 0, 0, 3, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 2, @"Should contain the correct value");
    XCTAssertEqual(Data[4], 3, @"Should contain the correct value");
    XCTAssertEqual(Data[5], 2, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 0, 0, 2, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 2, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 3, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 1, 0, 0, 2, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 3, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 2, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 1, 0, 2, 1, 1, Data);
    
    XCTAssertEqual(Data[0], 2, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 3, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 1, 0, 3, 1, 1, Data);
    
    XCTAssertEqual(Data[0], 2, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 3, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 2, @"Should contain the correct value");
    
    CCPixelDataDestroy(Pixels);
    
    
    Pixels = CCPixelDataCompositeCreate(CC_STD_ALLOCATOR, (CCPixelDataCompositeReference[2]){
        CCPixelDataCompositeReinterpretedSubPixelData(CCRetain(Base), 2, 0, 0, (CCPixelDataCompositeReferenceRegion){ .x = 0, .y = 0, .z = 0, .width = 1, .height = 2, .depth = 1 }),
        CCPixelDataCompositeConvertedPixelData(Base, 0, 0, 0)
    }, 2, CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   2, CCColourFormatChannelOffset0)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 1, CCColourFormatChannelOffset1), 3, 2, 1);
    
    CCPixelDataGetPackedData(Pixels, 0, 0, 0, 3, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 3, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 0, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[4], 7, @"Should contain the correct value");
    XCTAssertEqual(Data[5], 2, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 0, 0, 2, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 3, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 0, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 7, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 1, 0, 0, 2, 2, 1, Data);
    
    XCTAssertEqual(Data[0], 0, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 1, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 7, @"Should contain the correct value");
    XCTAssertEqual(Data[3], 2, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 1, 0, 2, 1, 1, Data);
    
    XCTAssertEqual(Data[0], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 7, @"Should contain the correct value");
    
    
    CCPixelDataGetPackedData(Pixels, 0, 1, 0, 3, 1, 1, Data);
    
    XCTAssertEqual(Data[0], 4, @"Should contain the correct value");
    XCTAssertEqual(Data[1], 7, @"Should contain the correct value");
    XCTAssertEqual(Data[2], 2, @"Should contain the correct value");
    
    CCPixelDataDestroy(Pixels);
}

@end
