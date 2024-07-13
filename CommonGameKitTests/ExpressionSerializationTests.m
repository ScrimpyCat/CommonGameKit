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



@interface ExpressionSerializationTests : CCTestCase
@end

@implementation ExpressionSerializationTests

-(void) testSerializingPointers
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    int *IntPtr = NULL;
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT(PTYPE(int, retain, dynamic)), &(int*){ &(int){ 1 } }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(PTYPE(int, retain, dynamic)), &IntPtr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(*IntPtr, 1, @"Should deserialize correctly");
    CCFree(IntPtr);
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(PTYPE(int, retain, dynamic)), &(int*){ NULL }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(PTYPE(int, retain, dynamic)), &IntPtr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(IntPtr, NULL, @"Should deserialize correctly");
    
    
    int **IntPtrPtr = NULL;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &(int**){ NULL }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &IntPtrPtr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(IntPtrPtr, NULL, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &(int**){ &(int*){ NULL } }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &IntPtrPtr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(*IntPtrPtr, NULL, @"Should deserialize correctly");
    CCFree(IntPtrPtr);
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &(int**){ &(int*){ &(int){ 1 } } }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_DYNAMIC_POINTER(&CC_REFLECT(PTYPE(int, retain, dynamic)), CCReflectOwnershipRetain), &IntPtrPtr, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(**IntPtrPtr, 1, @"Should deserialize correctly");
    CCFree(*IntPtrPtr);
    CCFree(IntPtrPtr);
    
    
    CCCollectionElementDestructor Destructor;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(CCCollectionElementDestructor), &(CCCollectionElementDestructor){ CCStringDestructorForCollection }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCCollectionElementDestructor), &Destructor, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Destructor, CCStringDestructorForCollection, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingIntegers
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    int32_t ValueI32 = -1;
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int32_t), &(int32_t){ 0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int32_t), &ValueI32, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI32, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int32_t), &(int32_t){ INT32_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int32_t), &ValueI32, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI32, INT32_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int32_t), &(int32_t){ INT32_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int32_t), &ValueI32, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI32, INT32_MIN, @"Should deserialize correctly");
    
    
    int16_t ValueI16 = -1;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int16_t), &(int16_t){ 0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int16_t), &(int16_t){ INT16_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, INT16_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int16_t), &(int16_t){ INT16_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, INT16_MIN, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, 3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, 3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, -3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, INT16_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, INT16_MIN, @"Should deserialize correctly");

    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, NAN);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int16_t), &ValueI16, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI16, 0, @"Should deserialize correctly");
    
    
    int8_t ValueI8 = -1;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int8_t), &(int8_t){ 0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int8_t), &(int8_t){ INT8_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, INT8_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int8_t), &(int8_t){ INT8_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, INT8_MIN, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, 3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, 3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, -3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, INT8_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, INT8_MIN, @"Should deserialize correctly");

    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, NAN);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int8_t), &ValueI8, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI8, 0, @"Should deserialize correctly");
    
    
    int64_t ValueI64 = -1;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int64_t), &(int64_t){ 0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int64_t), &(int64_t){ INT64_MAX }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, INT64_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(int64_t), &(int64_t){ INT64_MIN }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, INT64_MIN, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, 3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, 3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -3.5f);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, -3, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, INT64_MAX, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, -INFINITY);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, INT64_MIN, @"Should deserialize correctly");

    
    Expr = CCExpressionCreateFloat(CC_STD_ALLOCATOR, NAN);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(int64_t), &ValueI64, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueI64, 0, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingFloats
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    float ValueF = -1;
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT(float), &(float){ 0.0f }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(float), &ValueF, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueF, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(float), &(float){ 0.5f }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(float), &ValueF, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueF, 0.5f, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(float), &(float){ INFINITY }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(float), &ValueF, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueF, INFINITY, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, -5);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(float), &ValueF, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueF, -5.0f, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("0xc0000000"), TRUE);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(float), &ValueF, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueF, -2.0f, @"Should deserialize correctly");
    
    
    double ValueD = -1;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(double), &(double){ 0.0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(double), &ValueD, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueD, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(double), &(double){ 0.5 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(double), &ValueD, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueD, 0.5f, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(double), &(double){ INFINITY }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(double), &ValueD, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueD, INFINITY, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, -5);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(double), &ValueD, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueD, -5.0f, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateString(CC_STD_ALLOCATOR, CC_STRING("0xc000000000000000"), TRUE);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(double), &ValueD, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueD, -2.0f, @"Should deserialize correctly");
    
    
    long double ValueL = -1;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(long double), &(long double){ 0.0 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(long double), &ValueL, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueL, 0, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(long double), &(long double){ 0.5 }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(long double), &ValueL, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueL, 0.5f, @"Should deserialize correctly");
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(long double), &(long double){ INFINITY }, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(long double), &ValueL, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueL, INFINITY, @"Should deserialize correctly");
    
    
    Expr = CCExpressionCreateInteger(CC_STD_ALLOCATOR, -5);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(long double), &ValueL, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(ValueL, -5.0f, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingStructs
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT(CCReflectInteger), &CC_REFLECT(int), Zone);
    CCReflectInteger Integer;
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCReflectInteger), &Integer, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Integer.size, CC_REFLECT(int).size, @"Should deserialize correctly");
    XCTAssertEqual(Integer.endian, CC_REFLECT(int).endian, @"Should deserialize correctly");
    XCTAssertEqual(Integer.fixed, CC_REFLECT(int).fixed, @"Should deserialize correctly");
    XCTAssertEqual(Integer.sign, CC_REFLECT(int).sign, @"Should deserialize correctly");
    XCTAssertEqual(Integer.id, CC_REFLECT(int).id, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingOpaques
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT(CCString), &(CCString){ CC_STRING("foo") }, Zone);
    CCString Str = 0;
    
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCString), &Str, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertTrue(CCStringEqual(Str, CC_STRING("foo")), @"Should deserialize correctly");
    CCStringDestroy(Str);
    
    
    Expr = CCExpressionCreateFromSource("( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 3 ) ) ( 102 111 111 ) )");
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCString), &Str, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertTrue(CCStringEqual(Str, CC_STRING("foo")), @"Should deserialize correctly");
    CCStringDestroy(Str);
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(CCString), &(CCString){ CC_STRING("a very long string 1234567890") }, Zone);
    Str = 0;
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCString), &Str, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertTrue(CCStringEqual(Str, CC_STRING("a very long string 1234567890")), @"Should deserialize correctly");
    CCStringDestroy(Str);
    
    
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT(CCString), &(CCString){ 0 }, Zone);
    Str = -1;
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT(CCString), &Str, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Str, 0, @"Should deserialize correctly");
    
    
    CCDictionaryCallbacks Callbacks = {
        .getHash = CCStringHasherForDictionary,
        .compareKeys = CCStringComparatorForDictionary,
        .keyDestructor = CCStringDestructorForDictionary
    };
    
    CCDictionary Dictionary = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, sizeof(CCString), sizeof(int), &Callbacks);
    
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("one") }, &(int){ 1 });
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("two") }, &(int){ 2 });
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }, &(int){ 3 });
    
    CCMemoryZoneSave(Zone);
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), &Dictionary, Zone);
    CCDictionaryDestroy(Dictionary);
    
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), &Dictionary, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("one") }), 1, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("two") }), 2, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }), 3, @"Should deserialize correctly");
    
    CCDictionaryDestroy(Dictionary);
    
    
    Expr = CCExpressionCreateFromSource("( ( :struct ( size: 160 ) ( fields: ( ( ( name: \"keyEnumerable\" ) ( offset: 0 ) ( type: ( :enumerable ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 3 ) ) ) ) ( count: 3 ) ) ) ) ( ( name: \"valueEnumerable\" ) ( offset: 80 ) ( type: ( :enumerable ( type: ( :integer ( sign: 1 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ( count: 3 ) ) ) ) ) ) ) ( ( keyEnumerable: ( ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 32 ) ) ( 116 104 114 101 101 32 105 115 32 97 32 118 101 114 121 32 118 101 114 121 32 108 111 110 103 32 115 116 114 105 110 103 ) ) ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 3 ) ) ( 111 110 101 ) ) ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 3 ) ) ( 116 119 111 ) ) ) ) ( valueEnumerable: ( 3 1 2 ) ) ) )");
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), &Dictionary, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("one") }), 1, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("two") }), 2, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }), 3, @"Should deserialize correctly");
    
    CCDictionaryDestroy(Dictionary);
    
    
    CCOrderedCollection Collection = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, sizeof(CCDictionary), CCDictionaryDestructorForCollection);
    
    Dictionary = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, sizeof(CCString), sizeof(int), &Callbacks);
    
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("one") }, &(int){ 1 });
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("two") }, &(int){ 2 });
    CCDictionarySetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }, &(int){ 3 });
    
    CCOrderedCollectionAppendElement(Collection, &Dictionary);
    
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_CCCollection(&CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, CCDictionaryDestructorForCollection), &Collection, Zone);
    CCCollectionDestroy(Collection);
    
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_CCCollection(&CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, CCDictionaryDestructorForCollection), &Collection, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(CCCollectionGetCount(Collection), 1, @"Should deserialize correctly");
    
    Dictionary = *(CCDictionary*)CCOrderedCollectionGetElementAtIndex(Collection, 0);
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("one") }), 1, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("two") }), 2, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }), 3, @"Should deserialize correctly");
    
    CCCollectionDestroy(Collection);
    
    
    Expr = CCExpressionCreateFromSource("( ( :enumerable ( type: ( :opaque ( type-size: 88 ) ( descriptor: ( ( :struct ( size: 56 ) ( fields: ( ( ( name: \"name\" ) ( offset: 0 ) ( type: ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) ) ) ( ( name: \"data.keyType\" ) ( offset: 8 ) ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 1 ) ) ) ) ) ( ( name: \"data.valueType\" ) ( offset: 16 ) ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 1 ) ) ) ) ) ( ( name: \"data.allocator\" ) ( offset: 24 ) ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 2 ) ) ) ) ) ( ( name: \"data.hint\" ) ( offset: 40 ) ( type: ( :integer ( sign: 1 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ) ( ( name: \"data.callbacks\" ) ( offset: 48 ) ( type: ( :pointer ( type: ( :struct ( size: 32 ) ( fields: ( ( ( name: \"keyDestructor\" ) ( offset: 0 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 7 ) ) ) ) ) ( ( name: \"valueDestructor\" ) ( offset: 8 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 7 ) ) ) ) ) ( ( name: \"getHash\" ) ( offset: 16 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 8 ) ) ) ) ) ( ( name: \"compareKeys\" ) ( offset: 24 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 4 ) ) ) ) ) ) ) ) ) ( storage: 1 ) ( ownership: 0 ) ) ) ) ) ) ) ( ( name: 5 ) ( data.keyType: ( ( :pointer ( type: ( :struct ( size: 40 ) ( fields: ( ( ( name: \"id\" ) ( offset: 0 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ) ( ( name: \"size\" ) ( offset: 8 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 8 ) ) ) ) ( ( name: \"typeSize\" ) ( offset: 16 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 8 ) ) ) ) ( ( name: \"map\" ) ( offset: 24 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ) ( ( name: \"unmap\" ) ( offset: 32 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 0 ) ( ownership: 0 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 1 ) ) ) ) ) ) ) ) ) ( storage: 1 ) ( ownership: 0 ) ) ( ( ( id: 4 ) ( size: 8 ) ( typeSize: 40 ) ( map: ( ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ( unmap: ( ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ) ) ) ( data.valueType: ( ( :pointer ( type: ( :struct ( size: 24 ) ( fields: ( ( ( name: \"id\" ) ( offset: 0 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ) ( ( name: \"size\" ) ( offset: 8 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 8 ) ) ) ) ( ( name: \"endian\" ) ( offset: 16 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ) ( ( name: \"sign\" ) ( offset: 20 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ) ( ( name: \"fixed\" ) ( offset: 21 ) ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ) ) ) ) ) ( storage: 1 ) ( ownership: 0 ) ) ( ( ( id: 1 ) ( size: 4 ) ( endian: 0 ) ( sign: 1 ) ( fixed: 0 ) ) ) ) ) ( data.allocator: ( ( :struct ( size: 16 ) ( fields: ( ( ( name: \"allocator\" ) ( offset: 0 ) ( type: ( :integer ( sign: 1 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ) ( ( name: \"data\" ) ( offset: 8 ) ( type: ( :pointer ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ( storage: 1 ) ( ownership: 0 ) ) ) ) ) ) ) ( ( allocator: 1 ) ( data: ( ) ) ) ) ) ( data.hint: 1073741824 ) ( data.callbacks: ( ( ( keyDestructor: ( ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 1 ) ) ) ( valueDestructor: ( ) ) ( getHash: ( ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ( compareKeys: ( ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 0 ) ) ) ) ) ) ) ) ) ) ) ( count: 1 ) ) ( ( ( :struct ( size: 160 ) ( fields: ( ( ( name: \"keyEnumerable\" ) ( offset: 0 ) ( type: ( :enumerable ( type: ( :opaque ( type-size: 40 ) ( descriptor: ( ( :integer ( sign: 0 ) ( fixed: 1 ) ( endian: 0 ) ( size: 1 ) ) 3 ) ) ) ) ( count: 3 ) ) ) ) ( ( name: \"valueEnumerable\" ) ( offset: 80 ) ( type: ( :enumerable ( type: ( :integer ( sign: 1 ) ( fixed: 0 ) ( endian: 0 ) ( size: 4 ) ) ) ( count: 3 ) ) ) ) ) ) ) ( ( keyEnumerable: ( ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 32 ) ) ( 116 104 114 101 101 32 105 115 32 97 32 118 101 114 121 32 118 101 114 121 32 108 111 110 103 32 115 116 114 105 110 103 ) ) ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 3 ) ) ( 111 110 101 ) ) ( ( :array ( type: ( :integer ( sign: 0 ) ( fixed: 0 ) ( endian: 0 ) ( size: 1 ) ) ) ( count: 3 ) ) ( 116 119 111 ) ) ) ) ( valueEnumerable: ( 3 1 2 ) ) ) ) ) )");
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_CCCollection(&CC_REFLECT_CCDictionary(&CC_REFLECT(CCString), &CC_REFLECT(int), CC_STD_ALLOCATOR, CCDictionaryHintSizeSmall, &Callbacks), CC_STD_ALLOCATOR, CCCollectionHintOrdered | CCCollectionHintSizeSmall, CCDictionaryDestructorForCollection), &Collection, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(CCCollectionGetCount(Collection), 1, @"Should deserialize correctly");
    
    Dictionary = *(CCDictionary*)CCOrderedCollectionGetElementAtIndex(Collection, 0);
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("one") }), 1, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("two") }), 2, @"Should deserialize correctly");
    XCTAssertEqual(*(int*)CCDictionaryGetValue(Dictionary, &(CCString){ CC_STRING("three is a very very long string") }), 3, @"Should deserialize correctly");
    
    CCCollectionDestroy(Collection);
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingArrays
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    int Array[3] = { 1, 2, 3 };
    
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), Array, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_ARRAY(&CC_REFLECT(int), 3), Array, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Array[0], 1, @"Should deserialize correctly");
    XCTAssertEqual(Array[1], 2, @"Should deserialize correctly");
    XCTAssertEqual(Array[2], 3, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

-(void) testSerializingValidations
{
    CCMemoryZone Zone = CCMemoryZoneCreate(CC_STD_ALLOCATOR, 1024);
    
    int Value = 5;
    CCExpression Expr = CCExpressionReflectionSerialize(&CC_REFLECT_VALIDATE_MINIMUM(&CC_REFLECT(int), CC_REFLECT_VALUE(&CC_REFLECT(int), &(int){ 10 }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT(&CC_REFLECT(int), &(int){ 10 })), &Value, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_VALIDATE_MINIMUM(&CC_REFLECT(int), CC_REFLECT_VALUE(&CC_REFLECT(int), &(int){ 10 }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT(&CC_REFLECT(int), &(int){ 10 })), &Value, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Value, 10, @"Should deserialize correctly");
    
    
    Value = 15;
    Expr = CCExpressionReflectionSerialize(&CC_REFLECT_VALIDATE_MINIMUM(&CC_REFLECT(int), CC_REFLECT_VALUE(&CC_REFLECT(int), &(int){ 10 }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT(&CC_REFLECT(int), &(int){ 10 })), &Value, Zone);
    CCExpressionReflectionDeserialize(Expr, &CC_REFLECT_VALIDATE_MINIMUM(&CC_REFLECT(int), CC_REFLECT_VALUE(&CC_REFLECT(int), &(int){ 10 }), CC_REFLECT_RESULT_DEFAULT, CC_REFLECT_RESULT(&CC_REFLECT(int), &(int){ 10 })), &Value, Zone, CC_STD_ALLOCATOR);
    CCExpressionDestroy(Expr);
    
    XCTAssertEqual(Value, 15, @"Should deserialize correctly");
    
    CCMemoryZoneDestroy(Zone);
}

@end
