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

#import "ExpressionEvaluatorTests.h"

@interface ExpressionBitwiseTests : ExpressionEvaluatorTests

@end

@implementation ExpressionBitwiseTests

-(void) testNot
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(not)",            CCExpressionValueTypeExpression },
        { "(not test)",          CCExpressionValueTypeExpression },
        { "(not 1.5)",         CCExpressionValueTypeExpression },
        { "(not \"test\")",        CCExpressionValueTypeExpression },
        { "(not ())",       CCExpressionValueTypeExpression },
        { "(= 0 (not 1))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 1 (not 0))",       CCExpressionValueTypeInteger,   .integer = TRUE }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testAnd
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(and)",            CCExpressionValueTypeExpression },
        { "(and test)",          CCExpressionValueTypeExpression },
        { "(and 1.5)",         CCExpressionValueTypeExpression },
        { "(and \"test\")",        CCExpressionValueTypeExpression },
        { "(= () (and ()))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 1 (and 1))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 2) (and (1 2)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 1 (and 3 5))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 1 4) (and (5 3 5) (3 5 4)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 1 4 1) (and (5 3 5) (3 5 4 1)))",       CCExpressionValueTypeInteger,   .integer = TRUE }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testOr
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(or)",            CCExpressionValueTypeExpression },
        { "(or test)",          CCExpressionValueTypeExpression },
        { "(or 1.5)",         CCExpressionValueTypeExpression },
        { "(or \"test\")",        CCExpressionValueTypeExpression },
        { "(= () (or ()))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 1 (or 1))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 2) (or (1 2)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 7 (or 3 5))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (7 7 5) (or (5 3 5) (3 5 4)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (7 7 5 1) (or (5 3 5) (3 5 4 1)))",       CCExpressionValueTypeInteger,   .integer = TRUE }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testXor
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(xor)",            CCExpressionValueTypeExpression },
        { "(xor test)",          CCExpressionValueTypeExpression },
        { "(xor 1.5)",         CCExpressionValueTypeExpression },
        { "(xor \"test\")",        CCExpressionValueTypeExpression },
        { "(= () (xor ()))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 1 (xor 1))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 2) (xor (1 2)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 6 (xor 3 5))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (6 6 1) (xor (5 3 5) (3 5 4)))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (6 6 1 1) (xor (5 3 5) (3 5 4 1)))",       CCExpressionValueTypeInteger,   .integer = TRUE }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

@end
