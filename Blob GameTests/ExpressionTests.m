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
#import "ExpressionEvaluator.h"

@interface ExpressionTests : XCTestCase

@end

@implementation ExpressionTests

-(void) testParsing
{
    CCExpression Expression = CCExpressionCreateFromSource("()");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 0, @"Should return an empty list");
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(() (1) (1 2))");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with three expressions");
    
    CCExpression Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expr)), 0, @"Should be empty");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expr)), 1, @"Should have one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expr)), 2, @"Should have two expression");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(1)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Expr), 1, @"Should be 1");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(-13)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Expr), -13, @"Should be -13");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(1 2 3)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with three expressions");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Expr), 1, @"Should be 1");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Expr), 2, @"Should be 2");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Expr), 3, @"Should be 3");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(13.0)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeFloat, @"Should be a float");
    XCTAssertEqual(CCExpressionGetFloat(Expr), 13.0f, @"Should be 13.0");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(-13.0)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeFloat, @"Should be a float");
    XCTAssertEqual(CCExpressionGetFloat(Expr), -13.0f, @"Should be -13.0");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(1.0 2.0 3.0)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with three expressions");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeFloat, @"Should be a float");
    XCTAssertEqual(CCExpressionGetFloat(Expr), 1.0f, @"Should be 1.0");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeFloat, @"Should be a float");
    XCTAssertEqual(CCExpressionGetFloat(Expr), 2.0f, @"Should be 2.0");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeFloat, @"Should be a float");
    XCTAssertEqual(CCExpressionGetFloat(Expr), 3.0f, @"Should be 3.0");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(atom)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(Expr), CC_STRING("atom")), @"Should be 'atom'");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(atom-something)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(Expr), CC_STRING("atom-something")), @"Should be 'atom-something'");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(atom-1 atom-2 atom-3)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with three expressions");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(Expr), CC_STRING("atom-1")), @"Should be 'atom-1'");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(Expr), CC_STRING("atom-2")), @"Should be 'atom-2'");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(Expr), CC_STRING("atom-3")), @"Should be 'atom-3'");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(\"string\")");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string")), @"Should be \"string\"");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(\"string-something\")");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string-something")), @"Should be \"string-something\"");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(\"string\\\"-\\\"some\\\\thing\")");CCExpressionPrint(Expression);
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string\"-\"some\\thing")), @"Should be \"string\"-\"some\\thing\"");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(\"string;something\")");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 1, @"Should return a list with one expression");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string;something")), @"Should be \"string;something\"");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(\"string-1\" \"string-2\" \"string-3\")");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with three expressions");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 0);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string-1")), @"Should be \"string-1\"");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string-2")), @"Should be \"string-2\"");
    
    Expr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2);
    XCTAssertEqual(CCExpressionGetType(Expr), CCExpressionValueTypeString, @"Should be an string");
    XCTAssertTrue(CCStringEqual(CCExpressionGetString(Expr), CC_STRING("string-3")), @"Should be \"string-3\"");
    
    CCExpressionDestroy(Expression);
}

-(void) testEvaluation
{
    CCExpression Expression = CCExpressionCreateFromSource("(+ 2 4 10)");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 4, @"Should return a list with 4 expressions");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 16, @"Should be 16");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(+ (+ 2 8) (* 4 10))");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with 3 expressions");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 50, @"Should be 50");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(+ (+ missing 8) (* 4 10))");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with 3 expressions");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should be a list with 3 expressions");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(test-add (+ 2 8) (* 4 10))");
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 3, @"Should return a list with 3 expressions");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should be a list with 3 expressions");
    
    CCExpressionDestroy(Expression);
}

