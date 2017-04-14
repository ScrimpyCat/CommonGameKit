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

@interface ExpressionEqualityTests : ExpressionEvaluatorTests

@end

@implementation ExpressionEqualityTests

-(void) testEqual
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(=)",            CCExpressionValueTypeExpression },
        { "(= 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(= ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(= 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 == 1 },
        { "(= 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 == 0 },
        { "(= 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 == 1 },
        { "(= 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 == 0 },
        { "(= 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 == 1.0f },
        { "(= 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f == 1 },
        { "(= 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f == 0 },
        { "(= 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 == 1.0f },
        { "(= 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 == 0.0f },
        { "(= 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 == 0.0f },
        { "(= 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f == 1.0f },
        { "(= 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f == 0.0f },
        { "(= 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f == 0.0f },
        { "(= 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 == 1.5f },
        { "(= 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f == 1 },
        { "(= 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f == 1.0f },
        { "(= 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f == 1.5f },
        { "(= 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f == 1.5f },
        
        { "(= 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 == 1) && (1 == 1) },
        { "(= 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 == 0) && (0 == 1) },
        { "(= 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 == 1) && (1 == 1) },
        { "(= 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 == 0) && (0 == 0) },
        { "(= 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 == 1.0f) && (1.0f == 1) },
        { "(= 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f == 1) && (1 == 1) },
        { "(= 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f == 0) && (0 == 1) },
        { "(= 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 == 1.0f) && (1.0f == 1) },
        { "(= 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 == 0.0f) && (0.0f == 0) },
        { "(= 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 == 0.0f) && (0.0f == 0) },
        { "(= 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f == 1.0f) && (1.0f == 1) },
        { "(= 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f == 0.0f) && (0.0f == 1) },
        { "(= 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f == 0.0f) && (0.0f == 0) },
        { "(= 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 == 1.5f) && (1.5f == 1.5f) },
        { "(= 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f == 1) && (1 == 1.5f) },
        { "(= 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f == 1.0f) && (1.0f == 1.5f) },
        { "(= 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f == 1.5f) && (1.5f == 1.5f) },
        { "(= 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f == 1.5f) && (1.5f == 1.5f) },
        
        { "(= 1 test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(= 1 \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(= 1 ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(= test 1)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(= \"test\" 1)", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(= () 1)",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(= test test)",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= test \"test\")",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= \"test\" \"test\")", CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= () ())",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 2 3) (1 2 3))",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (test) (\"test\"))",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 \"test\") (1 \"test\"))", CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= (1 \"tests\") (1 \"test\"))",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(= (= 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 == 1) == 1 },
        { "(= (= 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 == 0) == 1 },
        { "(= (= 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 == 1) == 1 },
        { "(= (= 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 == 0) == 0 },
        { "(= (= 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 == 1.0f) == 1 },
        { "(= (= 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f == 1) == 1 },
        { "(= (= 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f == 0) == 1 },
        { "(= (= 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 == 1.0f) == 1 },
        { "(= (= 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 == 0.0f) == 0 },
        { "(= (= 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 == 0.0f) == 0 },
        { "(= (= 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f == 1.0f) == 1 },
        { "(= (= 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f == 0.0f) == 1 },
        { "(= (= 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f == 0.0f) == 0 },
        { "(= (= 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 == 1.5f) == 1.5f },
        { "(= (= 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f == 1) == 1.5f },
        { "(= (= 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f == 1.0f) == 1.5f },
        { "(= (= 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f == 1.5f) == 1.5f },
        { "(= (= 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f == 1.5f) == 1.5f }
    };
#pragma clang diagnostic pop

    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testNotEqual
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(!=)",            CCExpressionValueTypeExpression },
        { "(!= 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(!= ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(!= 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 != 1 },
        { "(!= 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 != 0 },
        { "(!= 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 != 1 },
        { "(!= 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 != 0 },
        { "(!= 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 != 1.0f },
        { "(!= 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f != 1 },
        { "(!= 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f != 0 },
        { "(!= 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 != 1.0f },
        { "(!= 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 != 0.0f },
        { "(!= 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 != 0.0f },
        { "(!= 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f != 1.0f },
        { "(!= 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f != 0.0f },
        { "(!= 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f != 0.0f },
        { "(!= 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 != 1.5f },
        { "(!= 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f != 1 },
        { "(!= 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f != 1.0f },
        { "(!= 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f != 1.5f },
        { "(!= 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f != 1.5f },
        
        { "(!= 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 != 1) && (1 != 1) },
        { "(!= 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 != 0) && (1 != 1) },
        { "(!= 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 != 1) && (0 != 1) },
        { "(!= 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 != 0) && (0 != 0) },
        { "(!= 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 != 1.0f) && (1 != 1) },
        { "(!= 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f != 1) && (1.0f != 1) },
        { "(!= 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f != 0) && (1.0f != 1) },
        { "(!= 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 != 1.0f) && (0 != 1) },
        { "(!= 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 != 0.0f) && (0 != 0) },
        { "(!= 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 != 0.0f) && (0 != 0) },
        { "(!= 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f != 1.0f) && (1.0f != 1) },
        { "(!= 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f != 0.0f) && (1.0f != 1) },
        { "(!= 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f != 0.0f) && (0.0f != 0) },
        { "(!= 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 != 1.5f) && (1 != 1.5f) },
        { "(!= 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f != 1) && (1.5f != 1.5f) },
        { "(!= 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f != 1.0f) && (1.5f != 1.5f) },
        { "(!= 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f != 1.5f) && (1.0f != 1.5f) },
        { "(!= 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f != 1.5f) && (1.5f != 1.5f) },
        
        { "(!= 1 test)",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(!= 1 \"test\")", CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(!= 1 ())",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(!= test 1)",     CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(!= \"test\" 1)", CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(!= () 1)",       CCExpressionValueTypeInteger,   .integer = TRUE },
        
        { "(!= test test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= test \"test\")",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= \"test\" \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= () ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= (1 2 3) (1 2 3))",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= (test) (\"test\"))",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= (1 \"test\") (1 \"test\"))", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(!= (1 \"tests\") (1 \"test\"))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        
        { "(!= (!= 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 != 1) != 1 },
        { "(!= (!= 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 != 0) != 1 },
        { "(!= (!= 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 != 1) != 1 },
        { "(!= (!= 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 != 0) != 0 },
        { "(!= (!= 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 != 1.0f) != 1 },
        { "(!= (!= 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f != 1) != 1 },
        { "(!= (!= 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f != 0) != 1 },
        { "(!= (!= 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 != 1.0f) != 1 },
        { "(!= (!= 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 != 0.0f) != 0 },
        { "(!= (!= 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 != 0.0f) != 0 },
        { "(!= (!= 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f != 1.0f) != 1 },
        { "(!= (!= 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f != 0.0f) != 1 },
        { "(!= (!= 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f != 0.0f) != 0 },
        { "(!= (!= 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 != 1.5f) != 1.5f },
        { "(!= (!= 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f != 1) != 1.5f },
        { "(!= (!= 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f != 1.0f) != 1.5f },
        { "(!= (!= 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f != 1.5f) != 1.5f },
        { "(!= (!= 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f != 1.5f) != 1.5f }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testLessThan
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(<)",            CCExpressionValueTypeExpression },
        { "(< 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(< ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(< 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 < 1 },
        { "(< 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 < 0 },
        { "(< 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 < 1 },
        { "(< 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 < 0 },
        { "(< 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 < 1.0f },
        { "(< 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f < 1 },
        { "(< 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f < 0 },
        { "(< 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 < 1.0f },
        { "(< 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 < 0.0f },
        { "(< 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 < 0.0f },
        { "(< 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f < 1.0f },
        { "(< 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f < 0.0f },
        { "(< 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f < 0.0f },
        { "(< 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 < 1.5f },
        { "(< 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f < 1 },
        { "(< 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f < 1.0f },
        { "(< 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f < 1.5f },
        { "(< 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f < 1.5f },
        
        { "(< 1 2 3)",      CCExpressionValueTypeInteger,   .integer = (1 < 2) && (2 < 3) },
        { "(< 3 2 1)",      CCExpressionValueTypeInteger,   .integer = (3 < 2) && (2 < 1) },
        { "(< 2 1 3)",      CCExpressionValueTypeInteger,   .integer = (2 < 1) && (1 < 3) },
        { "(< 3 1 2)",      CCExpressionValueTypeInteger,   .integer = (3 < 1) && (1 < 2) },
        { "(< 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 < 1) && (1 < 1) },
        { "(< 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 < 0) && (0 < 1) },
        { "(< 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 < 1) && (1 < 1) },
        { "(< 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 < 0) && (0 < 0) },
        { "(< 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 < 1.0f) && (1.0f < 1) },
        { "(< 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f < 1) && (1 < 1) },
        { "(< 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f < 0) && (0 < 1) },
        { "(< 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 < 1.0f) && (1.0f < 1) },
        { "(< 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 < 0.0f) && (0.0f < 0) },
        { "(< 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 < 0.0f) && (0.0f < 0) },
        { "(< 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f < 1.0f) && (1.0f < 1) },
        { "(< 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f < 0.0f) && (0.0f < 1) },
        { "(< 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f < 0.0f) && (0.0f < 0) },
        { "(< 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 < 1.5f) && (1.5f < 1.5f) },
        { "(< 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f < 1) && (1 < 1.5f) },
        { "(< 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f < 1.0f) && (1.0f < 1.5f) },
        { "(< 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f < 1.5f) && (1.5f < 1.5f) },
        { "(< 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f < 1.5f) && (1.5f < 1.5f) },
        
        { "(< 1 test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(< 1 \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(< 1 ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(< test 1)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(< \"test\" 1)", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(< () 1)",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(< (< 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 < 1) < 1 },
        { "(< (< 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 < 0) < 1 },
        { "(< (< 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 < 1) < 1 },
        { "(< (< 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 < 0) < 0 },
        { "(< (< 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 < 1.0f) < 1 },
        { "(< (< 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f < 1) < 1 },
        { "(< (< 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f < 0) < 1 },
        { "(< (< 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 < 1.0f) < 1 },
        { "(< (< 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 < 0.0f) < 0 },
        { "(< (< 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 < 0.0f) < 0 },
        { "(< (< 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f < 1.0f) < 1 },
        { "(< (< 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f < 0.0f) < 1 },
        { "(< (< 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f < 0.0f) < 0 },
        { "(< (< 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 < 1.5f) < 1.5f },
        { "(< (< 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f < 1) < 1.5f },
        { "(< (< 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f < 1.0f) < 1.5f },
        { "(< (< 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f < 1.5f) < 1.5f },
        { "(< (< 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f < 1.5f) < 1.5f }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testLessThanEqual
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(<=)",            CCExpressionValueTypeExpression },
        { "(<= 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(<= ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(<= 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 <= 1 },
        { "(<= 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 <= 0 },
        { "(<= 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 <= 1 },
        { "(<= 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 <= 0 },
        { "(<= 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 <= 1.0f },
        { "(<= 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f <= 1 },
        { "(<= 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f <= 0 },
        { "(<= 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 <= 1.0f },
        { "(<= 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 <= 0.0f },
        { "(<= 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 <= 0.0f },
        { "(<= 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f <= 1.0f },
        { "(<= 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f <= 0.0f },
        { "(<= 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f <= 0.0f },
        { "(<= 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 <= 1.5f },
        { "(<= 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f <= 1 },
        { "(<= 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f <= 1.0f },
        { "(<= 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f <= 1.5f },
        { "(<= 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f <= 1.5f },
        
        { "(<= 1 2 3)",      CCExpressionValueTypeInteger,   .integer = (1 <= 2) && (2 <= 3) },
        { "(<= 3 2 1)",      CCExpressionValueTypeInteger,   .integer = (3 <= 2) && (2 <= 1) },
        { "(<= 2 1 3)",      CCExpressionValueTypeInteger,   .integer = (2 <= 1) && (1 <= 3) },
        { "(<= 3 1 2)",      CCExpressionValueTypeInteger,   .integer = (3 <= 1) && (1 <= 2) },
        { "(<= 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 <= 1) && (1 <= 1) },
        { "(<= 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 <= 0) && (0 <= 1) },
        { "(<= 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 <= 1) && (1 <= 1) },
        { "(<= 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 <= 0) && (0 <= 0) },
        { "(<= 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 <= 1.0f) && (1.0f <= 1) },
        { "(<= 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f <= 1) && (1 <= 1) },
        { "(<= 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f <= 0) && (0 <= 1) },
        { "(<= 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 <= 1.0f) && (1.0f <= 1) },
        { "(<= 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 <= 0.0f) && (0.0f <= 0) },
        { "(<= 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 <= 0.0f) && (0.0f <= 0) },
        { "(<= 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f <= 1.0f) && (1.0f <= 1) },
        { "(<= 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f <= 0.0f) && (0.0f <= 1) },
        { "(<= 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f <= 0.0f) && (0.0f <= 0) },
        { "(<= 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 <= 1.5f) && (1.5f <= 1.5f) },
        { "(<= 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f <= 1) && (1 <= 1.5f) },
        { "(<= 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f <= 1.0f) && (1.0f <= 1.5f) },
        { "(<= 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f <= 1.5f) && (1.5f <= 1.5f) },
        { "(<= 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f <= 1.5f) && (1.5f <= 1.5f) },
        
        { "(<= 1 test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(<= 1 \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(<= 1 ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(<= test 1)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(<= \"test\" 1)", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(<= () 1)",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(<= (<= 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 <= 1) <= 1 },
        { "(<= (<= 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 <= 0) <= 1 },
        { "(<= (<= 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 <= 1) <= 1 },
        { "(<= (<= 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 <= 0) <= 0 },
        { "(<= (<= 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 <= 1.0f) <= 1 },
        { "(<= (<= 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f <= 1) <= 1 },
        { "(<= (<= 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f <= 0) <= 1 },
        { "(<= (<= 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 <= 1.0f) <= 1 },
        { "(<= (<= 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 <= 0.0f) <= 0 },
        { "(<= (<= 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 <= 0.0f) <= 0 },
        { "(<= (<= 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f <= 1.0f) <= 1 },
        { "(<= (<= 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f <= 0.0f) <= 1 },
        { "(<= (<= 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f <= 0.0f) <= 0 },
        { "(<= (<= 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 <= 1.5f) <= 1.5f },
        { "(<= (<= 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f <= 1) <= 1.5f },
        { "(<= (<= 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f <= 1.0f) <= 1.5f },
        { "(<= (<= 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f <= 1.5f) <= 1.5f },
        { "(<= (<= 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f <= 1.5f) <= 1.5f }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testGreaterThan
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(>)",            CCExpressionValueTypeExpression },
        { "(> 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(> ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(> 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 > 1 },
        { "(> 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 > 0 },
        { "(> 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 > 1 },
        { "(> 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 > 0 },
        { "(> 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 > 1.0f },
        { "(> 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f > 1 },
        { "(> 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f > 0 },
        { "(> 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 > 1.0f },
        { "(> 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 > 0.0f },
        { "(> 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 > 0.0f },
        { "(> 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f > 1.0f },
        { "(> 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f > 0.0f },
        { "(> 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f > 0.0f },
        { "(> 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 > 1.5f },
        { "(> 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f > 1 },
        { "(> 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f > 1.0f },
        { "(> 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f > 1.5f },
        { "(> 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f > 1.5f },
        
        { "(> 1 2 3)",      CCExpressionValueTypeInteger,   .integer = (1 > 2) && (2 > 3) },
        { "(> 3 2 1)",      CCExpressionValueTypeInteger,   .integer = (3 > 2) && (2 > 1) },
        { "(> 2 1 3)",      CCExpressionValueTypeInteger,   .integer = (2 > 1) && (1 > 3) },
        { "(> 3 1 2)",      CCExpressionValueTypeInteger,   .integer = (3 > 1) && (1 > 2) },
        { "(> 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 > 1) && (1 > 1) },
        { "(> 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 > 0) && (0 > 1) },
        { "(> 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 > 1) && (1 > 1) },
        { "(> 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 > 0) && (0 > 0) },
        { "(> 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 > 1.0f) && (1.0f > 1) },
        { "(> 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f > 1) && (1 > 1) },
        { "(> 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f > 0) && (0 > 1) },
        { "(> 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 > 1.0f) && (1.0f > 1) },
        { "(> 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 > 0.0f) && (0.0f > 0) },
        { "(> 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 > 0.0f) && (0.0f > 0) },
        { "(> 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f > 1.0f) && (1.0f > 1) },
        { "(> 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f > 0.0f) && (0.0f > 1) },
        { "(> 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f > 0.0f) && (0.0f > 0) },
        { "(> 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 > 1.5f) && (1.5f > 1.5f) },
        { "(> 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f > 1) && (1 > 1.5f) },
        { "(> 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f > 1.0f) && (1.0f > 1.5f) },
        { "(> 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f > 1.5f) && (1.5f > 1.5f) },
        { "(> 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f > 1.5f) && (1.5f > 1.5f) },
        
        { "(> 1 test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(> 1 \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(> 1 ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(> test 1)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(> \"test\" 1)", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(> () 1)",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(> (> 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 > 1) > 1 },
        { "(> (> 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 > 0) > 1 },
        { "(> (> 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 > 1) > 1 },
        { "(> (> 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 > 0) > 0 },
        { "(> (> 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 > 1.0f) > 1 },
        { "(> (> 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f > 1) > 1 },
        { "(> (> 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f > 0) > 1 },
        { "(> (> 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 > 1.0f) > 1 },
        { "(> (> 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 > 0.0f) > 0 },
        { "(> (> 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 > 0.0f) > 0 },
        { "(> (> 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f > 1.0f) > 1 },
        { "(> (> 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f > 0.0f) > 1 },
        { "(> (> 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f > 0.0f) > 0 },
        { "(> (> 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 > 1.5f) > 1.5f },
        { "(> (> 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f > 1) > 1.5f },
        { "(> (> 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f > 1.0f) > 1.5f },
        { "(> (> 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f > 1.5f) > 1.5f },
        { "(> (> 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f > 1.5f) > 1.5f }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

-(void) testGreaterThanEqual
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wtautological-compare"
    ExpressionResult Test[] = {
        { "(>=)",            CCExpressionValueTypeExpression },
        { "(>= 1)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= -1)",         CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= 1.5)",        CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= -1.5)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= test)",       CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= i)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= f)",          CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= \"string\")", CCExpressionValueTypeInteger,   .integer = 0 },
        { "(>= ())",         CCExpressionValueTypeInteger,   .integer = 0 },
        
        { "(>= 1 1)",        CCExpressionValueTypeInteger,   .integer = 1 >= 1 },
        { "(>= 1 0)",        CCExpressionValueTypeInteger,   .integer = 1 >= 0 },
        { "(>= 0 1)",        CCExpressionValueTypeInteger,   .integer = 0 >= 1 },
        { "(>= 0 0)",        CCExpressionValueTypeInteger,   .integer = 0 >= 0 },
        { "(>= 1 1.0)",      CCExpressionValueTypeInteger,   .integer = 1 >= 1.0f },
        { "(>= 1.0 1)",      CCExpressionValueTypeInteger,   .integer = 1.0f >= 1 },
        { "(>= 1.0 0)",      CCExpressionValueTypeInteger,   .integer = 1.0f >= 0 },
        { "(>= 0 1.0)",      CCExpressionValueTypeInteger,   .integer = 0 >= 1.0f },
        { "(>= 0 0.0)",      CCExpressionValueTypeInteger,   .integer = 0 >= 0.0f },
        { "(>= 0.0 0)",      CCExpressionValueTypeInteger,   .integer = 0 >= 0.0f },
        { "(>= 1.0 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f >= 1.0f },
        { "(>= 1.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 1.0f >= 0.0f },
        { "(>= 0.0 0.0)",    CCExpressionValueTypeInteger,   .integer = 0.0f >= 0.0f },
        { "(>= 1 1.5)",      CCExpressionValueTypeInteger,   .integer = 1 >= 1.5f },
        { "(>= 1.5 1)",      CCExpressionValueTypeInteger,   .integer = 1.5f >= 1 },
        { "(>= 1.5 1.0)",    CCExpressionValueTypeInteger,   .integer = 1.5f >= 1.0f },
        { "(>= 1.0 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.0f >= 1.5f },
        { "(>= 1.5 1.5)",    CCExpressionValueTypeInteger,   .integer = 1.5f >= 1.5f },
        
        { "(>= 1 2 3)",      CCExpressionValueTypeInteger,   .integer = (1 >= 2) && (2 >= 3) },
        { "(>= 3 2 1)",      CCExpressionValueTypeInteger,   .integer = (3 >= 2) && (2 >= 1) },
        { "(>= 2 1 3)",      CCExpressionValueTypeInteger,   .integer = (2 >= 1) && (1 >= 3) },
        { "(>= 3 1 2)",      CCExpressionValueTypeInteger,   .integer = (3 >= 1) && (1 >= 2) },
        { "(>= 1 1 1)",      CCExpressionValueTypeInteger,   .integer = (1 >= 1) && (1 >= 1) },
        { "(>= 1 0 1)",      CCExpressionValueTypeInteger,   .integer = (1 >= 0) && (0 >= 1) },
        { "(>= 0 1 1)",      CCExpressionValueTypeInteger,   .integer = (0 >= 1) && (1 >= 1) },
        { "(>= 0 0 0)",      CCExpressionValueTypeInteger,   .integer = (0 >= 0) && (0 >= 0) },
        { "(>= 1 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (1 >= 1.0f) && (1.0f >= 1) },
        { "(>= 1.0 1 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f >= 1) && (1 >= 1) },
        { "(>= 1.0 0 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f >= 0) && (0 >= 1) },
        { "(>= 0 1.0 1)",    CCExpressionValueTypeInteger,   .integer = (0 >= 1.0f) && (1.0f >= 1) },
        { "(>= 0 0.0 0)",    CCExpressionValueTypeInteger,   .integer = (0 >= 0.0f) && (0.0f >= 0) },
        { "(>= 0.0 0 0)",    CCExpressionValueTypeInteger,   .integer = (0 >= 0.0f) && (0.0f >= 0) },
        { "(>= 1.0 1.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f >= 1.0f) && (1.0f >= 1) },
        { "(>= 1.0 0.0 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f >= 0.0f) && (0.0f >= 1) },
        { "(>= 0.0 0.0 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f >= 0.0f) && (0.0f >= 0) },
        { "(>= 1 1.5 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 >= 1.5f) && (1.5f >= 1.5f) },
        { "(>= 1.5 1 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f >= 1) && (1 >= 1.5f) },
        { "(>= 1.5 1.0 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f >= 1.0f) && (1.0f >= 1.5f) },
        { "(>= 1.0 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f >= 1.5f) && (1.5f >= 1.5f) },
        { "(>= 1.5 1.5 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f >= 1.5f) && (1.5f >= 1.5f) },
        
        { "(>= 1 test)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(>= 1 \"test\")", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(>= 1 ())",       CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(>= test 1)",     CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(>= \"test\" 1)", CCExpressionValueTypeInteger,   .integer = FALSE },
        { "(>= () 1)",       CCExpressionValueTypeInteger,   .integer = FALSE },
        
        { "(>= (>= 1 1) 1)",      CCExpressionValueTypeInteger,   .integer = (1 >= 1) >= 1 },
        { "(>= (>= 1 0) 1)",      CCExpressionValueTypeInteger,   .integer = (1 >= 0) >= 1 },
        { "(>= (>= 0 1) 1)",      CCExpressionValueTypeInteger,   .integer = (0 >= 1) >= 1 },
        { "(>= (>= 0 0) 0)",      CCExpressionValueTypeInteger,   .integer = (0 >= 0) >= 0 },
        { "(>= (>= 1 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (1 >= 1.0f) >= 1 },
        { "(>= (>= 1.0 1) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f >= 1) >= 1 },
        { "(>= (>= 1.0 0) 1)",    CCExpressionValueTypeInteger,   .integer = (1.0f >= 0) >= 1 },
        { "(>= (>= 0 1.0) 1)",    CCExpressionValueTypeInteger,   .integer = (0 >= 1.0f) >= 1 },
        { "(>= (>= 0 0.0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 >= 0.0f) >= 0 },
        { "(>= (>= 0.0 0) 0)",    CCExpressionValueTypeInteger,   .integer = (0 >= 0.0f) >= 0 },
        { "(>= (>= 1.0 1.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f >= 1.0f) >= 1 },
        { "(>= (>= 1.0 0.0) 1)",  CCExpressionValueTypeInteger,   .integer = (1.0f >= 0.0f) >= 1 },
        { "(>= (>= 0.0 0.0) 0)",  CCExpressionValueTypeInteger,   .integer = (0.0f >= 0.0f) >= 0 },
        { "(>= (>= 1 1.5) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1 >= 1.5f) >= 1.5f },
        { "(>= (>= 1.5 1) 1.5)",  CCExpressionValueTypeInteger,   .integer = (1.5f >= 1) >= 1.5f },
        { "(>= (>= 1.5 1.0) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f >= 1.0f) >= 1.5f },
        { "(>= (>= 1.0 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.0f >= 1.5f) >= 1.5f },
        { "(>= (>= 1.5 1.5) 1.5)",CCExpressionValueTypeInteger,   .integer = (1.5f >= 1.5f) >= 1.5f }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

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
        { "(= 1 (not 0))",       CCExpressionValueTypeInteger,   .integer = TRUE },
        { "(= 0 (not 10))",       CCExpressionValueTypeInteger,   .integer = TRUE }
    };
#pragma clang diagnostic pop
    
    
    [self assert: Test count: sizeof(Test) / sizeof(*Test)];
}

@end
