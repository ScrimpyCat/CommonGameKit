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

#import "GLSetup.h"
#ifndef CC_OPENGL_MODERN
#warning "Will not run test. Must set the framework's GL version to modern to run this test."
#else

@import Cocoa;
@import OpenGL.GL3;
@import XCTest;

@interface StateModernTests : XCTestCase

@end

@implementation StateModernTests
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
    
    CCGLCurrentState = CCGLStateCreate();
}

-(void) tearDown
{
    CGLSetCurrentContext(NULL);
    
    CGLReleaseContext(ctx);
    CCGLStateDestroy(CCGLCurrentState);
    CCGLCurrentState = NULL;
    
    [super tearDown];
}

-(void) testBlendState
{
#if CC_GL_STATE_BLEND
    CCGLState *State = CCGLCurrentState;
    
    glBlendFunc(GL_ONE, GL_ONE); CC_GL_CHECK();
    glBlendEquation(GL_FUNC_ADD); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(State, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(State->blendFunc[Loop].rgb.src, GL_ONE, @"RGB blend func src should be GL_ONE");
        XCTAssertEqual(State->blendFunc[Loop].rgb.dst, GL_ONE, @"RGB blend func dst should be GL_ONE");
        XCTAssertEqual(State->blendFunc[Loop].alpha.src, GL_ONE, @"alpha blend func src should be GL_ONE");
        XCTAssertEqual(State->blendFunc[Loop].alpha.dst, GL_ONE, @"alpha blend func dst should be GL_ONE");
        
        XCTAssertEqual(State->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"RGB blend equation should be GL_FUNC_ADD");
        XCTAssertEqual(State->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"alpha blend equation should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(State->blendFunc.rgb.src, GL_ONE, @"RGB blend func src should be GL_ONE");
    XCTAssertEqual(State->blendFunc.rgb.dst, GL_ONE, @"RGB blend func dst should be GL_ONE");
    XCTAssertEqual(State->blendFunc.alpha.src, GL_ONE, @"alpha blend func src should be GL_ONE");
    XCTAssertEqual(State->blendFunc.alpha.dst, GL_ONE, @"alpha blend func dst should be GL_ONE");
    
    XCTAssertEqual(State->blendEquation.rgb.mode, GL_FUNC_ADD, @"RGB blend equation should be GL_FUNC_ADD");
    XCTAssertEqual(State->blendEquation.alpha.mode, GL_FUNC_ADD, @"alpha blend equation should be GL_FUNC_ADD");
#endif
    
    
    glBlendFunc(GL_ZERO, GL_ZERO); CC_GL_CHECK();
    glBlendEquation(GL_FUNC_SUBTRACT); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(State, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(State->blendFunc[Loop].rgb.src, GL_ZERO, @"RGB blend func src should be GL_ZERO");
        XCTAssertEqual(State->blendFunc[Loop].rgb.dst, GL_ZERO, @"RGB blend func dst should be GL_ZERO");
        XCTAssertEqual(State->blendFunc[Loop].alpha.src, GL_ZERO, @"alpha blend func src should be GL_ZERO");
        XCTAssertEqual(State->blendFunc[Loop].alpha.dst, GL_ZERO, @"alpha blend func dst should be GL_ZERO");
        
        XCTAssertEqual(State->blendEquation[Loop].rgb.mode, GL_FUNC_SUBTRACT, @"RGB blend equation should be GL_FUNC_SUBTRACT");
        XCTAssertEqual(State->blendEquation[Loop].alpha.mode, GL_FUNC_SUBTRACT, @"alpha blend equation should be GL_FUNC_SUBTRACT");
    }
#else
    XCTAssertEqual(State->blendFunc.rgb.src, GL_ZERO, @"RGB blend func src should be GL_ZERO");
    XCTAssertEqual(State->blendFunc.rgb.dst, GL_ZERO, @"RGB blend func dst should be GL_ZERO");
    XCTAssertEqual(State->blendFunc.alpha.src, GL_ZERO, @"alpha blend func src should be GL_ZERO");
    XCTAssertEqual(State->blendFunc.alpha.dst, GL_ZERO, @"alpha blend func dst should be GL_ZERO");
    
    XCTAssertEqual(State->blendEquation.rgb.mode, GL_FUNC_SUBTRACT, @"RGB blend equation should be GL_FUNC_SUBTRACT");
    XCTAssertEqual(State->blendEquation.alpha.mode, GL_FUNC_SUBTRACT, @"alpha blend equation should be GL_FUNC_SUBTRACT");
#endif
#endif
}

-(void) testBlendFuncMacro
{
#if CC_GL_STATE_BLEND
    glBlendFunc(GL_ZERO, GL_ZERO); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_BLEND_FUNC(GL_DST_COLOR, GL_SRC_COLOR);
    
    GLenum SrcRGB, DstRGB, SrcAlpha, DstAlpha;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();

    XCTAssertEqual(SrcRGB, GL_DST_COLOR, @"src should be GL_DST_COLOR");
    XCTAssertEqual(DstRGB, GL_SRC_COLOR, @"dst should be GL_SRC_COLOR");
    XCTAssertEqual(SrcAlpha, GL_DST_COLOR, @"src should be GL_DST_COLOR");
    XCTAssertEqual(DstAlpha, GL_SRC_COLOR, @"dst should be GL_SRC_COLOR");

#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
#endif
    
    
    
    CC_GL_BLEND_FUNC(GL_ZERO, GL_ONE);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_ZERO, @"src should be GL_ZERO");
    XCTAssertEqual(DstRGB, GL_ONE, @"dst should be GL_ONE");
    XCTAssertEqual(SrcAlpha, GL_ZERO, @"src should be GL_ZERO");
    XCTAssertEqual(DstAlpha, GL_ONE, @"dst should be GL_ONE");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_ZERO, @"src state should be GL_ZERO");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_ONE, @"dst state should be GL_ONE");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_ZERO, @"src state should be GL_ZERO");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_ONE, @"dst state should be GL_ONE");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_ZERO, @"src state should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_ONE, @"dst state should be GL_ONE");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_ZERO, @"src state should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_ONE, @"dst state should be GL_ONE");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst = GL_SRC_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst = GL_SRC_COLOR;
    }
#else
    CC_GL_CURRENT_STATE->blendFunc.rgb.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.rgb.dst = GL_SRC_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.dst = GL_SRC_COLOR;
#endif
    CC_GL_BLEND_FUNC(GL_DST_COLOR, GL_SRC_COLOR);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_ZERO, @"src should be the same");
    XCTAssertEqual(DstRGB, GL_ONE, @"dst should be the same");
    XCTAssertEqual(SrcAlpha, GL_ZERO, @"src should be the same");
    XCTAssertEqual(DstAlpha, GL_ONE, @"dst should be the same");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst = GL_SRC_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst = GL_ZERO;
    }
#else
    CC_GL_CURRENT_STATE->blendFunc.rgb.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.rgb.dst = GL_SRC_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.dst = GL_ZERO;
#endif
    CC_GL_BLEND_FUNC(GL_DST_COLOR, GL_SRC_COLOR);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_DST_COLOR, @"src should be changed");
    XCTAssertEqual(DstRGB, GL_SRC_COLOR, @"dst should be changed");
    XCTAssertEqual(SrcAlpha, GL_DST_COLOR, @"src should be changed");
    XCTAssertEqual(DstAlpha, GL_SRC_COLOR, @"dst should be changed");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
#endif
#endif
}

-(void) testBlendFuncSeparateMacro
{
#if CC_GL_STATE_BLEND
    glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ONE, GL_ONE); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_BLEND_FUNC_SEPARATE(GL_DST_COLOR, GL_SRC_COLOR, GL_DST_ALPHA, GL_SRC_ALPHA);
    
    GLenum SrcRGB, DstRGB, SrcAlpha, DstAlpha;
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_DST_COLOR, @"src should be GL_DST_COLOR");
    XCTAssertEqual(DstRGB, GL_SRC_COLOR, @"dst should be GL_SRC_COLOR");
    XCTAssertEqual(SrcAlpha, GL_DST_ALPHA, @"src should be GL_DST_ALPHA");
    XCTAssertEqual(DstAlpha, GL_SRC_ALPHA, @"dst should be GL_SRC_ALPHA");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
#endif
    
    
    
    CC_GL_BLEND_FUNC_SEPARATE(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_ZERO, @"src should be GL_ZERO");
    XCTAssertEqual(DstRGB, GL_ONE, @"dst should be GL_ONE");
    XCTAssertEqual(SrcAlpha, GL_ZERO, @"src should be GL_ZERO");
    XCTAssertEqual(DstAlpha, GL_ONE, @"dst should be GL_ONE");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_ZERO, @"src state should be GL_ZERO");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_ONE, @"dst state should be GL_ONE");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_ZERO, @"src state should be GL_ZERO");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_ONE, @"dst state should be GL_ONE");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_ZERO, @"src state should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_ONE, @"dst state should be GL_ONE");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_ZERO, @"src state should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_ONE, @"dst state should be GL_ONE");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src = GL_DST_ALPHA;
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst = GL_SRC_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst = GL_SRC_ALPHA;
    }
#else
    CC_GL_CURRENT_STATE->blendFunc.rgb.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.src = GL_DST_ALPHA;
    CC_GL_CURRENT_STATE->blendFunc.rgb.dst = GL_SRC_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.dst = GL_SRC_ALPHA;
#endif
    CC_GL_BLEND_FUNC_SEPARATE(GL_DST_COLOR, GL_SRC_COLOR, GL_DST_ALPHA, GL_SRC_ALPHA);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_ZERO, @"src should be the same");
    XCTAssertEqual(DstRGB, GL_ONE, @"dst should be the same");
    XCTAssertEqual(SrcAlpha, GL_ZERO, @"src should be the same");
    XCTAssertEqual(DstAlpha, GL_ONE, @"dst should be the same");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src = GL_DST_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src = GL_DST_ALPHA;
        CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst = GL_SRC_COLOR;
        CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst = GL_ZERO;
    }
#else
    CC_GL_CURRENT_STATE->blendFunc.rgb.src = GL_DST_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.src = GL_DST_ALPHA;
    CC_GL_CURRENT_STATE->blendFunc.rgb.dst = GL_SRC_COLOR;
    CC_GL_CURRENT_STATE->blendFunc.alpha.dst = GL_ZERO;
