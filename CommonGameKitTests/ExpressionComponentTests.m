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
#import "ExpressionEvaluator.h"
#import "ComponentExpressions.h"
#import "InputMapGroupComponent.h"
#import "InputMapKeyboardComponent.h"

@interface ExpressionComponentTests : CCTestCase

@end

@implementation ExpressionComponentTests

static void KeyboardCallback()
{
}

-(void) setUp
{
    [super setUp];
    
    CCInputMapGroupComponentRegister();
    CCInputMapKeyboardComponentRegister();
    
    CCInputMapComponentRegisterCallback(CC_STRING(":test"), CCInputMapTypeKeyboard, KeyboardCallback);
}

static char Set[256];
static void SetI8(CCComponent Component, int8_t Value){ snprintf(Set, sizeof(Set), "%d", Value); }
static void SetI16(CCComponent Component, int16_t Value){ snprintf(Set, sizeof(Set), "%d", Value); }
static void SetI32(CCComponent Component, int32_t Value){ snprintf(Set, sizeof(Set), "%d", Value); }
static void SetI64(CCComponent Component, int64_t Value){ snprintf(Set, sizeof(Set), "%lld", Value); }
static void SetU8(CCComponent Component, uint8_t Value){ snprintf(Set, sizeof(Set), "%u", Value); }
static void SetU16(CCComponent Component, uint16_t Value){ snprintf(Set, sizeof(Set), "%u", Value); }
static void SetU32(CCComponent Component, uint32_t Value){ snprintf(Set, sizeof(Set), "%u", Value); }
static void SetU64(CCComponent Component, uint64_t Value){ snprintf(Set, sizeof(Set), "%llu", Value); }
static void SetStr(CCComponent Component, CCString Value){ CC_STRING_TEMP_BUFFER(Buffer, Value) snprintf(Set, sizeof(Set), "%s", Buffer); CCStringDestroy(Value); }
static void SetBool(CCComponent Component, _Bool Value){ snprintf(Set, sizeof(Set), "%s", Value ? "true" : "false"); }
static void SetF32(CCComponent Component, float Value){ snprintf(Set, sizeof(Set), "%.2f", Value); }
static void SetF64(CCComponent Component, double Value){ snprintf(Set, sizeof(Set), "%.2f", Value); }
static void SetColour(CCComponent Component, CCColourRGBA Value){ snprintf(Set, sizeof(Set), "%.2f, %.2f, %.2f, %.2f", Value.r, Value.g, Value.b, Value.a); }
static void SetData(CCComponent Component, int *Value){ snprintf(Set, sizeof(Set), "->%d", *Value); CCFree(Value); }

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("i8:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt8, .setter = (CCComponentExpressionSetter)SetI8 },
    { .name = CC_STRING("i16:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt16, .setter = (CCComponentExpressionSetter)SetI16 },
    { .name = CC_STRING("i32:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt32, .setter = (CCComponentExpressionSetter)SetI32 },
    { .name = CC_STRING("i64:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt64, .setter = (CCComponentExpressionSetter)SetI64 },
    { .name = CC_STRING("u8:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt8, .setter = (CCComponentExpressionSetter)SetU8 },
    { .name = CC_STRING("u16:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt16, .setter = (CCComponentExpressionSetter)SetU16 },
    { .name = CC_STRING("u32:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt32, .setter = (CCComponentExpressionSetter)SetU32 },
    { .name = CC_STRING("u64:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt64, .setter = (CCComponentExpressionSetter)SetU64 },
    { .name = CC_STRING("str:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeString, .setter = (CCComponentExpressionSetter)SetStr },
    { .name = CC_STRING("bool:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeBool, .setter = (CCComponentExpressionSetter)SetBool },
    { .name = CC_STRING("f32:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat32, .setter = (CCComponentExpressionSetter)SetF32 },
    { .name = CC_STRING("f64:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat64, .setter = (CCComponentExpressionSetter)SetF64 },
    { .name = CC_STRING("colour:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeColour, .setter = (CCComponentExpressionSetter)SetColour },
    { .name = CC_STRING("data:"), .serializedType = 'test', .setterType = CCComponentExpressionArgumentTypeData, .setter = (CCComponentExpressionSetter)SetData }
};

static size_t DestroyCount = 0;
static void TestDataDestructor(void *Ptr)
{
    DestroyCount++;
}

static CCExpression TestDataEvaluator(CCExpression Expression)
{
    int *Data;
    CC_SAFE_Malloc(Data, sizeof(int));
    
    CCMemorySetDestructor(Data, TestDataDestructor);
    
    *Data = CCExpressionGetInteger(CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1)));
    
    return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, 'test', Data, NULL, CCFree);
}

-(void) testDeserialization
{
#define TEST_DESERIALIZE_SUCCESS(expression, result) \
do { \
    Set[0] = 0; \
    CCExpression Arg = CCExpressionCreateFromSource(expression); \
    XCTAssertTrue(CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), FALSE), @"Should deserialize"); \
    XCTAssertTrue(!strcmp(Set, result), @"Should have the result (%s) got (%s)", result, Set); \
    CCExpressionDestroy(Arg); \
} while(0)
#define TEST_EVALUATE_DESERIALIZE_SUCCESS(expression, result) \
do { \
    Set[0] = 0; \
    CCExpression Arg = CCExpressionCreateFromSource(expression); \
    XCTAssertTrue(CCComponentExpressionDeserializeArgument(Component, CCExpressionEvaluate(Arg), Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), FALSE), @"Should deserialize"); \
    XCTAssertTrue(!strcmp(Set, result), @"Should have the result (%s) got (%s)", result, Set); \
    CCExpressionDestroy(Arg); \
} while(0)
#define TEST_DESERIALIZE_FAILURE(expression) \
do { \
    Set[0] = 0; \
    CCExpression Arg = CCExpressionCreateFromSource(expression); \
    XCTAssertFalse(CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), FALSE), @"Should not deserialize"); \
    XCTAssertTrue(!strcmp(Set, "")); \
    CCExpressionDestroy(Arg); \
} while(0)
    CCComponent Component = CCComponentCreate(CC_INPUT_MAP_GROUP_COMPONENT_ID);
    
    TEST_DESERIALIZE_FAILURE("(i8: :d)");
    TEST_DESERIALIZE_SUCCESS("(i8: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(i8: -1)", "-1");
    TEST_DESERIALIZE_SUCCESS("(i8: 127)", "127");
    TEST_DESERIALIZE_SUCCESS("(i8: -128)", "-128");
    TEST_DESERIALIZE_SUCCESS("(i8: 256)", "0");

    TEST_DESERIALIZE_FAILURE("(i16: :d)");
    TEST_DESERIALIZE_SUCCESS("(i16: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(i16: -1)", "-1");
    TEST_DESERIALIZE_SUCCESS("(i16: 32767)", "32767");
    TEST_DESERIALIZE_SUCCESS("(i16: -32768)", "-32768");
    TEST_DESERIALIZE_SUCCESS("(i16: 65536)", "0");

    TEST_DESERIALIZE_FAILURE("(i32: :d)");
    TEST_DESERIALIZE_SUCCESS("(i32: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(i32: -1)", "-1");
    TEST_DESERIALIZE_SUCCESS("(i32: 2147483647)", "2147483647");
    TEST_DESERIALIZE_SUCCESS("(i32: -2147483648)", "-2147483648");
    TEST_DESERIALIZE_SUCCESS("(i32: 4294967296)", "0");

    TEST_DESERIALIZE_FAILURE("(i64: :d)");
    TEST_DESERIALIZE_SUCCESS("(i64: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(i64: -1)", "-1");
    TEST_DESERIALIZE_SUCCESS("(i64: 2147483647)", "2147483647");
    TEST_DESERIALIZE_SUCCESS("(i64: -2147483648)", "-2147483648");
    TEST_DESERIALIZE_SUCCESS("(i64: 4294967296)", "0");

    TEST_DESERIALIZE_FAILURE("(u8: :d)");
    TEST_DESERIALIZE_SUCCESS("(u8: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(u8: -1)", "255");
    TEST_DESERIALIZE_SUCCESS("(u8: 127)", "127");
    TEST_DESERIALIZE_SUCCESS("(u8: -128)", "128");
    TEST_DESERIALIZE_SUCCESS("(u8: 256)", "0");

    TEST_DESERIALIZE_FAILURE("(u16: :d)");
    TEST_DESERIALIZE_SUCCESS("(u16: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(u16: -1)", "65535");
    TEST_DESERIALIZE_SUCCESS("(u16: 32767)", "32767");
    TEST_DESERIALIZE_SUCCESS("(u16: -32768)", "32768");
    TEST_DESERIALIZE_SUCCESS("(u16: 65536)", "0");

    TEST_DESERIALIZE_FAILURE("(u32: :d)");
    TEST_DESERIALIZE_SUCCESS("(u32: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(u32: -1)", "4294967295");
    TEST_DESERIALIZE_SUCCESS("(u32: 2147483647)", "2147483647");
    TEST_DESERIALIZE_SUCCESS("(u32: -2147483648)", "2147483648");
    TEST_DESERIALIZE_SUCCESS("(u32: 4294967296)", "0");

    TEST_DESERIALIZE_FAILURE("(u64: :d)");
    TEST_DESERIALIZE_SUCCESS("(u64: 1)", "1");
    TEST_DESERIALIZE_SUCCESS("(u64: -1)", "18446744073709551615");
    TEST_DESERIALIZE_SUCCESS("(u64: 2147483647)", "2147483647");
    TEST_DESERIALIZE_SUCCESS("(u64: -2147483648)", "18446744071562067968");
    TEST_DESERIALIZE_SUCCESS("(u64: 4294967296)", "0");

    TEST_DESERIALIZE_FAILURE("(str: :d)");
    TEST_DESERIALIZE_FAILURE("(str: 1)");
    TEST_DESERIALIZE_SUCCESS("(str: \"foo\")", "foo");
    TEST_DESERIALIZE_SUCCESS("(str: \"bar\")", "bar");
    TEST_DESERIALIZE_SUCCESS("(str: \"\")", "");

    TEST_DESERIALIZE_FAILURE("(bool: :d)");
    TEST_DESERIALIZE_SUCCESS("(bool: 1)", "true");
    TEST_DESERIALIZE_SUCCESS("(bool: #f)", "false");
    TEST_DESERIALIZE_SUCCESS("(bool: #t)", "true");
    TEST_DESERIALIZE_SUCCESS("(bool: 10)", "true");
    TEST_DESERIALIZE_SUCCESS("(bool: 0)", "false");
    TEST_DESERIALIZE_SUCCESS("(bool: -1)", "true");

    TEST_DESERIALIZE_FAILURE("(f32: :d)");
    TEST_DESERIALIZE_FAILURE("(f32: 1)");
    TEST_DESERIALIZE_SUCCESS("(f32: -1.0)", "-1.00");
    TEST_DESERIALIZE_SUCCESS("(f32: 1.5)", "1.50");
    TEST_DESERIALIZE_SUCCESS("(f32: 50.12)", "50.12");

    TEST_DESERIALIZE_FAILURE("(f64: :d)");
    TEST_DESERIALIZE_FAILURE("(f64: 1)");
    TEST_DESERIALIZE_SUCCESS("(f64: -1.0)", "-1.00");
    TEST_DESERIALIZE_SUCCESS("(f64: 1.5)", "1.50");
    TEST_DESERIALIZE_SUCCESS("(f64: 50.12)", "50.12");

    TEST_DESERIALIZE_FAILURE("(colour: :d)");
    TEST_DESERIALIZE_FAILURE("(colour: 1)");
    TEST_DESERIALIZE_FAILURE("(colour: 1.0)");
    TEST_DESERIALIZE_SUCCESS("(colour: 255 255 255 255)", "1.00, 1.00, 1.00, 1.00");
    TEST_DESERIALIZE_SUCCESS("(colour: 0 64 127 191)", "0.00, 0.25, 0.50, 0.75");
    TEST_DESERIALIZE_SUCCESS("(colour: 0 64 127)", "0.00, 0.25, 0.50, 1.00");
    TEST_DESERIALIZE_SUCCESS("(colour: 1.0 0.75 0.5 0.25)", "1.00, 0.75, 0.50, 0.25");
    TEST_DESERIALIZE_SUCCESS("(colour: 1.0 0.75 0.5)", "1.00, 0.75, 0.50, 1.00");
    TEST_DESERIALIZE_SUCCESS("(colour: (255 255 255 255))", "1.00, 1.00, 1.00, 1.00");
    TEST_DESERIALIZE_SUCCESS("(colour: (0 64 127 191))", "0.00, 0.25, 0.50, 0.75");
    TEST_DESERIALIZE_SUCCESS("(colour: (0 64 127))", "0.00, 0.25, 0.50, 1.00");
    TEST_DESERIALIZE_SUCCESS("(colour: (1.0 0.75 0.5 0.25))", "1.00, 0.75, 0.50, 0.25");
    TEST_DESERIALIZE_SUCCESS("(colour: (1.0 0.75 0.5))", "1.00, 0.75, 0.50, 1.00");
    
    CCExpressionEvaluatorRegister(CC_STRING("test-data"), TestDataEvaluator);
    TEST_DESERIALIZE_FAILURE("(data: :d)");
    TEST_DESERIALIZE_FAILURE("(data: 1)");
    TEST_DESERIALIZE_FAILURE("(data: 1.0)");
    TEST_DESERIALIZE_FAILURE("(data: (test-data 1))");
    TEST_EVALUATE_DESERIALIZE_SUCCESS("(data: (test-data 1))", "->1");
    TEST_EVALUATE_DESERIALIZE_SUCCESS("(data: (test-data 2))", "->2");
    XCTAssertEqual(DestroyCount, 2, @"Should have destroyed the data");
    
    CCComponentDestroy(Component);
}

-(void) testInputMapGroup
{
    CCExpression Expr = CCExpressionCreateFromSource("(input-group (action: \"test\") (input: (input-keyboard (keycode: :left)) (input-keyboard (keycode: :right))) (all-active: #t))");
    CCExpression Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    CCComponent Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_GROUP_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapGroupComponentGetWantsAllActive(Component), @"Should initialize correctly");
    XCTAssertTrue(CCStringEqual(CCInputMapComponentGetAction(Component), CC_STRING("test")), @"Should initialize correctly");
    XCTAssertEqual(CCCollectionGetCount(CCInputMapGroupComponentGetInputMaps(Component)), 2, @"Should be correct component");
    
    CCKeyboardKeycode Keycode[2] = { CCKeyboardKeycodeLeft, CCKeyboardKeycodeRight };
    CC_COLLECTION_FOREACH(CCComponent, Key, CCInputMapGroupComponentGetInputMaps(Component))
    {
        XCTAssertEqual(CCComponentGetID(Key), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
        
        XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Key), @"Should initialize correctly");
        XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Key), Keycode[CCOrderedCollectionGetIndex(CCInputMapGroupComponentGetInputMaps(Component), CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR))], @"Should initialize correctly");
        XCTAssertTrue(CCInputMapKeyboardComponentGetIgnoreModifier(Key), @"Should initialize correctly");
        XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Key), @"Should initialize correctly");
        XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Key), 0.0f, @"Should initialize correctly");
    }
    
    CCExpressionDestroy(Expr);
}

-(void) testInputMapKeyboard
{
    CCExpression Expr = CCExpressionCreateFromSource("(input-keyboard (keycode: :any) (repeat: #t) (action: \"test\") (ramp: 0.5))");
    CCExpression Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    CCComponent Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Component), CCInputMapKeyboardComponentKeycodeAny, @"Should initialize correctly");
    XCTAssertTrue(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertTrue(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertTrue(CCStringEqual(CCInputMapComponentGetAction(Component), CC_STRING("test")), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.5f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
    
    
    Expr = CCExpressionCreateFromSource("(input-keyboard (char: \"C\") (callback: :test) (flags: :shift :alt))");
    Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertFalse(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetCharacter(Component), 'C', @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetFlags(Component), CCKeyboardModifierShift | CCKeyboardModifierAlt, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetCallback(Component), &KeyboardCallback, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.0f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
    
    
    Expr = CCExpressionCreateFromSource("(input-keyboard (keycode: :a) (flags: :none))");
    Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Component), CCKeyboardKeycodeA, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetFlags(Component), 0, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetAction(Component), 0, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetCallback(Component), NULL, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.0f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
}

@end
