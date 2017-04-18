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

@interface ExpressionStateTests : CCTestCase

@end

@implementation ExpressionStateTests

-(void) testState
{
    CCExpression Expression = CCExpressionCreateFromSource("(begin (state! \".x\") (state! \".y\" 15) (state! \".ref-x\" (quote (+ .x 10))))");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeNull, @"Should be a null expression");
    
    XCTAssertEqual(CCExpressionGetState(Expression, CC_STRING(".x")), NULL, @"Should be uninitialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".y"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".ref-x"))), CCExpressionValueTypeExpression, @"Should be initialized");
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 15, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".y"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".ref-x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".x"))), 2, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 15, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".ref-x"))), 12, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\") (state! \".y\" 15) (state! \".ref-x\" (quote (+ .x 10))) (.x! (quote (* .y 2))) (.x .y .ref-x (+ .x .y .ref-x)))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeList, @"Should be a list");
    XCTAssertNotEqual(Result, Expression, @"Should not be the expression");
    XCTAssertEqual(CCCollectionGetCount(CCExpressionGetList(Result)), 4, @"Should have 4 values");
    
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetType(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 3)), CCExpressionValueTypeInteger, @"Should be an integer");
    
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 0)), 30, @"Should be 30");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 1)), 15, @"Should be 15");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 2)), 40, @"Should be 40");
    XCTAssertEqual(CCExpressionGetInteger(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Result), 3)), 85, @"Should be 85");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 0) (.x! (+ 1 .x)))");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should be 1");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 2, @"Should be 2");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 3, @"Should be 3");
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".x"))), 3, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" (quote (+ .y 1)) (invalidate: #f)) (state! \".y\" 10) .x)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 11, @"Should be 11");
    
    CCExpressionSetState(Expression, CC_STRING(".y"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1), FALSE);
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 11, @"Should be 11. Should not re-evaluate.");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateFromSource("(+ .y 2)"), FALSE);
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 3, @"Should be 3");
    
    CCExpressionSetState(Expression, CC_STRING(".y"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 10), FALSE);
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 3, @"Should be 3. Should not re-evaluate.");
    
    CCExpressionDestroy(Expression);
}

-(void) testStateInvalidation
{
    CCExpression Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 1) (state! \".y\" (quote .x)) .y)");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should return 1");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 2, @"Should be initialized");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 2, @"Should return 2");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 1) (state! \".y\" (quote .x) (invalidate: (quote (< .x 2)))) .y)");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should return 1");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 1, @"Should be initialized");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 1, @"Should return 1");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 0), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 0, @"Should be initialized");
    
    Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 0, @"Should return 0");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreate(CC_STD_ALLOCATOR, CCExpressionValueTypeExpression);
    CCExpression State = CCExpressionCreateFromSource("(+ .x 10)");
    
    CCExpressionCreateState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 1), FALSE, NULL, FALSE);
    CCExpressionCreateState(Expression, CC_STRING(".y"), State, TRUE, CCExpressionCreateFromSource("(< .x 2)"), FALSE);
    CCExpressionCreateState(Expression, CC_STRING(".z"), State, FALSE, CCExpressionCreateFromSource("(< .x 3)"), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 11, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 12, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 3), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 12, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 0), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 10, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 10, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
    
    
    
    Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 1) (state! \".state\" (quote (quote (+ .x 10)))) (state! \".y\" .state (invalidate: (quote (< .x 2)))) (state! \".z\" .state (invalidate: (quote (< .x 3)))))");
    CCExpressionEvaluate(Expression);

    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 11, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 2), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 12, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 3), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 11, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 12, @"Should be initialized");
    
    CCExpressionSetState(Expression, CC_STRING(".x"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 0), FALSE);
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".y"))), 10, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".z"))), 10, @"Should be initialized");

    
    CCExpressionDestroy(Expression);
}

-(void) testEnum
{
    CCExpression Expression = CCExpressionCreateFromSource("(begin (enum! \"&x\" \"&y\" \"&z\" (\"&a\" 20) \"&b\"))");
    
    CCExpressionEvaluate(Expression);
    
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING("&x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING("&y"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING("&z"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING("&a"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING("&b"))), CCExpressionValueTypeInteger, @"Should be initialized");
    
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING("&x"))), 0, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING("&y"))), 1, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING("&z"))), 2, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING("&a"))), 20, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING("&b"))), 21, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
}

-(void) testSuper
{
    CCExpression Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 10) (begin (state! \".x\" 20) (.x! 1) (super (.x! 2)) (.x! (super (+ 100 .x)))))");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 102, @"Should be 102");
    
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".x"))), 2, @"Should be initialized");
    
    CCExpression SubExpression = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(SubExpression, CC_STRING(".x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(SubExpression, CC_STRING(".x"))), 102, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
}

-(void) testStrictSuper
{
    CCExpression Expression = CCExpressionCreateFromSource("(begin (state! \".x\" 10) (begin (state! \".x\" 20) (.x! 1) (strict-super (.x! 2)) (.x! (strict-super (+ 100 .x)))))");
    
    CCExpression Result = CCExpressionEvaluate(Expression);
    XCTAssertEqual(CCExpressionGetType(Result), CCExpressionValueTypeInteger, @"Should be an integer");
    XCTAssertEqual(CCExpressionGetInteger(Result), 101, @"Should be 101");
    
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(Expression, CC_STRING(".x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(Expression, CC_STRING(".x"))), 2, @"Should be initialized");
    
    CCExpression SubExpression = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1);
    XCTAssertEqual(CCExpressionGetType(CCExpressionGetState(SubExpression, CC_STRING(".x"))), CCExpressionValueTypeInteger, @"Should be initialized");
    XCTAssertEqual(CCExpressionGetInteger(CCExpressionGetState(SubExpression, CC_STRING(".x"))), 101, @"Should be initialized");
    
    CCExpressionDestroy(Expression);
}

@end