#endif
    CC_GL_BLEND_FUNC_SEPARATE(GL_DST_COLOR, GL_SRC_COLOR, GL_DST_ALPHA, GL_SRC_ALPHA);
    
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&SrcRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&SrcAlpha); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&DstRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&DstAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(SrcRGB, GL_DST_COLOR, @"src should be changed");
    XCTAssertEqual(DstRGB, GL_SRC_COLOR, @"dst should be changed");
    XCTAssertEqual(SrcAlpha, GL_DST_ALPHA, @"src should be changed");
    XCTAssertEqual(DstAlpha, GL_SRC_ALPHA, @"dst should be changed");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc[Loop].alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.src, GL_DST_COLOR, @"src state should be GL_DST_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.rgb.dst, GL_SRC_COLOR, @"dst state should be GL_SRC_COLOR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.src, GL_DST_ALPHA, @"src state should be GL_DST_ALPHA");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendFunc.alpha.dst, GL_SRC_ALPHA, @"dst state should be GL_SRC_ALPHA");
#endif
#endif
}

-(void) testBlendEquationMacro
{
#if CC_GL_STATE_BLEND
    glBlendEquation(GL_FUNC_SUBTRACT); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_BLEND_EQUATION(GL_FUNC_ADD); CC_GL_CHECK();
    
    GLenum EqRGB, EqAlpha;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    XCTAssertEqual(EqAlpha, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
    
    
    
    CC_GL_BLEND_EQUATION(GL_FUNC_SUBTRACT); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_SUBTRACT, @"should be GL_FUNC_SUBTRACT");
    XCTAssertEqual(EqAlpha, GL_FUNC_SUBTRACT, @"should be GL_FUNC_SUBTRACT");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode = GL_FUNC_ADD;
        CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode = GL_FUNC_ADD;
    }
#else
    CC_GL_CURRENT_STATE->blendEquation.rgb.mode = GL_FUNC_ADD;
    CC_GL_CURRENT_STATE->blendEquation.alpha.mode = GL_FUNC_ADD;
#endif
    CC_GL_BLEND_EQUATION(GL_FUNC_ADD); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_SUBTRACT, @"should be unchanged");
    XCTAssertEqual(EqAlpha, GL_FUNC_SUBTRACT, @"should be unchanged");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode = GL_FUNC_SUBTRACT;
        CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode = GL_FUNC_ADD;
    }
#else
    CC_GL_CURRENT_STATE->blendEquation.rgb.mode = GL_FUNC_SUBTRACT;
    CC_GL_CURRENT_STATE->blendEquation.alpha.mode = GL_FUNC_ADD;
#endif
    CC_GL_BLEND_EQUATION(GL_FUNC_ADD); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    XCTAssertEqual(EqAlpha, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
#endif
}

-(void) testBlendEquationSeparateMacro
{
#if CC_GL_STATE_BLEND
    glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_ADD); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_BLEND_EQUATION_SEPARATE(GL_FUNC_ADD, GL_FUNC_ADD); CC_GL_CHECK();
    
    GLenum EqRGB, EqAlpha;
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    XCTAssertEqual(EqAlpha, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
    
    
    
    CC_GL_BLEND_EQUATION_SEPARATE(GL_FUNC_SUBTRACT, GL_FUNC_ADD); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_SUBTRACT, @"should be GL_FUNC_SUBTRACT");
    XCTAssertEqual(EqAlpha, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode = GL_FUNC_ADD;
        CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode = GL_FUNC_ADD;
    }
#else
    CC_GL_CURRENT_STATE->blendEquation.rgb.mode = GL_FUNC_ADD;
    CC_GL_CURRENT_STATE->blendEquation.alpha.mode = GL_FUNC_ADD;
#endif
    CC_GL_BLEND_EQUATION_SEPARATE(GL_FUNC_ADD, GL_FUNC_ADD); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_SUBTRACT, @"should be unchanged");
    XCTAssertEqual(EqAlpha, GL_FUNC_ADD, @"should be unchanged");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
#endif
    
    
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode = GL_FUNC_ADD;
        CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode = GL_FUNC_ADD;
    }
#else
    CC_GL_CURRENT_STATE->blendEquation.rgb.mode = GL_FUNC_ADD;
    CC_GL_CURRENT_STATE->blendEquation.alpha.mode = GL_FUNC_ADD;
#endif
    CC_GL_BLEND_EQUATION_SEPARATE(GL_FUNC_ADD, GL_FUNC_SUBTRACT); CC_GL_CHECK();
    
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&EqRGB); CC_GL_CHECK();
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&EqAlpha); CC_GL_CHECK();
    
    XCTAssertEqual(EqRGB, GL_FUNC_ADD, @"should be GL_FUNC_ADD");
    XCTAssertEqual(EqAlpha, GL_FUNC_SUBTRACT, @"should be GL_FUNC_SUBTRACT");
    
#if CC_GL_VERSION_MIN_REQUIRED(CC_OPENGL_VERSION_4_0, CC_OPENGL_ES_VERSION_NA) && CC_GL_VERSION_MAX_SUPPORTED(CC_OPENGL_VERSION_NA, CC_OPENGL_ES_VERSION_NA)
    for (GLint Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
        XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation[Loop].alpha.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
    }
#else
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.rgb.mode, GL_FUNC_ADD, @"state should be GL_FUNC_ADD");
    XCTAssertEqual(CC_GL_CURRENT_STATE->blendEquation.alpha.mode, GL_FUNC_SUBTRACT, @"state should be GL_FUNC_SUBTRACT");
#endif
#endif
}

-(void) testBufferState
{
#if CC_GL_STATE_BUFFER
#define TEST_BUFFER_TARGET(target) { target, &State->bindBuffer._##target, #target }
    CCGLState *State = CCGLCurrentState;
    
    const struct {
        GLenum target;
        const GLenum *state;
        char *name;
    } BufferTargets[] = {
        TEST_BUFFER_TARGET(GL_ARRAY_BUFFER),
        CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER),)
#if GL_COPY_READ_BUFFER_BINDING
        CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER),)
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
        CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER),)
#endif
        CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER),)
        CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER),)
        TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER),
        CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER),)
        CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER),)
        CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER),)
        CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER),)
#if GL_TEXTURE_BUFFER_BINDING
        CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER),)
#endif
        CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER),)
        CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER),)
    };
    const size_t Count = sizeof(BufferTargets) / sizeof(typeof(*BufferTargets));
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        glBindBuffer(BufferTargets[Loop].target, 0); CC_GL_CHECK();
    }
    
    CCGLStateInitializeWithCurrent(State);
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        XCTAssertEqual(*BufferTargets[Loop].state, 0, @"%s should be %u", BufferTargets[Loop].name, 0);
    }
    
    
    GLuint Buffers[Count];
    
    glGenBuffers(Count, Buffers); CC_GL_CHECK();
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        glBindBuffer(BufferTargets[Loop].target, Buffers[Loop]); CC_GL_CHECK();
    }
    
    CCGLStateInitializeWithCurrent(State);
    
    for (size_t Loop = 0; Loop < Count; Loop++)
    {
        XCTAssertEqual(*BufferTargets[Loop].state, Buffers[Loop], @"%s should be %u", BufferTargets[Loop].name, Buffers[Loop]);
    }
    
    glDeleteBuffers(Count, Buffers); CC_GL_CHECK();
#endif
}

-(void) testBindBufferMacro
{
#if CC_GL_STATE_BUFFER
#undef TEST_BUFFER_TARGET
#define TEST_BUFFER_TARGET(target) \
glGetIntegerv(target##_BINDING, (GLint*)&BoundBuffer); CC_GL_CHECK(); \
XCTAssertEqual(BoundBuffer, CurrentBuffer, @"Should be bound to the buffer"); \
XCTAssertEqual(CC_GL_CURRENT_STATE->bindBuffer._##target, CurrentBuffer, @"State should be the buffer");
    
    GLuint CurrentBuffer = 0;
    glBindBuffer(GL_ARRAY_BUFFER, CurrentBuffer); CC_GL_CHECK();
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, CurrentBuffer); CC_GL_CHECK());
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, glBindBuffer(GL_COPY_READ_BUFFER, CurrentBuffer); CC_GL_CHECK());
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, glBindBuffer(GL_COPY_WRITE_BUFFER, CurrentBuffer); CC_GL_CHECK());
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, CurrentBuffer); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, glBindBuffer(GL_DRAW_INDIRECT_BUFFER, CurrentBuffer); CC_GL_CHECK());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CurrentBuffer); CC_GL_CHECK();
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, glBindBuffer(GL_PIXEL_PACK_BUFFER, CurrentBuffer); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, glBindBuffer(GL_PIXEL_UNPACK_BUFFER, CurrentBuffer); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, glBindBuffer(GL_QUERY_BUFFER, CurrentBuffer); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, glBindBuffer(GL_SHADER_STORAGE_BUFFER, CurrentBuffer); CC_GL_CHECK());
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, glBindBuffer(GL_TEXTURE_BUFFER, CurrentBuffer); CC_GL_CHECK());
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, CurrentBuffer); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, glBindBuffer(GL_UNIFORM_BUFFER, CurrentBuffer); CC_GL_CHECK());
    
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    GLuint Buffer;
    glGenBuffers(1, &Buffer); CC_GL_CHECK();
    
    CurrentBuffer = Buffer;
    CC_GL_BIND_BUFFER(GL_ARRAY_BUFFER, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_ATOMIC_COUNTER_BUFFER, CurrentBuffer));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_COPY_READ_BUFFER, CurrentBuffer));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_COPY_WRITE_BUFFER, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_DISPATCH_INDIRECT_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_DRAW_INDIRECT_BUFFER, CurrentBuffer));
    CC_GL_BIND_BUFFER(GL_ELEMENT_ARRAY_BUFFER, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_PIXEL_PACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_PIXEL_UNPACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, CC_GL_BIND_BUFFER(GL_QUERY_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_SHADER_STORAGE_BUFFER, CurrentBuffer));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, CC_GL_BIND_BUFFER(GL_TEXTURE_BUFFER, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_TRANSFORM_FEEDBACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_UNIFORM_BUFFER, CurrentBuffer));
    
    
    GLuint BoundBuffer;
    TEST_BUFFER_TARGET(GL_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER));
    TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER));
    
    
    
    CurrentBuffer = Buffer;
    CC_GL_BIND_BUFFER(GL_ARRAY_BUFFER, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_ATOMIC_COUNTER_BUFFER, CurrentBuffer));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_COPY_READ_BUFFER, CurrentBuffer));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_COPY_WRITE_BUFFER, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_DISPATCH_INDIRECT_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_DRAW_INDIRECT_BUFFER, CurrentBuffer));
    CC_GL_BIND_BUFFER(GL_ELEMENT_ARRAY_BUFFER, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_PIXEL_PACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_PIXEL_UNPACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, CC_GL_BIND_BUFFER(GL_QUERY_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_BIND_BUFFER(GL_SHADER_STORAGE_BUFFER, CurrentBuffer));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, CC_GL_BIND_BUFFER(GL_TEXTURE_BUFFER, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_TRANSFORM_FEEDBACK_BUFFER, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_BIND_BUFFER(GL_UNIFORM_BUFFER, CurrentBuffer));
    
    
    TEST_BUFFER_TARGET(GL_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER));
    TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER));
    
    
    
    GLenum Target;
    CurrentBuffer = 0;
    Target = GL_ARRAY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, Target = GL_ATOMIC_COUNTER_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_COPY_READ_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_COPY_WRITE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, Target = GL_DISPATCH_INDIRECT_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, Target = GL_DRAW_INDIRECT_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    Target = GL_ELEMENT_ARRAY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, Target = GL_PIXEL_PACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, Target = GL_PIXEL_UNPACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, Target = GL_QUERY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, Target = GL_SHADER_STORAGE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, Target = GL_TEXTURE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, Target = GL_TRANSFORM_FEEDBACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_UNIFORM_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    
    
    TEST_BUFFER_TARGET(GL_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER));
    TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER));
    
    
    
    CurrentBuffer = Buffer;
    Target = GL_ARRAY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, Target = GL_ATOMIC_COUNTER_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_COPY_READ_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_COPY_WRITE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, Target = GL_DISPATCH_INDIRECT_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, Target = GL_DRAW_INDIRECT_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    Target = GL_ELEMENT_ARRAY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, Target = GL_PIXEL_PACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, Target = GL_PIXEL_UNPACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, Target = GL_QUERY_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, Target = GL_SHADER_STORAGE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, Target = GL_TEXTURE_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, Target = GL_TRANSFORM_FEEDBACK_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, Target = GL_UNIFORM_BUFFER; CC_GL_BIND_BUFFER_TARGET(Target, CurrentBuffer));
    
    
    TEST_BUFFER_TARGET(GL_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER));
    TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER));
    
    
    
