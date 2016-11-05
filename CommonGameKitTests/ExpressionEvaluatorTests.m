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

#import "ExpressionEvaluatorTests.h"

const int32_t i = -5;
const float f = -2.5f;

@implementation ExpressionEvaluatorTests

-(void) assert: (ExpressionResult*)test count: (size_t)count
{
    for (size_t Loop = 0; Loop < count; Loop++)
    {
        CCExpression Expression = CCExpressionCreateFromSource(test[Loop].source);
        CCExpressionCreateState(Expression, CC_STRING(".i"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, i), FALSE, NULL, FALSE);
        CCExpressionCreateState(Expression, CC_STRING(".f"), CCExpressionCreateFloat(CC_STD_ALLOCATOR, f), FALSE, NULL, FALSE);
        
        CCExpression Result = CCExpressionEvaluate(Expression);
        XCTAssertEqual(CCExpressionGetType(Result), test[Loop].type, @"Should be the correct type: %s", test[Loop].source);
        if (test[Loop].type == CCExpressionValueTypeInteger) XCTAssertEqual(CCExpressionGetInteger(Result), test[Loop].integer, @"Should be %d: %s", test[Loop].integer, test[Loop].source);
        else if (test[Loop].type == CCExpressionValueTypeFloat) XCTAssertEqual(CCExpressionGetFloat(Result), test[Loop].real, @"Should be %f: %s", test[Loop].real, test[Loop].source);
        else if (test[Loop].type == CCExpressionValueTypeExpression) XCTAssertEqual(Result, Expression, @"Should be the expression: %s", test[Loop].source);
        
        CCExpressionDestroy(Expression);
    }
}

@end
