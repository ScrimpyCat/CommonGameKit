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
#import "PixelDataFile.h"
#import "PixelDataGenerator.h"

@interface PixelDataFileTests : XCTestCase

@end

static CCColour ScaledColour(size_t x, size_t y, size_t z)
{
    return CCColourCreateFromRGBA((CCColourRGBA){ (float)x / 10.0f, (float)y / 10.0f, (float)z / 10.0f, fminf((float)(x + y + z) / 10.0f, 1.0f) });
}

@implementation PixelDataFileTests

-(void) testReadingWriting
{
    FSPath Path = FSPathCreate("gfx-test-dumps/test-rgb.png");
    
    FSManagerRemove(Path);
    CCPixelData Pixels = CCPixelDataGeneratorCreate(CC_STD_ALLOCATOR, ScaledColour, CCColourFormatRGB8Unorm);
    CCPixelDataFileWrite(Pixels, 0, 0, 0, 10, 10, 1, Path);
    
    XCTAssertTrue(FSManagerExists(Path), @"Image file exists");
    
    CCPixelData File = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
    XCTAssertEqual(Pixels->format, File->format, @"Compatible formats should be the same");
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            CCColour ColourA = CCPixelDataGetColour(Pixels, x, y, 0);
            CCColour ColourB = CCPixelDataGetColour(File, x, y, 0);
            
            XCTAssertEqual(ColourA.channel[0].u8, ColourB.channel[0].u8, @"Pixel data red channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[1].u8, ColourB.channel[1].u8, @"Pixel data green channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[2].u8, ColourB.channel[2].u8, @"Pixel data blue channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[3].u8, ColourB.channel[3].u8, @"Pixel data alpha channel should be the same (%zu, %zu)", x, y);
        }
    }
    CCPixelDataDestroy(File);
    CCPixelDataDestroy(Pixels);
    
    
    FSPathSetComponentAtIndex(Path, FSPathComponentCreate(FSPathComponentTypeFile, "test-rgba"), FSPathGetComponentCount(Path) - 2);
    FSManagerRemove(Path);
    Pixels = CCPixelDataGeneratorCreate(CC_STD_ALLOCATOR, ScaledColour, CCColourFormatRGBA8Unorm);
    CCPixelDataFileWrite(Pixels, 0, 0, 0, 10, 10, 1, Path);
    
    XCTAssertTrue(FSManagerExists(Path), @"Image file exists");
    
    File = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
    XCTAssertEqual(Pixels->format, File->format, @"Compatible formats should be the same");
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            CCColour ColourA = CCPixelDataGetColour(Pixels, x, y, 0);
            CCColour ColourB = CCPixelDataGetColour(File, x, y, 0);
            
            XCTAssertEqual(ColourA.channel[0].u8, ColourB.channel[0].u8, @"Pixel data red channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[1].u8, ColourB.channel[1].u8, @"Pixel data green channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[2].u8, ColourB.channel[2].u8, @"Pixel data blue channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[3].u8, ColourB.channel[3].u8, @"Pixel data alpha channel should be the same (%zu, %zu)", x, y);
        }
    }
    CCPixelDataDestroy(File);
    CCPixelDataDestroy(Pixels);
    
    
    FSPathSetComponentAtIndex(Path, FSPathComponentCreate(FSPathComponentTypeFile, "test-rgb-srgb"), FSPathGetComponentCount(Path) - 2);
    FSManagerRemove(Path);
    Pixels = CCPixelDataGeneratorCreate(CC_STD_ALLOCATOR, ScaledColour, CCColourFormatRGB8Unorm_sRGB);
    CCPixelDataFileWrite(Pixels, 0, 0, 0, 10, 10, 1, Path);
    
    XCTAssertTrue(FSManagerExists(Path), @"Image file exists");
    
    File = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
    XCTAssertEqual(Pixels->format, File->format, @"Compatible formats should be the same");
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            CCColour ColourA = CCPixelDataGetColour(Pixels, x, y, 0);
            CCColour ColourB = CCPixelDataGetColour(File, x, y, 0);
            
            XCTAssertEqual(ColourA.channel[0].u8, ColourB.channel[0].u8, @"Pixel data red channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[1].u8, ColourB.channel[1].u8, @"Pixel data green channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[2].u8, ColourB.channel[2].u8, @"Pixel data blue channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[3].u8, ColourB.channel[3].u8, @"Pixel data alpha channel should be the same (%zu, %zu)", x, y);
        }
    }
    CCPixelDataDestroy(File);
    CCPixelDataDestroy(Pixels);
    
    
    FSPathSetComponentAtIndex(Path, FSPathComponentCreate(FSPathComponentTypeFile, "test-rgba-srgb"), FSPathGetComponentCount(Path) - 2);
    FSManagerRemove(Path);
    Pixels = CCPixelDataGeneratorCreate(CC_STD_ALLOCATOR, ScaledColour, CCColourFormatRGBA8Unorm_sRGB);
    CCPixelDataFileWrite(Pixels, 0, 0, 0, 10, 10, 1, Path);
    
    XCTAssertTrue(FSManagerExists(Path), @"Image file exists");
    
    File = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
    XCTAssertEqual(Pixels->format, File->format, @"Compatible formats should be the same");
    for (size_t y = 0; y < 10; y++)
    {
        for (size_t x = 0; x < 10; x++)
        {
            CCColour ColourA = CCPixelDataGetColour(Pixels, x, y, 0);
            CCColour ColourB = CCPixelDataGetColour(File, x, y, 0);
            
            XCTAssertEqual(ColourA.channel[0].u8, ColourB.channel[0].u8, @"Pixel data red channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[1].u8, ColourB.channel[1].u8, @"Pixel data green channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[2].u8, ColourB.channel[2].u8, @"Pixel data blue channel should be the same (%zu, %zu)", x, y);
            XCTAssertEqual(ColourA.channel[3].u8, ColourB.channel[3].u8, @"Pixel data alpha channel should be the same (%zu, %zu)", x, y);
        }
    }
    CCPixelDataDestroy(File);
    CCPixelDataDestroy(Pixels);
    
    FSPathDestroy(Path);
}

@end