#undef TEST_BUFFER_TARGET
#define TEST_BUFFER_TARGET(target) \
glGetIntegerv(target##_BINDING, (GLint*)&BoundBuffer); CC_GL_CHECK(); \
XCTAssertNotEqual(BoundBuffer, CurrentBuffer, @"Should not be bound to the buffer"); \
XCTAssertEqual(CC_GL_CURRENT_STATE->bindBuffer._##target, CurrentBuffer, @"State should be the buffer");
    
    CurrentBuffer = 0;
    CC_GL_CURRENT_STATE->bindBuffer._GL_ARRAY_BUFFER = CurrentBuffer;
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_ATOMIC_COUNTER_BUFFER = CurrentBuffer);
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_COPY_READ_BUFFER = CurrentBuffer);
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_COPY_WRITE_BUFFER = CurrentBuffer);
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_DISPATCH_INDIRECT_BUFFER = CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_DRAW_INDIRECT_BUFFER = CurrentBuffer);
    CC_GL_CURRENT_STATE->bindBuffer._GL_ELEMENT_ARRAY_BUFFER = CurrentBuffer;
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_PIXEL_PACK_BUFFER = CurrentBuffer);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_PIXEL_UNPACK_BUFFER = CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_QUERY_BUFFER = CurrentBuffer);
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_SHADER_STORAGE_BUFFER = CurrentBuffer);
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_TEXTURE_BUFFER = CurrentBuffer);
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_TRANSFORM_FEEDBACK_BUFFER = CurrentBuffer);
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, CC_GL_CURRENT_STATE->bindBuffer._GL_UNIFORM_BUFFER = CurrentBuffer);
    
    
    TEST_BUFFER_TARGET(GL_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(4_2, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_ATOMIC_COUNTER_BUFFER));
#if GL_COPY_READ_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_READ_BUFFER));
#endif
#if GL_COPY_WRITE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_COPY_WRITE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DISPATCH_INDIRECT_BUFFER));
    CC_GL_VERSION_ACTIVE(4_0, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_DRAW_INDIRECT_BUFFER));
    TEST_BUFFER_TARGET(GL_ELEMENT_ARRAY_BUFFER);
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_PACK_BUFFER));
    CC_GL_VERSION_ACTIVE(1_5, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_PIXEL_UNPACK_BUFFER));
    CC_GL_VERSION_ACTIVE(4_4, NA, NA, NA, TEST_BUFFER_TARGET(GL_QUERY_BUFFER));
    CC_GL_VERSION_ACTIVE(4_3, NA, 3_1, NA, TEST_BUFFER_TARGET(GL_SHADER_STORAGE_BUFFER));
#if GL_TEXTURE_BUFFER_BINDING
    CC_GL_VERSION_ACTIVE(3_1, NA, NA, NA, TEST_BUFFER_TARGET(GL_TEXTURE_BUFFER));
#endif
    CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_TRANSFORM_FEEDBACK_BUFFER));
    CC_GL_VERSION_ACTIVE(3_1, NA, 3_0, NA, TEST_BUFFER_TARGET(GL_UNIFORM_BUFFER));
    
    glDeleteBuffers(1, &Buffer); CC_GL_CHECK();
#endif
}

-(void) testColourState
{
#if CC_GL_STATE_COLOUR
    CCGLState *State = CCGLCurrentState;
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); CC_GL_CHECK();
    glColorMask(FALSE, FALSE, FALSE, FALSE); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearColour.red, 0.0f, @"should be 0.0");
    XCTAssertEqual(State->clearColour.green, 0.0f, @"should be 0.0");
    XCTAssertEqual(State->clearColour.blue, 0.0f, @"should be 0.0");
    XCTAssertEqual(State->clearColour.alpha, 0.0f, @"should be 0.0");
    XCTAssertFalse(State->colourMask.red, @"should be false");
    XCTAssertFalse(State->colourMask.green, @"should be false");
    XCTAssertFalse(State->colourMask.blue, @"should be false");
    XCTAssertFalse(State->colourMask.alpha, @"should be false");
    
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); CC_GL_CHECK();
    glColorMask(TRUE, TRUE, TRUE, TRUE); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearColour.red, 1.0f, @"should be 1.0");
    XCTAssertEqual(State->clearColour.green, 1.0f, @"should be 1.0");
    XCTAssertEqual(State->clearColour.blue, 1.0f, @"should be 1.0");
    XCTAssertEqual(State->clearColour.alpha, 1.0f, @"should be 1.0");
    XCTAssertTrue(State->colourMask.red, @"should be true");
    XCTAssertTrue(State->colourMask.green, @"should be true");
    XCTAssertTrue(State->colourMask.blue, @"should be true");
    XCTAssertTrue(State->colourMask.alpha, @"should be true");
#endif
}

-(void) testClearColourMacros
{
#if CC_GL_STATE_COLOUR
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_CLEAR_COLOR(0.2f, 0.4f, 0.6f, 0.8f);
    
    GLclampf ClearColour[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, ClearColour); CC_GL_CHECK();
    XCTAssertEqual(ClearColour[0], 0.2f, @"Should have correct value");
    XCTAssertEqual(ClearColour[1], 0.4f, @"Should have correct value");
    XCTAssertEqual(ClearColour[2], 0.6f, @"Should have correct value");
    XCTAssertEqual(ClearColour[3], 0.8f, @"Should have correct value");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.red, 0.2f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.green, 0.4f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.blue, 0.6f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.alpha, 0.8f, @"State should have correct value");
    
    
    
    CC_GL_CLEAR_COLOR(0.0f, 0.4f, 0.6f, 0.8f);
    
    glGetFloatv(GL_COLOR_CLEAR_VALUE, ClearColour); CC_GL_CHECK();
    XCTAssertEqual(ClearColour[0], 0.0f, @"Should have correct value");
    XCTAssertEqual(ClearColour[1], 0.4f, @"Should have correct value");
    XCTAssertEqual(ClearColour[2], 0.6f, @"Should have correct value");
    XCTAssertEqual(ClearColour[3], 0.8f, @"Should have correct value");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.red, 0.0f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.green, 0.4f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.blue, 0.6f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.alpha, 0.8f, @"State should have correct value");
    
    
    
    CC_GL_CURRENT_STATE->clearColour.red = 1.0f;
    CC_GL_CURRENT_STATE->clearColour.green = 1.0f;
    CC_GL_CURRENT_STATE->clearColour.blue = 1.0f;
    CC_GL_CURRENT_STATE->clearColour.alpha = 1.0f;
    
    CC_GL_CLEAR_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    
    glGetFloatv(GL_COLOR_CLEAR_VALUE, ClearColour); CC_GL_CHECK();
    XCTAssertEqual(ClearColour[0], 0.0f, @"Should remain the same");
    XCTAssertEqual(ClearColour[1], 0.4f, @"Should remain the same");
    XCTAssertEqual(ClearColour[2], 0.6f, @"Should remain the same");
    XCTAssertEqual(ClearColour[3], 0.8f, @"Should remain the same");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.red, 1.0f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.green, 1.0f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.blue, 1.0f, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearColour.alpha, 1.0f, @"State should have correct value");
#endif
}

-(void) testColourMaskMacros
{
#if CC_GL_STATE_COLOUR
    glColorMask(TRUE, TRUE, TRUE, TRUE); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_COLOR_MASK(TRUE, FALSE, TRUE, FALSE);
    
    GLboolean ColourMask[4];
    glGetBooleanv(GL_COLOR_WRITEMASK, ColourMask); CC_GL_CHECK();
    XCTAssertEqual(ColourMask[0], TRUE, @"Should have correct value");
    XCTAssertEqual(ColourMask[1], FALSE, @"Should have correct value");
    XCTAssertEqual(ColourMask[2], TRUE, @"Should have correct value");
    XCTAssertEqual(ColourMask[3], FALSE, @"Should have correct value");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.red, TRUE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.green, FALSE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.blue, TRUE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.alpha, FALSE, @"State should have correct value");
    
    
    
    CC_GL_COLOR_MASK(TRUE, FALSE, TRUE, TRUE);
    
    glGetBooleanv(GL_COLOR_WRITEMASK, ColourMask); CC_GL_CHECK();
    XCTAssertEqual(ColourMask[0], TRUE, @"Should have correct value");
    XCTAssertEqual(ColourMask[1], FALSE, @"Should have correct value");
    XCTAssertEqual(ColourMask[2], TRUE, @"Should have correct value");
    XCTAssertEqual(ColourMask[3], TRUE, @"Should have correct value");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.red, TRUE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.green, FALSE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.blue, TRUE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.alpha, TRUE, @"State should have correct value");
    
    
    
    CC_GL_CURRENT_STATE->colourMask.red = FALSE;
    CC_GL_CURRENT_STATE->colourMask.green = FALSE;
    CC_GL_CURRENT_STATE->colourMask.blue = FALSE;
    CC_GL_CURRENT_STATE->colourMask.alpha = FALSE;
    
    CC_GL_COLOR_MASK(FALSE, FALSE, FALSE, FALSE);
    
    glGetBooleanv(GL_COLOR_WRITEMASK, ColourMask); CC_GL_CHECK();
    XCTAssertEqual(ColourMask[0], TRUE, @"Should remain the same");
    XCTAssertEqual(ColourMask[1], FALSE, @"Should remain the same");
    XCTAssertEqual(ColourMask[2], TRUE, @"Should remain the same");
    XCTAssertEqual(ColourMask[3], TRUE, @"Should remain the same");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.red, FALSE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.green, FALSE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.blue, FALSE, @"State should have correct value");
    XCTAssertEqual(CC_GL_CURRENT_STATE->colourMask.alpha, FALSE, @"State should have correct value");
