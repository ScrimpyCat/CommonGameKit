/*
 *  Copyright (c) 2017, Stefan Johnson
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
@import OpenGL.GL3;
@import XCTest;

#import "GFX.h"
#import "GLGFX.h"
#import "GLSetup.h"

@interface GLGFXTests : XCTestCase

@end

@implementation GLGFXTests
{
    CGLContextObj ctx;
}

-(void) setUp
{
    [super setUp];
    
    CGLPixelFormatObj PixelFormat;
    CGLChoosePixelFormat((CGLPixelFormatAttribute[]){
        kCGLPFAAllRenderers,
        kCGLPFAStencilSize, 1,
        kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_GL4_Core,
        0
    }, &PixelFormat, &(GLint){0});
    
    CGLCreateContext(PixelFormat, NULL, &ctx);
    CGLDestroyPixelFormat(PixelFormat);
    
    CGLSetCurrentContext(ctx);
    
    GLGFXSetup();
}

-(void) tearDown
{
    CGLSetCurrentContext(NULL);
    
    CGLReleaseContext(ctx);
    CCGLStateDestroy(CCGLCurrentState);
    CCGLCurrentState = NULL;
    
    [super tearDown];
}

-(void) testTextureBounds
{
    GFXTexture Root = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 100, 1, 1, NULL);
    
    GFXTexture Sub = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Root, 10, 0, 0, 5, 1, 1, NULL);
    
    GFXTexture Sub2 = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Sub, 2, 0, 0, 2, 1, 1, NULL);
    
    CCVector3D Bottom, Top;
    GFXTextureGetBounds(Sub2, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.12f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 0.14f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    
    GFXTextureGetBounds(Sub, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.1f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 0.15f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    
    GFXTextureGetBounds(Root, &Bottom, &Top);
    
    XCTAssertEqual(Bottom.x, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Bottom.z, 0.0f, @"Should have correct coord");
    
    XCTAssertEqual(Top.x, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.y, 1.0f, @"Should have correct coord");
    XCTAssertEqual(Top.z, 1.0f, @"Should have correct coord");
    
    GFXTextureDestroy(Sub2);
    GFXTextureDestroy(Sub);
    GFXTextureDestroy(Root);
}

-(void) testTextureCoordNormalization
{
    GFXTexture Root = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 100, 1, 1, NULL);
    
    GFXTexture Sub = GFXTextureCreateSubTexture(CC_STD_ALLOCATOR, Root, 10, 0, 0, 5, 1, 1, NULL);
    
    GFXTexture Root2 = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 50, 1, 1, NULL);
    
    CCVector3D Multiplier = GFXTextureGetMultiplier(Root), Coord = GFXTextureNormalizePoint(Root, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.01f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.01f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    
    Multiplier = GFXTextureGetMultiplier(Sub);
    Coord = GFXTextureNormalizePoint(Sub, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.01f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.11f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    
    Multiplier = GFXTextureGetMultiplier(Root2);
    Coord = GFXTextureNormalizePoint(Root2, 1, 0, 0);
    
    XCTAssertEqual(Multiplier.x, 0.02f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.y, 1.0f, @"Should have correct multiplier");
    XCTAssertEqual(Multiplier.z, 1.0f, @"Should have correct multiplier");
    
    XCTAssertEqual(Coord.x, 0.02f, @"Should have correct coord");
    XCTAssertEqual(Coord.y, 0.0f, @"Should have correct coord");
    XCTAssertEqual(Coord.z, 0.0f, @"Should have correct coord");
    
    GFXTextureDestroy(Root2);
    GFXTextureDestroy(Sub);
    GFXTextureDestroy(Root);
}

-(void) testStreams
{
    GFXTextureStream Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 4, 1, 1);
    
    GFXTexture Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    GFXTexture Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 1, 1, NULL);
    GFXTexture Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    GFXTexture Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t x;
    GFXTextureGetOffset(Texture1, &x, NULL, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, NULL, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, NULL, NULL);
    XCTAssertEqual(x, 3, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, NULL, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, NULL, NULL);
    XCTAssertEqual(x, 2, "Should position the texture in the correct location in the stream");

    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D, CCColourFormatRGB8Unorm, 3, 2, 1);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 1, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t y;
    GFXTextureGetOffset(Texture1, &x, &y, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, &y, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, &y, NULL);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 1, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, &y, NULL);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, &y, NULL);
    XCTAssertEqual(x, 2, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension3D, CCColourFormatRGB8Unorm, 3, 2, 2);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 2, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 2, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 2, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    XCTAssertNotEqual(Texture1, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture3, NULL, "Should have enough room to create the texture");
    XCTAssertEqual(Texture4, NULL, "Should not have enough room to create the texture");
    
    size_t z;
    GFXTextureGetOffset(Texture1, &x, &y, &z);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture2, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture3, &x, &y, &z);
    XCTAssertEqual(x, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture2);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture4 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 2, 1, NULL);
    
    XCTAssertNotEqual(Texture2, NULL, "Should have enough room to create the texture");
    XCTAssertNotEqual(Texture4, NULL, "Should have enough room to create the texture");
    
    
    GFXTextureGetOffset(Texture2, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 0, "Should position the texture in the correct location in the stream");
    
    GFXTextureGetOffset(Texture4, &x, &y, &z);
    XCTAssertEqual(x, 1, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(y, 0, "Should position the texture in the correct location in the stream");
    XCTAssertEqual(z, 1, "Should position the texture in the correct location in the stream");
    
    
    GFXTextureDestroy(Texture4);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
    GFXTextureStreamDestroy(Stream);
    
    
    
    
    Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension1D, CCColourFormatRGB8Unorm, 4, 1, 1);
    
    Texture1 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    Texture2 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 2, 1, 1, NULL);
    Texture3 = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, 1, 1, 1, NULL);
    
    GFXTextureStreamDestroy(Stream);
    GFXTextureDestroy(Texture3);
    GFXTextureDestroy(Texture2);
    GFXTextureDestroy(Texture1);
}

@end
