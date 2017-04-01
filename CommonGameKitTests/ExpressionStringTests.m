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

#import "CCTestCase.h"
#import "Expression.h"

@interface ExpressionStringTests : CCTestCase

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

-(void) testJoin
{
    CCExpression Expression = CCExpressionCreateFromSource("(cat (\"1\" \"2\" \"3\"))");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("123")), @"Should be the correct string");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(cat (\"1\" \"2\" \"3\") \",\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("1,2,3")), @"Should be the correct string");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(cat (\"example!!!!!!\" \"!\"))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("example!!!!!!!")), @"Should be the correct string");
    
    CCExpressionDestroy(Expression);
}

-(void) testLength
{
    CCExpression Expression = CCExpressionCreateFromSource("(length \"123\")");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 3, @"Should be the correct length");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(length \"1234567890abcdef\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 16, @"Should be the correct length");
    
    CCExpressionDestroy(Expression);


    Expression = CCExpressionCreateFromSource("(length \"\")");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should be the correct length");
    
    CCExpressionDestroy(Expression);
}

-(void) testFormat
{
    CCExpression Expression = CCExpressionCreateFromSource("(format :bin 5)");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("101")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin -5))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("11111111111111111111111111111011")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin 1.0))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("111111100000000000000000000000")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin \"foo\"))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("foo")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec 5)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec -5))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("-5")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec 1.0))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("1")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec \"foo\"))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("foo")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex 5)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex -5))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("fffffffb")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex 1.0))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("3f800000")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex \"foo\"))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("foo")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt 5)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5.0")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt -5))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("-5.0")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt 1.0))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("1.0")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt \"foo\"))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("foo")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin 5 (compact: #f))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("00000000000000000000000000000101")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec 5 (compact: #f))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex 5 (compact: #f))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("00000005")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt 5 (compact: #f))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5.000000")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt 5.125 (compact: #f) (precision: 4))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5.1250")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :flt 5.125 (compact: #t) (precision: 4))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("5.125")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :hex 5 (compact: #f) (precision: 4))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("0005")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :dec 5 (compact: #f) (precision: 4))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("0005")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin 5 (compact: #f) (precision: 4))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("0101")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin 5 (compact: #t))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("101")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin (5 0 2 7) (separator: \",\") (compact: #t))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("101,0,10,111")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(format :bin (5 0 2 7) (separator: \",\") (prefix: \"(\") (suffix: \")\") (compact: #t))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeString, @"Should be a string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Result), CC_STRING("(101),(0),(10),(111)")), @"Should be formatted correctly");
    
    CCExpressionDestroy(Expression);
}

@end