#endif
}

-(void) testCullFaceState
{
#if CC_GL_STATE_CULL_FACE
    CCGLState *State = CCGLCurrentState;
    
    glCullFace(GL_FRONT); CC_GL_CHECK();
    glFrontFace(GL_CW); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->cullFace.mode, GL_FRONT, @"should be GL_FRONT");
    XCTAssertEqual(State->frontFace.mode, GL_CW, @"should be GL_CW");
    
    
    glCullFace(GL_BACK); CC_GL_CHECK();
    glFrontFace(GL_CCW); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->cullFace.mode, GL_BACK, @"should be GL_BACK");
    XCTAssertEqual(State->frontFace.mode, GL_CCW, @"should be GL_CCW");
#endif
}

-(void) testCullFaceMacro
{
#if CC_GL_STATE_CULL_FACE
    glCullFace(GL_FRONT); CC_GL_CHECK();
    glFrontFace(GL_CW); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_CULL_FACE(GL_BACK);
    CC_GL_FRONT_FACE(GL_CCW);
    
    GLenum CullFace, FrontFace;
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&CullFace); CC_GL_CHECK();
    glGetIntegerv(GL_FRONT_FACE, (GLint*)&FrontFace); CC_GL_CHECK();
    
    XCTAssertEqual(CullFace, GL_BACK, @"should be GL_BACK");
    XCTAssertEqual(FrontFace, GL_CCW, @"should be GL_CCW");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->cullFace.mode, GL_BACK, @"should be GL_BACK");
    XCTAssertEqual(CC_GL_CURRENT_STATE->frontFace.mode, GL_CCW, @"should be GL_CCW");
    
    
    
    CC_GL_CULL_FACE(GL_FRONT);
    CC_GL_FRONT_FACE(GL_CW);
    
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&CullFace); CC_GL_CHECK();
    glGetIntegerv(GL_FRONT_FACE, (GLint*)&FrontFace); CC_GL_CHECK();
    
    XCTAssertEqual(CullFace, GL_FRONT, @"should be GL_FRONT");
    XCTAssertEqual(FrontFace, GL_CW, @"should be GL_CW");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->cullFace.mode, GL_FRONT, @"should be GL_FRONT");
    XCTAssertEqual(CC_GL_CURRENT_STATE->frontFace.mode, GL_CW, @"should be GL_CW");
    
    
    
    CC_GL_CURRENT_STATE->cullFace.mode = GL_BACK;
    CC_GL_CURRENT_STATE->frontFace.mode = GL_CCW;
    
    CC_GL_CULL_FACE(GL_BACK);
    CC_GL_FRONT_FACE(GL_CCW);
    
    glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&CullFace); CC_GL_CHECK();
    glGetIntegerv(GL_FRONT_FACE, (GLint*)&FrontFace); CC_GL_CHECK();
    
    XCTAssertEqual(CullFace, GL_FRONT, @"should be unchanged");
    XCTAssertEqual(FrontFace, GL_CW, @"should be unchanged");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->cullFace.mode, GL_BACK, @"should be GL_BACK");
    XCTAssertEqual(CC_GL_CURRENT_STATE->frontFace.mode, GL_CCW, @"should be GL_CCW");
#endif
}

-(void) testDepthState
{
#if CC_GL_STATE_DEPTH
    CCGLState *State = CCGLCurrentState;
    
    glClearDepth(0.0f); CC_GL_CHECK();
    glDepthFunc(GL_NEVER); CC_GL_CHECK();
    glDepthMask(FALSE); CC_GL_CHECK();
    glDepthRange(0.0f, 0.0f); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearDepth.depth, 0.0f, @"should be 0.0");
    XCTAssertEqual(State->depthFunc.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertFalse(State->depthMask.flag, @"should be false");
    XCTAssertEqual(State->depthRange.near, 0.0f, @"should be 0.0");
    XCTAssertEqual(State->depthRange.far, 0.0f, @"should be 0.0");
    
    
    glClearDepth(1.0f); CC_GL_CHECK();
    glDepthFunc(GL_ALWAYS); CC_GL_CHECK();
    glDepthMask(TRUE); CC_GL_CHECK();
    glDepthRange(1.0f, 1.0f); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearDepth.depth, 1.0f, @"should be 1.0");
    XCTAssertEqual(State->depthFunc.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertTrue(State->depthMask.flag, @"should be true");
    XCTAssertEqual(State->depthRange.near, 1.0f, @"should be 1.0");
    XCTAssertEqual(State->depthRange.far, 1.0f, @"should be 1.0");
#endif
}

-(void) testDepthMacro
{
#if CC_GL_STATE_DEPTH
    glClearDepth(0.0f); CC_GL_CHECK();
    glDepthFunc(GL_NEVER); CC_GL_CHECK();
    glDepthMask(FALSE); CC_GL_CHECK();
    glDepthRange(0.0f, 0.0f); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_CLEAR_DEPTH(1.0f);
    CC_GL_DEPTH_FUNC(GL_ALWAYS);
    CC_GL_DEPTH_MASK(TRUE);
    CC_GL_DEPTH_RANGE(0.5f, 1.0f);
    
    GLclampd ClearDepth, DepthRange[2];
    GLenum DepthFunc;
    GLboolean DepthMask;
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &ClearDepth); CC_GL_CHECK();
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&DepthFunc); CC_GL_CHECK();
    glGetBooleanv(GL_DEPTH_WRITEMASK, &DepthMask); CC_GL_CHECK();
    glGetDoublev(GL_DEPTH_RANGE, DepthRange); CC_GL_CHECK();
    
    XCTAssertEqual(ClearDepth, 1.0f, @"should be 1.0");
    XCTAssertEqual(DepthFunc, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertTrue(DepthMask, @"should be true");
    XCTAssertEqual(DepthRange[0], 0.5f, @"should be 0.5");
    XCTAssertEqual(DepthRange[1], 1.0f, @"should be 1.0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearDepth.depth, 1.0f, @"should be 1.0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthFunc.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertTrue(CC_GL_CURRENT_STATE->depthMask.flag, @"should be true");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.near, 0.5f, @"should be 0.5");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.far, 1.0f, @"should be 1.0");
    
    
    
    CC_GL_CLEAR_DEPTH(0.0f);
    CC_GL_DEPTH_FUNC(GL_NEVER);
    CC_GL_DEPTH_MASK(FALSE);
    CC_GL_DEPTH_RANGE(0.5f, 0.5f);
    
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &ClearDepth); CC_GL_CHECK();
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&DepthFunc); CC_GL_CHECK();
    glGetBooleanv(GL_DEPTH_WRITEMASK, &DepthMask); CC_GL_CHECK();
    glGetDoublev(GL_DEPTH_RANGE, DepthRange); CC_GL_CHECK();
    
    XCTAssertEqual(ClearDepth, 0.0f, @"should be 0.0");
    XCTAssertEqual(DepthFunc, GL_NEVER, @"should be GL_NEVER");
    XCTAssertFalse(DepthMask, @"should be false");
    XCTAssertEqual(DepthRange[0], 0.5f, @"should be 0.5");
    XCTAssertEqual(DepthRange[1], 0.5f, @"should be 0.5");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearDepth.depth, 0.0f, @"should be 0.0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthFunc.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertFalse(CC_GL_CURRENT_STATE->depthMask.flag, @"should be false");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.near, 0.5f, @"should be 0.5");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.far, 0.5f, @"should be 0.5");
    
    
    
    CC_GL_CURRENT_STATE->clearDepth.depth = 1.0f;
    CC_GL_CURRENT_STATE->depthFunc.func = GL_ALWAYS;
    CC_GL_CURRENT_STATE->depthMask.flag = TRUE;
    CC_GL_CURRENT_STATE->depthRange.near = 0.0f;
    CC_GL_CURRENT_STATE->depthRange.far = 1.0f;
    
    CC_GL_CLEAR_DEPTH(1.0f);
    CC_GL_DEPTH_FUNC(GL_ALWAYS);
    CC_GL_DEPTH_MASK(TRUE);
    CC_GL_DEPTH_RANGE(0.0f, 1.0f);
    
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &ClearDepth); CC_GL_CHECK();
    glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&DepthFunc); CC_GL_CHECK();
    glGetBooleanv(GL_DEPTH_WRITEMASK, &DepthMask); CC_GL_CHECK();
    glGetDoublev(GL_DEPTH_RANGE, DepthRange); CC_GL_CHECK();
    
    XCTAssertEqual(ClearDepth, 0.0f, @"should be unchanged");
    XCTAssertEqual(DepthFunc, GL_NEVER, @"should be unchanged");
    XCTAssertFalse(DepthMask, @"should be unchanged");
    XCTAssertEqual(DepthRange[0], 0.5f, @"should be unchanged");
    XCTAssertEqual(DepthRange[1], 0.5f, @"should be unchanged");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearDepth.depth, 1.0f, @"should be 1.0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthFunc.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertTrue(CC_GL_CURRENT_STATE->depthMask.flag, @"should be true");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.near, 0.0f, @"should be 0.0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->depthRange.far, 1.0f, @"should be 1.0");
#endif
}