-(void) testState
{
    CCExpression Expression = CCExpressionCreateFromSource("(+ value 4 10)");
    CCExpressionCreateState(Expression, CC_STRING("value"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetType(Expression), CCExpressionValueTypeList, @"Should return a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Expression)), 4, @"Should return a list with 4 expressions");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 16, @"Should be 16");
    
    
    CCExpressionSetState(Expression, CC_STRING("value"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, 20.0f), FALSE);
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeFloat, @"Should be an float");
    XCTAssertEqual(CCExpressionGetFloat(Result), 34.0f, @"Should be 34.0");
    
    
    Result = CCExpressionGetState(Expression, CC_STRING("value"));
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeFloat, @"Should be an float");
    XCTAssertEqual(CCExpressionGetFloat(Result), 20.0f, @"Should be 20.0");
    
    
    CCExpression CopiedExpression = CCExpressionCopy(Expression);
    
    Result = CCExpressionGetState(CopiedExpression, CC_STRING("value"));
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeFloat, @"Should be an float");
    XCTAssertEqual(CCExpressionGetFloat(Result), 20.0f, @"Should be 20.0");
    
    
    CCExpressionSetState(Expression, CC_STRING("value"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 5), FALSE);
    
    Result = CCExpressionGetState(Expression, CC_STRING("value"));
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 5, @"Should be 5");
    
    Result = CCExpressionGetState(CopiedExpression, CC_STRING("value"));
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeFloat, @"Should be an float");
    XCTAssertEqual(CCExpressionGetFloat(Result), 20.0f, @"Should be 20.0");
    
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 19, @"Should be 19");
    
    Result = CCExpressionEvaluate(CopiedExpression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeFloat, @"Should be an float");
    XCTAssertEqual(CCExpressionGetFloat(Result), 34.0f, @"Should be 34.0");
    
    CCExpressionDestroy(CopiedExpression);
    CCExpressionDestroy(Expression);
}

static int TestValueDestroyed = 0;
static void CCExpressionTestValueDestructor(void *Data)
{
    TestValueDestroyed++;
}

static int TestValueCopied = 0;
static CCExpression CCExpressionTestValueCopy(CCExpression Value)
{
    TestValueCopied++;
    return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, INT_MAX, CCExpressionGetData(Value), CCExpressionTestValueCopy, CCExpressionTestValueDestructor);
}

-(void) testCustomType
{
    CCExpression Expression = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, INT_MAX, &TestValueDestroyed, CCExpressionTestValueCopy, CCExpressionTestValueDestructor);
    
    XCTAssertEqual(CCExpressionGetType(Expression), INT_MAX, @"Should have the custom type");
    XCTAssertEqual(CCExpressionGetData(Expression), &TestValueDestroyed, @"Should have the custom data");
    
    
    CCExpression CopiedExpression = CCExpressionCopy(Expression);
    
    XCTAssertEqual(CCExpressionGetType(CopiedExpression), INT_MAX, @"Should have the custom type");
    XCTAssertEqual(CCExpressionGetData(CopiedExpression), &TestValueDestroyed, @"Should have the custom data");
    
    CCExpressionDestroy(CopiedExpression);
    
    
    CopiedExpression = CCExpressionCopy(Expression);
    CCExpressionDestroy(Expression);
    
    CCExpressionChangeOwnership(CopiedExpression, NULL, NULL);
    Expression = CCExpressionCopy(CopiedExpression);
    
    XCTAssertEqual(CCExpressionGetType(Expression), INT_MAX, @"Should have the custom type");
    XCTAssertEqual(CCExpressionGetData(Expression), &TestValueDestroyed, @"Should have the custom data");
    
    CCExpressionDestroy(CopiedExpression);
    CCExpressionDestroy(Expression);
    
    
    XCTAssertEqual(TestValueDestroyed, 2, @"Should have the custom type");
    XCTAssertEqual(TestValueCopied, 2, @"Should have the custom data");
}

CCExpression CCExpressionTestIncEvaluator(CCExpression Expression)
{
    return CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1 + CCExpressionGetInteger(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1))));
}

-(void) testFunction
{
    CCExpressionEvaluatorRegister(CC_STRING("test-function-integer-inc"), CCExpressionTestIncEvaluator);
    
    CCExpression Expression = CCExpressionCreateFromSource("(test-function-integer-inc 10)");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 11, @"Should be 11");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(test-function-integer-inc (test-function-integer-inc (test-function-integer-inc 10)))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 13, @"Should be 13");
    
    CCExpressionDestroy(Expression);
}

@end
