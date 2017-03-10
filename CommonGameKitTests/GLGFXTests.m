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
    
    CCVector3D Origin, Size;
    GFXTextureGetBounds(Sub2, &Origin, &Size);
    
    XCTAssertEqual(Origin.x, 0.12f, @"Should have correct origin");
    XCTAssertEqual(Origin.y, 0.0f, @"Should have correct origin");
    XCTAssertEqual(Origin.z, 0.0f, @"Should have correct origin");
    
    XCTAssertEqual(Size.x, 0.14f, @"Should have correct size");
    XCTAssertEqual(Size.y, 1.0f, @"Should have correct size");
    XCTAssertEqual(Size.z, 1.0f, @"Should have correct size");
    
    
    GFXTextureGetBounds(Sub, &Origin, &Size);
    
    XCTAssertEqual(Origin.x, 0.1f, @"Should have correct origin");
    XCTAssertEqual(Origin.y, 0.0f, @"Should have correct origin");
    XCTAssertEqual(Origin.z, 0.0f, @"Should have correct origin");
    
    XCTAssertEqual(Size.x, 0.15f, @"Should have correct size");
    XCTAssertEqual(Size.y, 1.0f, @"Should have correct size");
    XCTAssertEqual(Size.z, 1.0f, @"Should have correct size");
    
    
    GFXTextureGetBounds(Root, &Origin, &Size);
    
    XCTAssertEqual(Origin.x, 0.0f, @"Should have correct origin");
    XCTAssertEqual(Origin.y, 0.0f, @"Should have correct origin");
    XCTAssertEqual(Origin.z, 0.0f, @"Should have correct origin");
    
    XCTAssertEqual(Size.x, 1.0f, @"Should have correct size");
    XCTAssertEqual(Size.y, 1.0f, @"Should have correct size");
    XCTAssertEqual(Size.z, 1.0f, @"Should have correct size");
    
    GFXTextureDestroy(Sub2);
    GFXTextureDestroy(Sub);
    GFXTextureDestroy(Root);
}

@end