#define TEST_GL_ENABLE(cap) \
glEnable(cap); CC_GL_CHECK(); \
CCGLStateInitializeWithCurrent(State); \
XCTAssertTrue(State->enabled._##cap, @#cap " should be enabled");

#define TEST_GL_DISABLE(cap) \
glDisable(cap); CC_GL_CHECK(); \
CCGLStateInitializeWithCurrent(State); \
XCTAssertFalse(State->enabled._##cap, @#cap " should be disabled");

-(void) testEnabledState
{
#if CC_GL_STATE_ENABLED
    CCGLState *State = CCGLCurrentState;
    
    glEnable(GL_BLEND); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(State);
    for (size_t Loop = 0, Count = CC_GL_CAPABILITY(State, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertTrue(State->enabled.blend[Loop], @"GL_BLEND should be enabled");
    }
    
    TEST_GL_ENABLE(GL_COLOR_LOGIC_OP);
    TEST_GL_ENABLE(GL_CULL_FACE);
    TEST_GL_ENABLE(GL_DEPTH_CLAMP);
    TEST_GL_ENABLE(GL_DEPTH_TEST);
    TEST_GL_ENABLE(GL_DITHER);
    TEST_GL_ENABLE(GL_FRAMEBUFFER_SRGB);
    TEST_GL_ENABLE(GL_LINE_SMOOTH);
    TEST_GL_ENABLE(GL_MULTISAMPLE);
    TEST_GL_ENABLE(GL_POLYGON_OFFSET_FILL);
    TEST_GL_ENABLE(GL_POLYGON_OFFSET_LINE);
    TEST_GL_ENABLE(GL_POLYGON_OFFSET_POINT);
    TEST_GL_ENABLE(GL_POLYGON_SMOOTH);
    TEST_GL_ENABLE(GL_PRIMITIVE_RESTART);
    TEST_GL_ENABLE(GL_PROGRAM_POINT_SIZE);
    TEST_GL_ENABLE(GL_RASTERIZER_DISCARD);
    TEST_GL_ENABLE(GL_SAMPLE_ALPHA_TO_COVERAGE);
    TEST_GL_ENABLE(GL_SAMPLE_ALPHA_TO_ONE);
    TEST_GL_ENABLE(GL_SAMPLE_COVERAGE);
    TEST_GL_ENABLE(GL_SAMPLE_SHADING);
    TEST_GL_ENABLE(GL_SAMPLE_MASK);
    TEST_GL_ENABLE(GL_SCISSOR_TEST);
    TEST_GL_ENABLE(GL_STENCIL_TEST);
    TEST_GL_ENABLE(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
}

-(void) testDisabledState
{
#if CC_GL_STATE_ENABLED
    CCGLState *State = CCGLCurrentState;
    
    glDisable(GL_BLEND); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(State);
    for (size_t Loop = 0, Count = CC_GL_CAPABILITY(State, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertFalse(State->enabled.blend[Loop], @"GL_BLEND should be disabled");
    }
    
    TEST_GL_DISABLE(GL_COLOR_LOGIC_OP);
    TEST_GL_DISABLE(GL_CULL_FACE);
    TEST_GL_DISABLE(GL_DEPTH_CLAMP);
    TEST_GL_DISABLE(GL_DEPTH_TEST);
    TEST_GL_DISABLE(GL_DITHER);
    TEST_GL_DISABLE(GL_FRAMEBUFFER_SRGB);
    TEST_GL_DISABLE(GL_LINE_SMOOTH);
    TEST_GL_DISABLE(GL_MULTISAMPLE);
    TEST_GL_DISABLE(GL_POLYGON_OFFSET_FILL);
    TEST_GL_DISABLE(GL_POLYGON_OFFSET_LINE);
    TEST_GL_DISABLE(GL_POLYGON_OFFSET_POINT);
    TEST_GL_DISABLE(GL_POLYGON_SMOOTH);
    TEST_GL_DISABLE(GL_PRIMITIVE_RESTART);
    TEST_GL_DISABLE(GL_PROGRAM_POINT_SIZE);
    TEST_GL_DISABLE(GL_RASTERIZER_DISCARD);
    TEST_GL_DISABLE(GL_SAMPLE_ALPHA_TO_COVERAGE);
    TEST_GL_DISABLE(GL_SAMPLE_ALPHA_TO_ONE);
    TEST_GL_DISABLE(GL_SAMPLE_COVERAGE);
    TEST_GL_DISABLE(GL_SAMPLE_SHADING);
    TEST_GL_DISABLE(GL_SAMPLE_MASK);
    TEST_GL_DISABLE(GL_SCISSOR_TEST);
    TEST_GL_DISABLE(GL_STENCIL_TEST);
    TEST_GL_DISABLE(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
}

-(void) testEnabledMacro
{
#if CC_GL_STATE_ENABLED
    glDisable(GL_BLEND); CC_GL_CHECK();
    glDisable(GL_DEPTH_TEST); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_ENABLE(GL_BLEND);
    CC_GL_CURRENT_STATE->enabled._GL_DEPTH_TEST = TRUE;
    CC_GL_ENABLE(GL_DEPTH_TEST);
    
    XCTAssertTrue(glIsEnabled(GL_BLEND), @"GL_BLEND should be enabled"); CC_GL_CHECK();
    for (size_t Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertTrue(CC_GL_CURRENT_STATE->enabled.blend[Loop], @"GL_BLEND should be true");
    }
    XCTAssertFalse(glIsEnabled(GL_DEPTH_TEST), @"GL_DEPTH_TEST should stay disabled"); CC_GL_CHECK();
    XCTAssertTrue(CC_GL_CURRENT_STATE->enabled._GL_DEPTH_TEST, @"GL_DEPTH_TEST state should be true");
    
    
    CC_GL_DISABLE(GL_BLEND);
    glEnable(GL_DEPTH_TEST); CC_GL_CHECK();
    CC_GL_CURRENT_STATE->enabled._GL_DEPTH_TEST = FALSE;
    CC_GL_DISABLE(GL_DEPTH_TEST);
    
    XCTAssertFalse(glIsEnabled(GL_BLEND), @"GL_BLEND should be disabled"); CC_GL_CHECK();
    for (size_t Loop = 0, Count = CC_GL_CAPABILITY(CC_GL_CURRENT_STATE, GL_MAX_DRAW_BUFFERS); Loop < Count; Loop++)
    {
        XCTAssertFalse(CC_GL_CURRENT_STATE->enabled.blend[Loop], @"GL_BLEND should be false");
    }
    XCTAssertTrue(glIsEnabled(GL_DEPTH_TEST), @"GL_DEPTH_TEST should stay enabled"); CC_GL_CHECK();
    XCTAssertFalse(CC_GL_CURRENT_STATE->enabled._GL_DEPTH_TEST, @"GL_DEPTH_TEST state should be false");
#endif
}

-(void) testEnabledIndexed
{
#if CC_GL_STATE_ENABLED
    CCGLState *State = CCGLCurrentState;
    
    if (CC_GL_CAPABILITY(State, GL_MAX_DRAW_BUFFERS) > 1)
    {
        glEnablei(GL_BLEND, 0); CC_GL_CHECK();
        glDisablei(GL_BLEND, 1); CC_GL_CHECK();
        CCGLStateInitializeWithCurrent(State);
        XCTAssertTrue(State->enabled.blend[0], @"GL_BLEND should be enabled");
        XCTAssertFalse(State->enabled.blend[1], @"GL_BLEND should be disabled");
        
        CC_GL_ENABLE(GL_BLEND);
        XCTAssertTrue(State->enabled.blend[0], @"GL_BLEND should be enabled");
        XCTAssertTrue(State->enabled.blend[1], @"GL_BLEND should be enabled");
        
        CC_GL_DISABLEi(GL_BLEND, 0);
        XCTAssertFalse(State->enabled.blend[0], @"GL_BLEND should be disabled");
        XCTAssertTrue(State->enabled.blend[1], @"GL_BLEND should be enabled");
    }
#endif
}

-(void) testFramebufferState
{
#if CC_GL_STATE_FRAMEBUFFER
    CCGLState *State = CCGLCurrentState;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->bindFramebuffer.read, 0, @"should be 0");
    XCTAssertEqual(State->bindFramebuffer.write, 0, @"should be 0");
    
    
    GLuint fbo;
    glGenFramebuffers(1, &fbo); CC_GL_CHECK();
    glBindFramebuffer(GL_FRAMEBUFFER, 1); CC_GL_CHECK();

    CCGLStateInitializeWithCurrent(State);

    XCTAssertEqual(State->bindFramebuffer.read, fbo, @"should be %u", fbo);
    XCTAssertEqual(State->bindFramebuffer.write, fbo, @"should be %u", fbo);
    
    glDeleteFramebuffers(1, &fbo); CC_GL_CHECK();
#endif
}

-(void) testFramebufferMacro
{
#if CC_GL_STATE_FRAMEBUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, 0); CC_GL_CHECK();
    
    GLuint fbo;
    glGenFramebuffers(1, &fbo); CC_GL_CHECK();
    glBindFramebuffer(GL_FRAMEBUFFER, 1); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, fbo);
    
    GLuint BoundFBORead, BoundFBOWrite;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, fbo, @"should be bound to the fbo");
    XCTAssertEqual(BoundFBORead, fbo, @"should be bound to the fbo");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, fbo, @"state should be the fbo");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, fbo, @"state should be the fbo");
    
    
    
    CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, 0);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, 0, @"should be bound to 0");
    XCTAssertEqual(BoundFBORead, 0, @"should be bound to 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, 0, @"state should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, 0, @"state should be 0");
    
    
    
    CC_GL_CURRENT_STATE->bindFramebuffer.read = fbo;
    CC_GL_CURRENT_STATE->bindFramebuffer.write = fbo;
    CC_GL_BIND_FRAMEBUFFER(GL_FRAMEBUFFER, fbo);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, 0, @"should be unchanged");
    XCTAssertEqual(BoundFBORead, 0, @"should be unchanged");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, fbo, @"state should changed");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, fbo, @"state should changed");
    
    
    
    GLenum Target = GL_FRAMEBUFFER;
    CC_GL_BIND_FRAMEBUFFER_TARGET(Target, 0);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, 0, @"should be bound to 0");
    XCTAssertEqual(BoundFBORead, 0, @"should be bound to 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, 0, @"state should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, 0, @"state should be 0");
    
    
    
    CC_GL_BIND_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER, 0);
    CC_GL_BIND_FRAMEBUFFER(GL_READ_FRAMEBUFFER, fbo);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, 0, @"should be bound to 0");
    XCTAssertEqual(BoundFBORead, fbo, @"should be bound to the fbo");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, fbo, @"state should be the fbo");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, 0, @"state should be 0");
    
    
    
    CC_GL_BIND_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER, fbo);
    CC_GL_BIND_FRAMEBUFFER(GL_READ_FRAMEBUFFER, 0);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, fbo, @"should be bound to the fbo");
    XCTAssertEqual(BoundFBORead, 0, @"should be bound to 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, 0, @"state should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, fbo, @"state should be the fbo");
    
    
    
    CC_GL_CURRENT_STATE->bindFramebuffer.read = fbo;
    CC_GL_CURRENT_STATE->bindFramebuffer.write = 0;
    
    CC_GL_BIND_FRAMEBUFFER(GL_DRAW_FRAMEBUFFER, 0);
    CC_GL_BIND_FRAMEBUFFER(GL_READ_FRAMEBUFFER, fbo);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, fbo, @"should be unchanged");
    XCTAssertEqual(BoundFBORead, 0, @"should be unchanged");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, fbo, @"state should be the fbo");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, 0, @"state should be 0");
    
    
    
    Target = GL_DRAW_FRAMEBUFFER;
    CC_GL_BIND_FRAMEBUFFER_TARGET(Target, fbo);
    Target = GL_READ_FRAMEBUFFER;
    CC_GL_BIND_FRAMEBUFFER_TARGET(Target, 0);
    
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&BoundFBOWrite); CC_GL_CHECK();
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, (GLint*)&BoundFBORead); CC_GL_CHECK();
    
    XCTAssertEqual(BoundFBOWrite, fbo, @"should be bound to the fbo");
    XCTAssertEqual(BoundFBORead, 0, @"should be bound to 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.read, 0, @"state should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindFramebuffer.write, fbo, @"state should be to the fbo");
    
    glDeleteFramebuffers(1, &fbo); CC_GL_CHECK();
