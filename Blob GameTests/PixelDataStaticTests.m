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

@import Cocoa;
@import XCTest;
#import "PixelDataStatic.h"

@interface PixelDataStaticTests : XCTestCase

@end

@implementation PixelDataStaticTests
{
    CCPixelData pixelData;
}

-(void) setUp
{
    [super setUp];
    
    static uint8_t PixelData[6] = { //rgb1
        1, 4, 7, //red, blue, white
        2, 3, 0 //green, yellow, black
    };
    
    pixelData = CCPixelDataStaticCreate(CC_STD_ALLOCATOR, CCDataBufferCreate(CC_STD_ALLOCATOR, CCDataHintRead, sizeof(PixelData), PixelData, NULL, NULL), CCColourFormatSpaceRGB_RGB | CCColourFormatTypeUnsignedInteger
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelRed,   1, CCColourFormatChannelOffset0)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelGreen, 1, CCColourFormatChannelOffset1)
                                        | CC_COLOUR_FORMAT_CHANNEL(CCColourFormatChannelBlue,  1, CCColourFormatChannelOffset2), 3, 2, 1);
}

-(void) tearDown
{
    CCPixelDataDestroy(pixelData);
    
    [super tearDown];
}

-(void) testSampling
{
    CCColour Pixel = CCPixelDataGetColour(pixelData, 0, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 1, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 1, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 2, 0, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 1, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 0, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 1, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 1, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 2, 1, 0);
    
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelRed).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelGreen).u8, 0, @"Should contain the correct value");
    XCTAssertEqual(CCColourGetComponent(Pixel, CCColourFormatChannelBlue).u8, 0, @"Should contain the correct value");
    
    
    Pixel = CCPixelDataGetColour(pixelData, 0, 2, 0);
    
    XCTAssertEqual(Pixel.type, 0, @"Should be outside bounds");
}

@end
