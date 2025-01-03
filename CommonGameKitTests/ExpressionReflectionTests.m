/*
 *  Copyright (c) 2024, Stefan Johnson
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
#import "ExpressionReflection.h"
#import "Input.h"
#import "ReflectedTypes.h"

@interface ExpressionReflectionTests : CCTestCase

@end

@implementation ExpressionReflectionTests

-(void) testIntegers
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    CCExpression Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int), &(int){ 55 }, Zone);
    int ResultI;
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int), &ResultI, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI, 55, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int), &(int){ -55 }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int), &ResultI, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI, -55, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int), &(int){ INT_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int), &ResultI, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI, INT_MAX, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int), &(int){ INT_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int), &ResultI, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI, INT_MIN, @"Should serialise and deserialise correctly");
    
    
    unsigned int ResultU;
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(unsigned int), &(unsigned int){ UINT_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(unsigned int), &ResultU, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultU, UINT_MAX, @"Should serialise and deserialise correctly");
    
    
    int64_t ResultI64;
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int64_t), &(int64_t){ INT64_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int64_t), &ResultI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI64, INT64_MAX, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(int64_t), &(int64_t){ INT64_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int64_t), &ResultI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI64, INT64_MIN, @"Should serialise and deserialise correctly");
    
    
    int8_t ResultI8;
    Expression = CCExpressionCreateInteger(CC_STD_ALLOCATOR, 300);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, 127, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateInteger(CC_STD_ALLOCATOR, 300);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int64_t), &ResultI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI64, 300, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFloat(CC_STD_ALLOCATOR, 30.0f);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, 30, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("0xf"), TRUE);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, 15, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("-0xf"), TRUE);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, -15, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("-0xff"), TRUE);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, -128, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("0xff"), TRUE);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultI8, 127, @"Should serialise and deserialise correctly");
    
    
    for (size_t Loop = 0; Loop < 56; Loop++)
    {
        for (size_t Loop2 = 0; Loop2 < 256; Loop2++)
        {
            for (size_t Loop3 = 0; Loop3 < 2; Loop3++)
            {
                int8_t ResultI8;
                uint8_t ResultU8;
                
                intmax_t Value = (Loop2 << Loop) | (Loop3 << 63);
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint8_t), &ResultU8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU8, CCClamp(Value, 0, UINT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI8, CCClamp(Value, INT8_MIN, INT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Value &= ~INTMAX_C(0xff);
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint8_t), &ResultU8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU8, CCClamp(Value, 0, UINT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI8, CCClamp(Value, INT8_MIN, INT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Value |= 1;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint8_t), &ResultU8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU8, CCClamp(Value, 0, UINT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI8, CCClamp(Value, INT8_MIN, INT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Value |= 0xff;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint8_t), &ResultU8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU8, CCClamp(Value, 0, UINT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI8, CCClamp(Value, INT8_MIN, INT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Value ^= 1;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint8_t), &ResultU8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU8, CCClamp(Value, 0, UINT8_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int8_t), &ResultI8, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI8, CCClamp(Value, INT8_MIN, INT8_MAX), @"Should serialise and deserialise correctly");
            }
        }
    }
    
    
    for (size_t Loop = 0; Loop < 56; Loop++)
    {
        for (size_t Loop2 = 0; Loop2 < 256; Loop2++)
        {
            for (size_t Loop3 = 0; Loop3 < 2; Loop3++)
            {
                int32_t ResultI32;
                uint32_t ResultU32;
                
                intmax_t Value = (Loop2 << Loop) | (Loop3 << 63);
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint32_t), &ResultU32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU32, CCClamp(Value, 0, UINT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int32_t), &ResultI32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI32, CCClamp(Value, INT32_MIN, INT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Value &= ~INTMAX_C(0xff);
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint32_t), &ResultU32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU32, CCClamp(Value, 0, UINT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int32_t), &ResultI32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI32, CCClamp(Value, INT32_MIN, INT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Value |= 1;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint32_t), &ResultU32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU32, CCClamp(Value, 0, UINT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int32_t), &ResultI32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI32, CCClamp(Value, INT32_MIN, INT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Value |= 0xff;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint32_t), &ResultU32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU32, CCClamp(Value, 0, UINT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int32_t), &ResultI32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI32, CCClamp(Value, INT32_MIN, INT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Value ^= 1;
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(uint32_t), &ResultU32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultU32, CCClamp(Value, 0, UINT32_MAX), @"Should serialise and deserialise correctly");
                
                
                Expression = CCExpressionReflectionSerialize(&CC_REFLECT(intmax_t), &Value, Zone);
                CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(int32_t), &ResultI32, Zone, CC_STD_ALLOCATOR);
                CCExpressionDestroy(Expression);
                
                XCTAssertEqual(ResultI32, CCClamp(Value, INT32_MIN, INT32_MAX), @"Should serialise and deserialise correctly");
            }
        }
    }
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testFloats
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    float ResultF32;
    CCExpression Expression = CCExpressionReflectionSerialize(&CC_REFLECT(float), &(float){ 55.0f }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(float), &ResultF32, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultF32, 55.0f, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(float), &(float){ -55.0f }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(float), &ResultF32, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultF32, -55.0f, @"Should serialise and deserialise correctly");
    
    
    double ResultF64;
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(double), &(double){ -55.0 }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(double), &ResultF64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultF64, -55.0, @"Should serialise and deserialise correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testStrings
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    CCString ResultStr;
    CCExpression Expression = CCExpressionReflectionSerialize(&CC_REFLECT(CCString), &(CCString){ CC_STRING("foo") }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCString), &ResultStr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertTrue(CCStringEqual(ResultStr, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    
    CCStringDestroy(ResultStr);
    
    
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT(CCString), &(CCString){ CC_STRING("foobar1234567890") }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCString), &ResultStr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertTrue(CCStringEqual(ResultStr, CC_STRING("foobar1234567890")), @"Should serialise and deserialise correctly");
    
    CCStringDestroy(ResultStr);
    
    
    Expression = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("foo"), TRUE);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCString), &ResultStr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertTrue(CCStringEqual(ResultStr, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    
    CCStringDestroy(ResultStr);
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testArrays
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    int ResultIx3[3] = { 0, 0, 0 };
    CCExpression Expression = CCExpressionReflectionSerialize(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), (int[3]){ 1, 2, 3 }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), ResultIx3, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3[0], 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3[1], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3[2], 3, @"Should serialise and deserialise correctly");
    
    
    int ResultIx3x2[3][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 } };
    Expression = CCExpressionReflectionSerialize(&CC_REFLECT_ARRAY(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 2), 3), (int[3][2]){ { 1, 10 }, { 2, 20 }, { 3, 30 } }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 2), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 10, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 20, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 30, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(10 20 30)");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), ResultIx3, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3[0], 10, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3[1], 20, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3[2], 30, @"Should serialise and deserialise correctly");
    
    
    memset(ResultIx3x2, 0, sizeof(ResultIx3x2));
    Expression = CCExpressionCreateFromSource("(1 2)");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 0, @"Should serialise and deserialise correctly");
    
    
    memset(ResultIx3x2, 0, sizeof(ResultIx3x2));
    Expression = CCExpressionCreateFromSource("(1 2 3 4)");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 0, @"Should serialise and deserialise correctly");
    
    
    memset(ResultIx3x2, 0, sizeof(ResultIx3x2));
    Expression = CCExpressionCreateFromSource("((1 10) 2 (3) 4)");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 0, @"Should serialise and deserialise correctly");
    
    
    memset(ResultIx3x2, 0, sizeof(ResultIx3x2));
    Expression = CCExpressionCreateFromSource("((1 10) 2 (3) 4)");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 2), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 10, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 0, @"Should serialise and deserialise correctly");
    
    
    memset(ResultIx3x2, 0, sizeof(ResultIx3x2));
    Expression = CCExpressionCreateFromSource("(() (2 20 200))");
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT_ARRAY(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 2), 3), ResultIx3x2, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultIx3x2[0][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[0][1], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][0], 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[1][1], 20, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][0], 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultIx3x2[2][1], 0, @"Should serialise and deserialise correctly");

    CCMemoryZoneDestroy(Zone);
}

-(void) testOpaques
{
    CCReflectOpaqueTypeDescriptorMap = CCReflectOpaqueTypeDescriptorMapCGKDefaults;
    CCReflectOpaqueTypeDescriptorUnmap = CCReflectOpaqueTypeDescriptorUnmapCGKDefaults;
    
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    CCInputMap ResultInputMap = { 0 };
    CCExpression Expression = CCExpressionReflectionSerialize(&CC_REFLECT(CCInputMap), &(CCInputMap){
        .type = CCInputMapTypeGroup,
        .group = {
            .count = 2,
            .wantsAllActive = FALSE,
            .inputMaps = (CCInputMap[2]){
                {
                    .type = CCInputMapTypeMouseButton,
                    .mouse = {
                        .button = {
                            .code = CCMouseButton3,
                            .flags = CCKeyboardModifierShift,
                            .ramp = 1.5f,
                            .ignoreModifier = FALSE
                        }
                    }
                },
                {
                    .type = CCInputMapTypeControllerAxis,
                    .controller = {
                        .device = CC_STRING("foo"),
                        .connection = 5,
                        .axis = {
                            .deadzone = 2.0f,
                            .resolution = 20,
                            .x = {
                                .index = 1,
                                .flip = FALSE,
                                .oneSided = FALSE
                            },
                            .y = {
                                .index = 0,
                                .flip = TRUE,
                                .oneSided = FALSE
                            },
                            .z = { .index = -1 },
                            .w = { .index = -1 }
                        }
                    }
                }
            }
        }
    }, Zone);
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeGroup, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.count, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.wantsAllActive, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertNotEqual(ResultInputMap.group.inputMaps, NULL, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].type, CCInputMapTypeMouseButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.code, CCMouseButton3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.flags, CCKeyboardModifierShift, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.ramp, 1.5f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.ignoreModifier, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].type, CCInputMapTypeControllerAxis, @"Should serialise and deserialise correctly");
    XCTAssertTrue(CCStringEqual(ResultInputMap.group.inputMaps[1].controller.device, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.connection, 5, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.deadzone, 2.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.resolution, 20, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.index, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.flip, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.oneSided, FALSE, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource(
        "("
        "    ("
        "        :struct"
        "        (size: 64)"
        "        (fields:"
        "            ("
        "                ("
        "                    (name: \"type\")"
        "                    (offset: 0)"
        "                    (type: (:integer (sign: 1) (fixed: 0) (endian: 0) (size: 4)))"
        "                )"
        "                ("
        "                    (name: \"group.inputMaps\")"
        "                    (offset: 8)"
        "                    (type:"
        "                        ("
        "                            :pointer"
        "                            (type:"
        "                                (:array"
        "                                    (type:"
        "                                        (:opaque"
        "                                            (type-size: 40)"
        "                                            (descriptor: ((:integer (sign: 0) (fixed: 1) (endian: 0) (size: 1)) 10))"
        "                                        )"
        "                                    )"
        "                                    (count: 2)"
        "                                )"
        "                            )"
        "                            (storage: 1)"
        "                            (ownership: 2)"
        "                        )"
        "                    )"
        "                )"
        "                ("
        "                    (name: \"group.count\")"
        "                    (offset: 16)"
        "                    (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 8)))"
        "                )"
        "                ("
        "                    (name: \"group.wantsAllActive\")"
        "                    (offset: 24)"
        "                    (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                )"
        "            )"
        "        )"
        "    )"
        "    ("
        "        (type: 1)"
        "        (group.inputMaps:"
        "            ("
        "                ("
        "                    ("
        "                        ("
        "                            :struct"
        "                            (size: 64)"
        "                            (fields:"
        "                                ("
        "                                    ("
        "                                        (name: \"type\")"
        "                                        (offset: 0)"
        "                                        (type: (:integer (sign: 1) (fixed: 0) (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"mouse.button.code\")"
        "                                        (offset: 8)"
        "                                        (type: (:integer (sign: 1) (fixed: 0) (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"mouse.button.flags\")"
        "                                        (offset: 12)"
        "                                        (type: (:integer (sign: 1) (fixed: 0) (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"mouse.button.ramp\")"
        "                                        (offset: 16)"
        "                                        (type: (:float (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"mouse.button.ignoreModifier\")"
        "                                        (offset: 20)"
        "                                        (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                    )"
        "                                )"
        "                            )"
        "                        )"
        "                        ("
        "                            (type: 4)"
        "                            (mouse.button.code: 2)"
        "                            (mouse.button.flags: 1)"
        "                            (mouse.button.ramp: 1.500000)"
        "                            (mouse.button.ignoreModifier: 0)"
        "                        )"
        "                    )"
        "                    ("
        "                        ("
        "                            :struct"
        "                            (size: 64)"
        "                            (fields:"
        "                                ("
        "                                    ("
        "                                        (name: \"type\")"
        "                                        (offset: 0)"
        "                                        (type: (:integer (sign: 1) (fixed: 0) (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.device\")"
        "                                        (offset: 8)"
        "                                        (type:"
        "                                            ("
        "                                                :opaque"
        "                                                (type-size: 40)"
        "                                                (descriptor: ((:integer (sign: 0) (fixed: 1) (endian: 0) (size: 1)) 3))"
        "                                            )"
        "                                        )"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.connection\")"
        "                                        (offset: 16)"
        "                                        (type: (:integer (sign: 1) (fixed: 1) (endian: 0) (size: 1)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.axis.deadzone\")"
        "                                        (offset: 20)"
        "                                        (type: (:float (endian: 0) (size: 4)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.axis.resolution\")"
        "                                        (offset: 24)"
        "                                        (type: (:integer (sign: 0) (fixed: 1) (endian: 0) (size: 1)))"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.axis.x\")"
        "                                        (offset: 28)"
        "                                        (type:"
        "                                            ("
        "                                                :struct"
        "                                                (size: 8)"
        "                                                (fields:"
        "                                                    ("
        "                                                        ("
        "                                                            (name: \"index\")"
        "                                                            (offset: 0)"
        "                                                            (type: (:integer (sign: 1) (fixed: 1) (endian: 0) (size: 4)))"
        "                                                        )"
        "                                                        ("
        "                                                            (name: \"flip\")"
        "                                                            (offset: 4)"
        "                                                            (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                        )"
        "                                                        ("
        "                                                            (name: \"oneSided\")"
        "                                                            (offset: 5)"
        "                                                            (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                        )"
        "                                                    )"
        "                                                )"
        "                                            )"
        "                                        )"
        "                                    )"
        "                                    ("
        "                                        (name: \"controller.axis.y\")"
        "                                        (offset: 36)"
        "                                            (type:"
        "                                                ("
        "                                                    :struct"
        "                                                    (size: 8)"
        "                                                    (fields:"
        "                                                        ("
        "                                                            ("
        "                                                                (name: \"index\")"
        "                                                                (offset: 0)"
        "                                                                (type: (:integer (sign: 1) (fixed: 1) (endian: 0) (size: 4)))"
        "                                                            )"
        "                                                            ("
        "                                                                (name: \"flip\")"
        "                                                                (offset: 4)"
        "                                                                (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                            )"
        "                                                            ("
        "                                                                (name: \"oneSided\")"
        "                                                                (offset: 5)"
        "                                                                (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                            )"
        "                                                        )"
        "                                                    )"
        "                                                )"
        "                                            )"
        "                                        )"
        "                                        ("
        "                                            (name: \"controller.axis.z\")"
        "                                            (offset: 44)"
        "                                            (type:"
        "                                                ("
        "                                                    :struct"
        "                                                    (size: 8)"
        "                                                    (fields:"
        "                                                        ("
        "                                                            ("
        "                                                                (name: \"index\")"
        "                                                                (offset: 0)"
        "                                                                (type: (:integer (sign: 1) (fixed: 1) (endian: 0) (size: 4)))"
        "                                                            )"
        "                                                            ("
        "                                                                (name: \"flip\")"
        "                                                                (offset: 4)"
        "                                                                (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                            )"
        "                                                            ("
        "                                                                (name: \"oneSided\")"
        "                                                                (offset: 5)"
        "                                                                (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                            )"
        "                                                        )"
        "                                                    )"
        "                                                )"
        "                                            )"
        "                                        )"
        "                                        ("
        "                                            (name: \"controller.axis.w\")"
        "                                            (offset: 52)"
        "                                            (type:"
        "                                            ("
        "                                                :struct"
        "                                                (size: 8)"
        "                                                (fields:"
        "                                                    ("
        "                                                        ("
        "                                                            (name: \"index\")"
        "                                                            (offset: 0)"
        "                                                            (type: (:integer (sign: 1) (fixed: 1) (endian: 0) (size: 4)))"
        "                                                        )"
        "                                                        ("
        "                                                            (name: \"flip\")"
        "                                                            (offset: 4)"
        "                                                            (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                        )"
        "                                                        ("
        "                                                            (name: \"oneSided\")"
        "                                                            (offset: 5)"
        "                                                            (type: (:integer (sign: 0) (fixed: 0) (endian: 0) (size: 1)))"
        "                                                        )"
        "                                                    )"
        "                                                )"
        "                                            )"
        "                                        )"
        "                                    )"
        "                                )"
        "                            )"
        "                        )"
        "                        ("
        "                            (type: 7)"
        "                            (controller.device: \"foo\")"
        "                            (controller.connection: 5)"
        "                            (controller.axis.deadzone: 2.000000)"
        "                            (controller.axis.resolution: 20)"
        "                            (controller.axis.x:"
        "                                ("
        "                                    (index: 1)"
        "                                    (flip: 0)"
        "                                    (oneSided: 0)"
        "                                )"
        "                            )"
        "                            (controller.axis.y:"
        "                                ("
        "                                    (index: 0)"
        "                                    (flip: 1)"
        "                                    (oneSided: 0)"
        "                                )"
        "                            )"
        "                            (controller.axis.z:"
        "                                ("
        "                                    (index: -1)"
        "                                    (flip: 0)"
        "                                    (oneSided: 0)"
        "                                )"
        "                            )"
        "                            (controller.axis.w:"
        "                                ("
        "                                    (index: -1)"
        "                                    (flip: 0)"
        "                                    (oneSided: 0)"
        "                                )"
        "                            )"
        "                        )"
        "                    )"
        "                )"
        "            )"
        "        )"
        "        (group.count: 2)"
        "        (group.wantsAllActive: 0)"
        "    )"
        ")"
    );
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeGroup, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.count, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.wantsAllActive, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertNotEqual(ResultInputMap.group.inputMaps, NULL, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].type, CCInputMapTypeMouseButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.code, CCMouseButton3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.flags, CCKeyboardModifierShift, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.ramp, 1.5f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].mouse.button.ignoreModifier, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].type, CCInputMapTypeControllerAxis, @"Should serialise and deserialise correctly");
    XCTAssertTrue(CCStringEqual(ResultInputMap.group.inputMaps[1].controller.device, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.connection, 5, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.deadzone, 2.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.resolution, 20, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.x.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.index, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.flip, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.y.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.z.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.axis.w.oneSided, FALSE, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-mouse-button)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeMouseButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.code, CCMouseButtonUnknown, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.flags, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ignoreModifier, TRUE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-mouse-button (button: :middle))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeMouseButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.code, CCMouseButtonMiddle, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.flags, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ignoreModifier, TRUE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-mouse-button (button: 1) (ramp: 2) (flags: :control-shift))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeMouseButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.code, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.flags, CCKeyboardModifierControl | CCKeyboardModifierShift, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ramp, 2.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.mouse.button.ignoreModifier, FALSE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-keyboard)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeKeyboard, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.keycode, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.character, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.flags, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.isKeycode, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ignoreModifier, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.repeats, FALSE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-keyboard (keycode: :a) (character: \"a\") (ramp: 2) (flags: :control-shift) (repeat: #t))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeKeyboard, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.keycode, CCKeyboardKeycodeA, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.character, 'a', @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.flags, CCKeyboardModifierControl | CCKeyboardModifierShift, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ramp, 2.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.isKeycode, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ignoreModifier, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.repeats, TRUE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-keyboard (keycode: 1) (flags: 3))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeKeyboard, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.keycode, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.character, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.flags, 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.isKeycode, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.ignoreModifier, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.keyboard.repeats, FALSE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-none)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeNone, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-axes)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerAxis, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.deadzone, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.resolution, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.oneSided, FALSE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-axes (device: \"foo\") (connection: 2) (deadzone: 1.5) (resolution: 8) (x: 1) (y: 2 :flip) (z: 3 :one-sided) (w: 4 :flip :one-sided))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerAxis, @"Should serialise and deserialise correctly");
    XCTAssertTrue(CCStringEqual(ResultInputMap.controller.device, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.deadzone, 1.5f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.resolution, 8, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.x.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.index, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.flip, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.y.oneSided, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.index, 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.flip, FALSE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.z.oneSided, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.index, 4, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.flip, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.axis.w.oneSided, TRUE, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-button)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.button.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-button (device: \"foo\") (connection: 2) (button: 1) (ramp: 1.5))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertTrue(CCStringEqual(ResultInputMap.controller.device, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.button.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.button.ramp, 1.5f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-hat)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerHat, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.hat.index, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.hat.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-controller-hat (device: \"foo\") (connection: 2) (hat: 1) (ramp: 1.5))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeControllerHat, @"Should serialise and deserialise correctly");
    XCTAssertTrue(CCStringEqual(ResultInputMap.controller.device, CC_STRING("foo")), @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.connection, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.hat.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.controller.hat.ramp, 1.5f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-group)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeGroup, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps, NULL, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.count, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.wantsAllActive, FALSE, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-group (input: (input-controller-button (button: 1)) (input-controller-button (button: 2))) (all-active: #t))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeGroup, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.count, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.wantsAllActive, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].controller.button.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.button.index, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-group (input: 1 (input-controller-button (button: 2))) (all-active: #t))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeGroup, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.count, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.wantsAllActive, TRUE, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[0].type, CCInputMapTypeNone, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.button.index, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.group.inputMaps[1].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-virtual-joystick)");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeVirtualJoystick, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps, NULL, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.count, 0, @"Should serialise and deserialise correctly");
    
    
    Expression = CCExpressionCreateFromSource("(input-virtual-joystick (input: (input-controller-button (button: 1)) (input-controller-button (button: 2))))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeVirtualJoystick, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.count, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.button.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.button.index, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    
    Expression = CCExpressionCreateFromSource("(input-virtual-joystick (x: (input-controller-button (button: 1)) (input-controller-button (button: 2))) (y: (input-controller-button (button: 3))) (z: (input-controller-button (button: 4)) (input-controller-button (button: 5))) (w: (input-controller-button (button: 6)) (input-controller-button (button: 7))))");
    
    memset(&ResultInputMap, 0, sizeof(ResultInputMap));
    CCExpressionReflectionDeserialize(Expression, &CC_REFLECT(CCInputMap), &ResultInputMap, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expression);
    
    XCTAssertEqual(ResultInputMap.type, CCInputMapTypeVirtualJoystick, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.count, 8, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.button.index, 1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[0].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.button.index, 2, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[1].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[2].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[2].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[2].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[2].controller.button.index, 3, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[2].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[3].type, CCInputMapTypeNone, @"Should serialise and deserialise correctly");
    
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[4].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[4].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[4].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[4].controller.button.index, 4, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[4].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[5].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[5].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[5].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[5].controller.button.index, 5, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[5].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[6].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[6].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[6].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[6].controller.button.index, 6, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[6].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[7].type, CCInputMapTypeControllerButton, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[7].controller.device, 0, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[7].controller.connection, -1, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[7].controller.button.index, 7, @"Should serialise and deserialise correctly");
    XCTAssertEqual(ResultInputMap.virtual.joystick.inputMaps[7].controller.button.ramp, 0.0f, @"Should serialise and deserialise correctly");
    
    CCInputMapDestructor(&ResultInputMap);
    
    CCMemoryZoneDestroy(Zone);
    
    CCReflectOpaqueTypeDescriptorMap = CCReflectOpaqueTypeDescriptorMapDefaults;
    CCReflectOpaqueTypeDescriptorUnmap = CCReflectOpaqueTypeDescriptorUnmapDefaults;
}

@end