#endif
}

-(void) testScissorState
{
#if CC_GL_STATE_SCISSOR
    CCGLState *State = CCGLCurrentState;
    
    glScissor(0, 0, 0, 0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->scissor.x, 0, @"should be 0");
    XCTAssertEqual(State->scissor.y, 0, @"should be 0");
    XCTAssertEqual(State->scissor.width, 0, @"should be 0");
    XCTAssertEqual(State->scissor.height, 0, @"should be 0");
    
    
    glScissor(1, 2, 3, 4); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->scissor.x, 1, @"should be 1");
    XCTAssertEqual(State->scissor.y, 2, @"should be 2");
    XCTAssertEqual(State->scissor.width, 3, @"should be 3");
    XCTAssertEqual(State->scissor.height, 4, @"should be 4");
#endif
}

-(void) testScissorMacro
{
#if CC_GL_STATE_SCISSOR
    glScissor(0, 0, 0, 0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_SCISSOR(1, 2, 3, 4);
    
    GLint ScissorBox[4];
    glGetIntegerv(GL_SCISSOR_BOX, ScissorBox); CC_GL_CHECK();
    
    XCTAssertEqual(ScissorBox[0], 1, @"should be 1");
    XCTAssertEqual(ScissorBox[1], 2, @"should be 2");
    XCTAssertEqual(ScissorBox[2], 3, @"should be 3");
    XCTAssertEqual(ScissorBox[3], 4, @"should be 4");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.x, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.y, 2, @"should be 2");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.width, 3, @"should be 3");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.height, 4, @"should be 4");
    
    
    
    CC_GL_SCISSOR(0, 0, 0, 0);
    
    glGetIntegerv(GL_SCISSOR_BOX, ScissorBox); CC_GL_CHECK();
    
    XCTAssertEqual(ScissorBox[0], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[1], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[2], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[3], 0, @"should be 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.x, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.y, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.width, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.height, 0, @"should be 0");
    
    
    
    CC_GL_CURRENT_STATE->scissor.x = 1;
    CC_GL_CURRENT_STATE->scissor.y = 2;
    CC_GL_CURRENT_STATE->scissor.width = 3;
    CC_GL_CURRENT_STATE->scissor.height = 4;
    CC_GL_SCISSOR(1, 2, 3, 4);
    
    glGetIntegerv(GL_SCISSOR_BOX, ScissorBox); CC_GL_CHECK();
    
    XCTAssertEqual(ScissorBox[0], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[1], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[2], 0, @"should be 0");
    XCTAssertEqual(ScissorBox[3], 0, @"should be 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.x, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.y, 2, @"should be 2");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.width, 3, @"should be 3");
    XCTAssertEqual(CC_GL_CURRENT_STATE->scissor.height, 4, @"should be 4");
#endif
}

-(void) testShaderState
{
#if CC_GL_STATE_SHADER
    CCGLState *State = CCGLCurrentState;
    
    glUseProgram(0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->useProgram.program, 0, @"should be 0");
    
    
    GLuint program = glCreateProgram(); CC_GL_CHECK();
    glLinkProgram(program); CC_GL_CHECK();
    
    glUseProgram(program); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->useProgram.program, program, @"should be %u", program);
    
    glDeleteProgram(program); CC_GL_CHECK();
#endif
}

-(void) testShaderMacro
{
#if CC_GL_STATE_SHADER
    glUseProgram(0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    GLuint program = glCreateProgram(); CC_GL_CHECK();
    glLinkProgram(program); CC_GL_CHECK();
    
    CC_GL_USE_PROGRAM(program);
    
    GLuint BoundProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&BoundProgram); CC_GL_CHECK();
    
    XCTAssertEqual(BoundProgram, program, @"should be %u", program);
    XCTAssertEqual(CC_GL_CURRENT_STATE->useProgram.program, program, @"should be %u", program);
    
    
    
    CC_GL_USE_PROGRAM(0);
    
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&BoundProgram); CC_GL_CHECK();
    
    XCTAssertEqual(BoundProgram, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->useProgram.program, 0, @"should be 0");
    
    
    
    CC_GL_CURRENT_STATE->useProgram.program = program;
    CC_GL_USE_PROGRAM(program);
    
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&BoundProgram); CC_GL_CHECK();
    
    XCTAssertEqual(BoundProgram, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->useProgram.program, program, @"should be %u", program);
    
    glDeleteProgram(program); CC_GL_CHECK();
#endif
}

