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

@interface ExpressionControlFlowTests : CCTestCase

@end

@implementation ExpressionControlFlowTests

-(void) testConditionals
{
    CCExpression Expression = CCExpressionCreateFromSource("(if 0 10 20)");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 20, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(if 1 10 20)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 10, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 0) (if .x (.x! (+ .x 10)) (.x! (+ .x 20))))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 20, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 1) (if .x (.x! (+ .x 10)) (.x! (+ .x 20))))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 11, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
}

-(void) testLoop
{
    CCExpression Expression = CCExpressionCreateFromSource("(loop \"@var\" (1 2 3) @var)");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should have 3 values");
    
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), CCExpressionValueTypeInteger, @"Should be an integer");
    
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), 1, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), 2, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), 3, @"Should be the value");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(loop \"@var\" (1 2 3) (+ @var @var))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should have 3 values");
    
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), CCExpressionValueTypeInteger, @"Should be an integer");
    
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), 2, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), 4, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), 6, @"Should be the value");
    
    CCExpressionDestroy(Expression);
    
    
    Expression = CCExpressionCreateFromSource("(loop \"@var\" (1 2 3) 0)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 3, @"Should have 3 values");
    
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), CCExpressionValueTypeInteger, @"Should be an integer");
    
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), 0, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), 0, @"Should be the value");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), 0, @"Should be the value");
    
    CCExpressionDestroy(Expression);
}

@end
