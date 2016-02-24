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
#import "Expression.h"

@interface ExpressionStringTests : XCTestCase

@end

@implementation ExpressionStringTests

-(void) testPrefix
{
    CCExpression Expression = CCExpressionCreateFromSource("(prefix \"1\" \"12345\")");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(prefix \"12\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(prefix \"12345\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(prefix \"5\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should be false");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(prefix \"45\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should be false");
    
    CCExpressionDestroy(Expression);
}

-(void) testSuffix
{
    CCExpression Expression = CCExpressionCreateFromSource("(suffix \"1\" \"12345\")");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should be false");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(suffix \"12\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should be false");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(suffix \"12345\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(suffix \"5\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(suffix \"45\" \"12345\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be true");
    
    CCExpressionDestroy(Expression);
}

-(void) testFilename
{
    CCExpression Expression = CCExpressionCreateFromSource("(filename \"folder/folder/file.txt\")");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("file.txt")), @"Should be filename");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(filename \"file.txt\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("file.txt")), @"Should be filename");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(filename \"folder/folder/\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeExpression, @"Should be an expression");
    
    CCExpressionDestroy(Expression);
}

-(void) testReplace
{
    CCExpression Expression = CCExpressionCreateFromSource("(replace \" \" \"-\" \"1 2 3 4 5\")");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("1-2-3-4-5")), @"Should be the correct string");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(replace \"11\" \"123\" \"1111\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("123123")), @"Should be the correct string");
    
    CCExpressionDestroy(Expression);
}

@end