-(void) testStencilState
{
#if CC_GL_STATE_STENCIL
    CCGLState *State = CCGLCurrentState;
    
    glClearStencil(0); CC_GL_CHECK();
    glStencilFunc(GL_NEVER, 0, 0); CC_GL_CHECK();
    glStencilMask(0); CC_GL_CHECK();
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearStencil.s, 0, @"should be 0");
    XCTAssertEqual(State->stencilFunc.front.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(State->stencilFunc.front.ref, 0, @"should be 0");
    XCTAssertEqual(State->stencilFunc.front.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilFunc.back.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(State->stencilFunc.back.ref, 0, @"should be 0");
    XCTAssertEqual(State->stencilFunc.back.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilMask.front.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilMask.back.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilOp.front.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(State->stencilOp.front.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(State->stencilOp.front.dppass, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(State->stencilOp.back.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(State->stencilOp.back.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(State->stencilOp.back.dppass, GL_KEEP, @"should be GL_KEEP");
    
    
    glClearStencil(1); CC_GL_CHECK();
    glStencilFunc(GL_ALWAYS, 1, 1); CC_GL_CHECK();
    glStencilMask(1); CC_GL_CHECK();
    glStencilOp(GL_ZERO, GL_INCR, GL_DECR); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->clearStencil.s, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(State->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(State->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.back.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilMask.back.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilOp.front.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(State->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(State->stencilOp.front.dppass, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(State->stencilOp.back.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(State->stencilOp.back.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(State->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    glStencilFuncSeparate(GL_FRONT, GL_LESS, 0, 1); CC_GL_CHECK();
    glStencilMaskSeparate(GL_FRONT, 0); CC_GL_CHECK();
    glStencilOpSeparate(GL_FRONT, GL_INCR, GL_INCR, GL_INCR); CC_GL_CHECK();
    glStencilFuncSeparate(GL_BACK, GL_GREATER, 1, 0); CC_GL_CHECK();
    glStencilMaskSeparate(GL_BACK, 1); CC_GL_CHECK();
    glStencilOpSeparate(GL_BACK, GL_DECR, GL_DECR, GL_DECR); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->stencilFunc.front.func, GL_LESS, @"should be GL_LESS");
    XCTAssertEqual(State->stencilFunc.front.ref, 0, @"should be 0");
    XCTAssertEqual(State->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.back.func, GL_GREATER, @"should be GL_GREATER");
    XCTAssertEqual(State->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(State->stencilFunc.back.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilMask.front.mask, 0, @"should be 0");
    XCTAssertEqual(State->stencilMask.back.mask, 1, @"should be 1");
    XCTAssertEqual(State->stencilOp.front.sfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(State->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(State->stencilOp.front.dppass, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(State->stencilOp.back.sfail, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(State->stencilOp.back.dpfail, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(State->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
#endif
}

-(void) testStencilClearMacro
{
#if CC_GL_STATE_STENCIL
    glClearStencil(0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_CLEAR_STENCIL(1);
    
    GLint s;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &s); CC_GL_CHECK();
    
    XCTAssertEqual(s, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearStencil.s, 1, @"should be 1");
    
    
    CC_GL_CLEAR_STENCIL(0);
    
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &s); CC_GL_CHECK();
    
    XCTAssertEqual(s, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearStencil.s, 0, @"should be 0");
    
    
    
    CC_GL_CURRENT_STATE->clearStencil.s = 1;
    CC_GL_CLEAR_STENCIL(1);
    
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &s); CC_GL_CHECK();
    
    XCTAssertEqual(s, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->clearStencil.s, 1, @"should be 1");
#endif
}

-(void) testStencilFuncMacro
{
#if CC_GL_STATE_STENCIL
    glStencilFunc(GL_NEVER, 0, 0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_STENCIL_FUNC(GL_ALWAYS, 1, 1);
    
    GLint front[3], back[3];
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(front[1], 1, @"should be 1");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(back[1], 1, @"should be 1");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(front[1], 0, @"should be 0");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(back[1], 0, @"should be 0");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_CURRENT_STATE->stencilFunc.front.func = GL_ALWAYS;
    CC_GL_CURRENT_STATE->stencilFunc.front.ref = 1;
    CC_GL_CURRENT_STATE->stencilFunc.front.mask = 0;
    CC_GL_CURRENT_STATE->stencilFunc.back.func = GL_ALWAYS;
    CC_GL_CURRENT_STATE->stencilFunc.back.ref = 1;
    CC_GL_CURRENT_STATE->stencilFunc.back.mask = 0;
    
    CC_GL_STENCIL_FUNC(GL_ALWAYS, 1, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(front[1], 0, @"should be 0");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(back[1], 0, @"should be 0");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    CC_GL_STENCIL_FUNC_SEPARATE(GL_FRONT, GL_ALWAYS, 1, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(front[1], 1, @"should be 1");
    XCTAssertEqual(front[2], 0, @"should be 0");
    XCTAssertEqual(back[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(back[1], 0, @"should be 0");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    CC_GL_STENCIL_FUNC_SEPARATE(GL_BACK, GL_ALWAYS, 1, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(front[1], 0, @"should be 0");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(back[1], 1, @"should be 1");
    XCTAssertEqual(back[2], 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    
    CC_GL_CURRENT_STATE->stencilFunc.front.func = GL_ALWAYS;
    CC_GL_CURRENT_STATE->stencilFunc.front.ref = 1;
    CC_GL_CURRENT_STATE->stencilFunc.front.mask = 1;
    CC_GL_CURRENT_STATE->stencilFunc.back.func = GL_ALWAYS;
    CC_GL_CURRENT_STATE->stencilFunc.back.ref = 0;
    CC_GL_CURRENT_STATE->stencilFunc.back.mask = 0;
    
    CC_GL_STENCIL_FUNC_SEPARATE(GL_FRONT, GL_ALWAYS, 1, 1);
    CC_GL_STENCIL_FUNC_SEPARATE(GL_BACK, GL_ALWAYS, 0, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(front[1], 0, @"should be 0");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(back[1], 0, @"should be 0");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    GLenum face = GL_BACK;
    CC_GL_STENCIL_FUNC_SEPARATE_FACE(face, GL_ALWAYS, 1, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(front[1], 0, @"should be 0");
    XCTAssertEqual(front[2], 1, @"should be 1");
    XCTAssertEqual(back[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(back[1], 1, @"should be 1");
    XCTAssertEqual(back[2], 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_STENCIL_FUNC(GL_NEVER, 0, 1);
    face = GL_FRONT;
    CC_GL_STENCIL_FUNC_SEPARATE_FACE(face, GL_ALWAYS, 1, 0);
    
    glGetIntegerv(GL_STENCIL_FUNC, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_REF, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FUNC, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_REF, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(front[1], 1, @"should be 1");
    XCTAssertEqual(front[2], 0, @"should be 0");
    XCTAssertEqual(back[0], GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(back[1], 0, @"should be 0");
    XCTAssertEqual(back[2], 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.func, GL_ALWAYS, @"should be GL_ALWAYS");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.ref, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.func, GL_NEVER, @"should be GL_NEVER");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.ref, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilFunc.back.mask, 1, @"should be 1");
#endif
}

-(void) testStencilMaskMacro
{
#if CC_GL_STATE_STENCIL
    glStencilMask(0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_STENCIL_MASK(1);
    
    GLint front, back;
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 1, @"should be 1");
    XCTAssertEqual(back, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_MASK(0);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 0, @"should be 0");
    XCTAssertEqual(back, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_CURRENT_STATE->stencilMask.front.mask = 1;
    CC_GL_CURRENT_STATE->stencilMask.back.mask = 1;
    
    CC_GL_STENCIL_MASK(1);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 0, @"should be 0");
    XCTAssertEqual(back, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_MASK(0);
    CC_GL_STENCIL_MASK_SEPARATE(GL_FRONT, 1);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 1, @"should be 1");
    XCTAssertEqual(back, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 0, @"should be 0");
    
    
    
    CC_GL_STENCIL_MASK(0);
    CC_GL_STENCIL_MASK_SEPARATE(GL_BACK, 1);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 0, @"should be 0");
    XCTAssertEqual(back, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_MASK(0);
    
    CC_GL_CURRENT_STATE->stencilFunc.front.mask = 1;
    CC_GL_CURRENT_STATE->stencilFunc.back.mask = 0xff;
    
    CC_GL_STENCIL_MASK_SEPARATE(GL_FRONT, 1);
    CC_GL_STENCIL_MASK_SEPARATE(GL_BACK, 0xff);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 1, @"should be 1");
    XCTAssertEqual(back, 0xff, @"should be 0xff");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 0xff, @"should be 0xff");
    
    
    
    CC_GL_STENCIL_MASK(0);
    GLenum face = GL_BACK;
    CC_GL_STENCIL_MASK_SEPARATE_FACE(face, 1);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 0, @"should be 0");
    XCTAssertEqual(back, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 1, @"should be 1");
    
    
    
    CC_GL_STENCIL_MASK(0);
    face = GL_FRONT;
    CC_GL_STENCIL_MASK_SEPARATE_FACE(face, 1);
    
    glGetIntegerv(GL_STENCIL_WRITEMASK, &front); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back); CC_GL_CHECK();
    
    XCTAssertEqual(front, 1, @"should be 1");
    XCTAssertEqual(back, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.front.mask, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilMask.back.mask, 0, @"should be 0");
#endif
}

-(void) testStencilOpMacro
{
#if CC_GL_STATE_STENCIL
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_STENCIL_OP(GL_ZERO, GL_INCR, GL_DECR);
    
    GLint front[3], back[3];
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(front[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(front[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(back[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(back[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(back[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_KEEP, @"should be GL_KEEP");
    
    
    
    CC_GL_CURRENT_STATE->stencilOp.front.sfail = GL_ZERO;
    CC_GL_CURRENT_STATE->stencilOp.front.dpfail = GL_INCR;
    CC_GL_CURRENT_STATE->stencilOp.front.dppass = GL_DECR;
    CC_GL_CURRENT_STATE->stencilOp.back.sfail = GL_ZERO;
    CC_GL_CURRENT_STATE->stencilOp.back.dpfail = GL_INCR;
    CC_GL_CURRENT_STATE->stencilOp.back.dppass = GL_DECR;
    
    CC_GL_STENCIL_OP(GL_ZERO, GL_INCR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    CC_GL_STENCIL_OP_SEPARATE(GL_FRONT, GL_ZERO, GL_INCR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(front[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(front[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(back[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_KEEP, @"should be GL_KEEP");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    CC_GL_STENCIL_OP_SEPARATE(GL_BACK, GL_ZERO, GL_INCR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(back[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(back[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    
    CC_GL_CURRENT_STATE->stencilOp.front.sfail = GL_INCR;
    CC_GL_CURRENT_STATE->stencilOp.front.dpfail = GL_INCR;
    CC_GL_CURRENT_STATE->stencilOp.front.dppass = GL_INCR;
    CC_GL_CURRENT_STATE->stencilOp.back.sfail = GL_DECR;
    CC_GL_CURRENT_STATE->stencilOp.back.dpfail = GL_DECR;
    CC_GL_CURRENT_STATE->stencilOp.back.dppass = GL_DECR;
    
    CC_GL_STENCIL_OP_SEPARATE(GL_FRONT, GL_INCR, GL_INCR, GL_INCR);
    CC_GL_STENCIL_OP_SEPARATE(GL_BACK, GL_DECR, GL_DECR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    GLenum face = GL_BACK;
    CC_GL_STENCIL_OP_SEPARATE_FACE(face, GL_ZERO, GL_INCR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(front[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(back[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(back[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_DECR, @"should be GL_DECR");
    
    
    
    CC_GL_STENCIL_OP(GL_KEEP, GL_KEEP, GL_KEEP);
    face = GL_FRONT;
    CC_GL_STENCIL_OP_SEPARATE_FACE(face, GL_ZERO, GL_INCR, GL_DECR);
    
    glGetIntegerv(GL_STENCIL_FAIL, &front[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &front[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &front[2]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_FAIL, &back[0]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &back[1]); CC_GL_CHECK();
    glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &back[2]); CC_GL_CHECK();
    
    XCTAssertEqual(front[0], GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(front[1], GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(front[2], GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(back[0], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[1], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(back[2], GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.sfail, GL_ZERO, @"should be GL_ZERO");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dpfail, GL_INCR, @"should be GL_INCR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.front.dppass, GL_DECR, @"should be GL_DECR");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.sfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dpfail, GL_KEEP, @"should be GL_KEEP");
    XCTAssertEqual(CC_GL_CURRENT_STATE->stencilOp.back.dppass, GL_KEEP, @"should be GL_KEEP");
#endif
}

-(void) testTextureState
{
#if CC_GL_STATE_TEXTURE
#define TEST_TEXTURE_TARGET(target, index) { target, &State->bindTexture[index]._##target, #target }
    CCGLState *State = CCGLCurrentState;
    
    GLint Max;
#if !CC_GL_STATE_TEXTURE_MAX
    CC_GL_VERSION_ACTIVE(1_1, 1_5, 1_0, 1_1, glGetIntegerv(GL_MAX_TEXTURE_UNITS, &Max); CC_GL_CHECK());
    CC_GL_VERSION_ACTIVE(2_0, NA, 2_0, NA, glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &Max); CC_GL_CHECK());
#else
    Max = CC_GL_STATE_TEXTURE_MAX;
#endif
    
    for (size_t Active = 0; Active < Max; Active++)
    {
        const struct {
            GLenum target;
            const GLenum *state;
            char *name;
        } TextureTargets[] = {
#if CC_GL_STATE_TEXTURE_1D
            CC_GL_VERSION_ACTIVE(1_1, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_1D, Active),)
#endif
#if CC_GL_STATE_TEXTURE_2D
            CC_GL_VERSION_ACTIVE(1_1, NA, 1_0, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_2D, Active),)
#endif
#if CC_GL_STATE_TEXTURE_3D
            CC_GL_VERSION_ACTIVE(1_1, NA, 3_0, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_3D, Active),)
#endif
#if CC_GL_STATE_TEXTURE_1D_ARRAY
            CC_GL_VERSION_ACTIVE(3_0, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_1D_ARRAY, Active),)
#endif
#if CC_GL_STATE_TEXTURE_2D_ARRAY
            CC_GL_VERSION_ACTIVE(3_0, NA, 3_0, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_2D_ARRAY, Active),)
#endif
#if CC_GL_STATE_TEXTURE_RECTANGLE
#if GL_ARB_texture_rectangle
            TEST_TEXTURE_TARGET(GL_TEXTURE_RECTANGLE_ARB, Active),
#else
            CC_GL_VERSION_ACTIVE(3_0, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_RECTANGLE, Active),)
#endif
#endif
#if CC_GL_STATE_TEXTURE_CUBE_MAP
            CC_GL_VERSION_ACTIVE(1_3, NA, 2_0, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_CUBE_MAP, Active),)
#endif
#if CC_GL_STATE_TEXTURE_CUBE_MAP_ARRAY
            CC_GL_VERSION_ACTIVE(4_0, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_CUBE_MAP_ARRAY, Active),)
#endif
#if CC_GL_STATE_TEXTURE_BUFFER
            CC_GL_VERSION_ACTIVE(3_0, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_BUFFER, Active),)
#endif
#if CC_GL_STATE_TEXTURE_2D_MULTISAMPLE
            CC_GL_VERSION_ACTIVE(3_2, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_2D_MULTISAMPLE, Active),)
#endif
#if CC_GL_STATE_TEXTURE_2D_MULTISAMPLE_ARRAY
            CC_GL_VERSION_ACTIVE(3_2, NA, NA, NA, TEST_TEXTURE_TARGET(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, Active),)
#endif
        };
        const size_t Count = sizeof(TextureTargets) / sizeof(typeof(*TextureTargets));
        
        glActiveTexture((GLenum)(GL_TEXTURE0 + Active)); CC_GL_CHECK();
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            glBindTexture(TextureTargets[Loop].target, 0); CC_GL_CHECK();
        }
        
        CCGLStateInitializeWithCurrent(State);
        
        XCTAssertEqual(State->activeTexture.texture, (GLenum)(GL_TEXTURE0 + Active), @"should be GL_TEXTURE%zu", Active);
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            XCTAssertEqual(*TextureTargets[Loop].state, 0, @"%s (in GL_TEXTURE%zu) should be %u", TextureTargets[Loop].name, Active, 0);
        }
        
        
        GLuint Textures[Count];
        
        glGenTextures(Count, Textures); CC_GL_CHECK();
        
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            glBindTexture(TextureTargets[Loop].target, Textures[Loop]); CC_GL_CHECK();
        }
        glActiveTexture(GL_TEXTURE0); CC_GL_CHECK();
        
        CCGLStateInitializeWithCurrent(State);
        
        XCTAssertEqual(State->activeTexture.texture, GL_TEXTURE0, @"should be GL_TEXTURE0");
        for (size_t Loop = 0; Loop < Count; Loop++)
        {
            XCTAssertEqual(*TextureTargets[Loop].state, Textures[Loop], @"%s (in GL_TEXTURE%zu) should be %u", TextureTargets[Loop].name, Active, Textures[Loop]);
        }
        
        glDeleteTextures(Count, Textures); CC_GL_CHECK();
    }
#endif
}

-(void) testActiveTextureMacro
{
#if CC_GL_STATE_TEXTURE
    glActiveTexture(GL_TEXTURE0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_ACTIVE_TEXTURE(GL_TEXTURE1);
    GLenum Active;
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&Active); CC_GL_CHECK();
    XCTAssertEqual(Active, GL_TEXTURE1, @"ActiveTexture should be GL_TEXTURE1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->activeTexture.texture, GL_TEXTURE1, @"State should be GL_TEXTURE1");
    
    
    
    CC_GL_ACTIVE_TEXTURE(GL_TEXTURE0);
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&Active); CC_GL_CHECK();
    XCTAssertEqual(Active, GL_TEXTURE0, @"ActiveTexture should be GL_TEXTURE0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->activeTexture.texture, GL_TEXTURE0, @"State should be GL_TEXTURE0");
    
    
    
    CC_GL_CURRENT_STATE->activeTexture.texture = GL_TEXTURE1;
    
    CC_GL_ACTIVE_TEXTURE(GL_TEXTURE1);
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&Active); CC_GL_CHECK();
    XCTAssertEqual(Active, GL_TEXTURE0, @"ActiveTexture should be GL_TEXTURE0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->activeTexture.texture, GL_TEXTURE1, @"State should be GL_TEXTURE1");
#endif
}

-(void) testBindTextureMacro
{
#if CC_GL_STATE_TEXTURE && CC_GL_STATE_TEXTURE_1D

#undef TEST_TEXTURE_TARGET
#define TEST_TEXTURE_TARGET(target, getTarget) \
glGetIntegerv(getTarget, (GLint*)&BoundTexture); CC_GL_CHECK(); \
XCTAssertEqual(BoundTexture, CurrentTexture, @"Should be bound to the texture"); \
XCTAssertEqual(CC_GL_CURRENT_STATE->bindTexture[Index - GL_TEXTURE0]._##target, CurrentTexture, @"State should be the texture");
    
    GLuint CurrentTexture = 0, Index = GL_TEXTURE0;
    
    glActiveTexture(Index); CC_GL_CHECK();
    glBindTexture(GL_TEXTURE_1D, CurrentTexture); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    GLuint Texture;
    glGenTextures(1, &Texture); CC_GL_CHECK();
    
    CurrentTexture = Texture;
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    
    GLuint BoundTexture;
    TEST_TEXTURE_TARGET(GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D);
    
    
    
    CurrentTexture = 0;
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    TEST_TEXTURE_TARGET(GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D);
    
    
    
    Index = GL_TEXTURE1;
    CC_GL_ACTIVE_TEXTURE(Index);
    CurrentTexture = Texture;
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    TEST_TEXTURE_TARGET(GL_TEXTURE_1D, GL_TEXTURE_BINDING_1D);
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindTexture[0]._GL_TEXTURE_1D, 0, @"Should be unchanged");
    
    
    
    CC_GL_CURRENT_STATE->bindTexture[Index - GL_TEXTURE0]._GL_TEXTURE_1D = 0;
    
    CurrentTexture = 0;
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    glGetIntegerv(GL_TEXTURE_BINDING_1D, (GLint*)&BoundTexture); CC_GL_CHECK();
    XCTAssertEqual(BoundTexture, Texture, @"Should be unchanged");
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindTexture[Index - GL_TEXTURE0]._GL_TEXTURE_1D, CurrentTexture, @"State should be the texture");
    
    
    
    GLenum Target = GL_TEXTURE_1D;
    CurrentTexture = Texture;
    CC_GL_BIND_TEXTURE_TARGET(Target, CurrentTexture);
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    
    
    
    CurrentTexture = 0;
    CC_GL_BIND_TEXTURE_TARGET(Target, CurrentTexture);
    CC_GL_BIND_TEXTURE(GL_TEXTURE_1D, CurrentTexture);
    
    glDeleteTextures(1, &Texture); CC_GL_CHECK();
#endif
}

-(void) testVertexArrayObjectState
{
#if CC_GL_STATE_VERTEX_ARRAY_OBJECT
    CCGLState *State = CCGLCurrentState;
    
    glBindVertexArray(0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->bindVertexArray.array, 0, @"should be 0");
    
    
    GLuint vao;
    glGenVertexArrays(1, &vao); CC_GL_CHECK();
    glBindVertexArray(vao); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->bindVertexArray.array, vao, @"should be %u", vao);
    
    glDeleteVertexArrays(1, &vao); CC_GL_CHECK();
#endif
}

-(void) testBindVertexArrayMacro
{
#if CC_GL_STATE_VERTEX_ARRAY_OBJECT
    glBindVertexArray(0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    GLuint vao;
    glGenVertexArrays(1, &vao); CC_GL_CHECK();
    CC_GL_BIND_VERTEX_ARRAY(vao);
    
    GLuint BoundVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&BoundVAO); CC_GL_CHECK();
    XCTAssertEqual(BoundVAO, vao, @"should be %u", vao);
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindVertexArray.array, vao, @"should be %u", vao);
    
    
    
    CC_GL_BIND_VERTEX_ARRAY(0);
    
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&BoundVAO); CC_GL_CHECK();
    XCTAssertEqual(BoundVAO, 0, @"should be %u", 0);
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindVertexArray.array, 0, @"should be %u", 0);
    
    
    
    CC_GL_CURRENT_STATE->bindVertexArray.array = vao;
    CC_GL_BIND_VERTEX_ARRAY(vao);
    
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&BoundVAO); CC_GL_CHECK();
    XCTAssertEqual(BoundVAO, 0, @"should be %u", 0);
    XCTAssertEqual(CC_GL_CURRENT_STATE->bindVertexArray.array, vao, @"should be %u", vao);
    
    glDeleteVertexArrays(1, &vao); CC_GL_CHECK();
#endif
}

-(void) testViewportState
{
#if CC_GL_STATE_VIEWPORT
    CCGLState *State = CCGLCurrentState;
    
    glViewport(0, 0, 0, 0); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->viewport.x, 0, @"should be 0");
    XCTAssertEqual(State->viewport.y, 0, @"should be 0");
    XCTAssertEqual(State->viewport.width, 0, @"should be 0");
    XCTAssertEqual(State->viewport.height, 0, @"should be 0");
    
    
    glViewport(1, 2, 3, 4); CC_GL_CHECK();
    
    CCGLStateInitializeWithCurrent(State);
    
    XCTAssertEqual(State->viewport.x, 1, @"should be 1");
    XCTAssertEqual(State->viewport.y, 2, @"should be 2");
    XCTAssertEqual(State->viewport.width, 3, @"should be 3");
    XCTAssertEqual(State->viewport.height, 4, @"should be 4");
#endif
}

-(void) testViewportMacro
{
#if CC_GL_STATE_VIEWPORT
    glViewport(0, 0, 0, 0); CC_GL_CHECK();
    CCGLStateInitializeWithCurrent(CCGLCurrentState);
    
    CC_GL_VIEWPORT(1, 2, 3, 4);
    
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport); CC_GL_CHECK();
    
    XCTAssertEqual(Viewport[0], 1, @"should be 1");
    XCTAssertEqual(Viewport[1], 2, @"should be 2");
    XCTAssertEqual(Viewport[2], 3, @"should be 3");
    XCTAssertEqual(Viewport[3], 4, @"should be 4");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.x, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.y, 2, @"should be 2");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.width, 3, @"should be 3");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.height, 4, @"should be 4");
    
    
    
    CC_GL_VIEWPORT(0, 0, 0, 0);
    
    glGetIntegerv(GL_VIEWPORT, Viewport); CC_GL_CHECK();
    
    XCTAssertEqual(Viewport[0], 0, @"should be 0");
    XCTAssertEqual(Viewport[1], 0, @"should be 0");
    XCTAssertEqual(Viewport[2], 0, @"should be 0");
    XCTAssertEqual(Viewport[3], 0, @"should be 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.x, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.y, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.width, 0, @"should be 0");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.height, 0, @"should be 0");
    
    
    
    CC_GL_CURRENT_STATE->viewport.x = 1;
    CC_GL_CURRENT_STATE->viewport.y = 2;
    CC_GL_CURRENT_STATE->viewport.width = 3;
    CC_GL_CURRENT_STATE->viewport.height = 4;
    CC_GL_VIEWPORT(1, 2, 3, 4);
    
    glGetIntegerv(GL_VIEWPORT, Viewport); CC_GL_CHECK();
    
    XCTAssertEqual(Viewport[0], 0, @"should be 0");
    XCTAssertEqual(Viewport[1], 0, @"should be 0");
    XCTAssertEqual(Viewport[2], 0, @"should be 0");
    XCTAssertEqual(Viewport[3], 0, @"should be 0");
    
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.x, 1, @"should be 1");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.y, 2, @"should be 2");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.width, 3, @"should be 3");
    XCTAssertEqual(CC_GL_CURRENT_STATE->viewport.height, 4, @"should be 4");
#endif
}

@end

#endif
