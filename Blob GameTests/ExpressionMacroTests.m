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

@interface ExpressionMacroTests : XCTestCase

@end

@implementation ExpressionMacroTests

-(void) testQuoting
{
    CCExpression Expression = CCExpressionCreateFromSource("(quote (+ 2 4))");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeExpression, @"Should be an expression");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should have 3 expressions");
    
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CCExpressionValueTypeAtom, @"Should be an atom");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), CCExpressionValueTypeInteger, @"Should be an atom");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), CCExpressionValueTypeInteger, @"Should be an atom");
    
    XCTAssertTrue(CCStringEqual(CCExpressionGetAtom(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CC_STRING("+")), @"Should be the + function");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), 2, @"Should be 2");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), 4, @"Should be 4");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(unquote (quote (+ 2 4)))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 6, @"Should be 6");
    
    CCExpressionDestroy(Expression);
}

@end
