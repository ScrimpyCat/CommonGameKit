/*
 *  Copyright (c) 2016, Stefan Johnson
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

#import <XCTest/XCTest.h>
#import "ProjectExpressions.h"
#import "Configuration.h"

@interface ExpressionProjectTests : XCTestCase

@end

@implementation ExpressionProjectTests

-(void) testProject
{
    CCExpression Expression = CCExpressionCreateFromSource("(game \"Test Game\""
                                                           "    (default-resolution: 640 480)\n"
                                                           "    (default-fullscreen: #f)\n"
                                                           "\n"
                                                           "    (dir-fonts: \"font/\")\n"
                                                           "    (dir-levels: \"logic/levels/\")\n"
                                                           "    (dir-rules: \"logic/rules/\")\n"
                                                           "    (dir-textures: \"graphics/textures/\")\n"
                                                           "    (dir-shaders: \"graphics/shaders/\")\n"
                                                           "    (dir-sounds: \"audio/\")\n"
                                                           "    (dir-layouts: \"ui/\")\n"
                                                           "    (dir-entities: \"logic/entities/\")\n"
                                                           "    (dir-logs: \"logs/\")\n"
                                                           "    (dir-tmp: \"tmp/\")\n"
                                                           ")\n");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCProjectExpressionValueTypeGameConfig, @"Should be a project config");
    XCTAssertTrue(!strcmp(((CCEngineConfig*)CCExpressionGetData(Result))->title, "Test Game"), @"Should be initialized");
    XCTAssertEqual(((CCEngineConfig*)CCExpressionGetData(Result))->window.fullscreen, FALSE, @"Should be initialized");
    XCTAssertEqual(((CCEngineConfig*)CCExpressionGetData(Result))->window.width, 640, @"Should be initialized");
    XCTAssertEqual(((CCEngineConfig*)CCExpressionGetData(Result))->window.height, 480, @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.fonts, 0)), "font/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.levels, 0)), "logic/levels/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.rules, 0)), "logic/rules/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.textures, 0)), "graphics/textures/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.shaders, 0)), "graphics/shaders/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.sounds, 0)), "audio/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.layouts, 0)), "ui/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(*(FSPath*)CCOrderedCollectionGetElementAtIndex(((CCEngineConfig*)CCExpressionGetData(Result))->directory.entities, 0)), "logic/entities/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(((CCEngineConfig*)CCExpressionGetData(Result))->directory.logs), "logs/"), @"Should be initialized");
    XCTAssertTrue(!strcmp(FSPathGetPathString(((CCEngineConfig*)CCExpressionGetData(Result))->directory.tmp), "tmp/"), @"Should be initialized");
    
    CCExpressionDestroy(Expression);
}

@end
