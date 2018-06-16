/*
 *  Copyright (c) 2018, Stefan Johnson
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
#import "ScriptableInterfaceSystem.h"
#import "ScriptableInterfaceDynamicFieldComponent.h"
#import "Callbacks.h"
#import "ComponentExpressions.h"

#define TEST_SYSTEM_ID CCSystemIDAvailable
#define TEST_COMPONENT_ID 1

static const CCString TestComponentName = CC_STRING("Test");

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    int32_t value;
} TestComponentClass, *TestComponentPrivate;

static inline void TestComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    
    ((TestComponentPrivate)Component)->value = 0;
}

static inline int32_t TestComponentGetValue(CCComponent Component)
{
    return ((TestComponentPrivate)Component)->value;
}

static inline void TestComponentSetValue(CCComponent Component, int32_t Value)
{
    ((TestComponentPrivate)Component)->value = Value;
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("value:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeInt32, .setter = (CCComponentExpressionSetter)TestComponentSetValue }
};

static void TestComponentDeserializer(CCComponent Component, CCExpression Arg)
{
    CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)));
}

static const CCComponentExpressionDescriptor TestComponentDescriptor = {
    .id = TEST_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = TestComponentDeserializer,
    .serialize = NULL
};

@interface ScriptableInterfaceSystemTests : CCTestCase

@end

static void TestSystemUpdate(CCComponentSystemHandle *System, void *Context, CCCollection Components)
{
}

static _Bool TestSystemAddingComponentNonCallbackHandlesComponent(CCComponentSystemHandle *System, CCComponentID id)
{
    return id == TEST_COMPONENT_ID;
}

@implementation ScriptableInterfaceSystemTests

static double Timestamp(void)
{
    return 0.0;
}

-(void) setUp
{
    [super setUp];
    
    CCTimestamp = Timestamp;
    
    CCScriptableInterfaceSystemRegister();
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestSystemUpdate, NULL, TestSystemAddingComponentNonCallbackHandlesComponent, NULL, NULL, NULL, NULL, NULL);
    
    CCScriptableInterfaceDynamicFieldComponentRegister();
    CCComponentRegister(TEST_COMPONENT_ID, TestComponentName, CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize, NULL, NULL);
    
    CCComponentExpressionRegister(CC_STRING("scripitable-interface-system-test-component"), &TestComponentDescriptor, TRUE);
}

-(void) tearDown
{
    CCComponentDeregister(TEST_COMPONENT_ID);
    CCScriptableInterfaceDynamicFieldComponentDeregister();
    
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);
    CCScriptableInterfaceSystemDeregister();
    
    CCTimestamp = NULL;
    
    [super tearDown];
}

-(void) testChangingValue
{
    CCComponent TargetComponent = CCComponentCreate(TEST_COMPONENT_ID);
    TestComponentSetValue(TargetComponent, 1);
    CCComponentSystemAddComponent(TargetComponent);
    
    CCComponent DynamicFieldComponent = CCComponentCreate(CC_SCRIPTABLE_INTERFACE_DYNAMIC_FIELD_COMPONENT_ID);
    CCScriptableInterfaceDynamicFieldComponentSetTarget(DynamicFieldComponent, TargetComponent);
    CCScriptableInterfaceDynamicFieldComponentSetField(DynamicFieldComponent, CCExpressionCreateFromSource("(value: .value)"));
    CCExpression State = CCExpressionCreateFromSource("(begin (state! \".value\" 2))");
    CCExpressionEvaluate(State);
    CCScriptableInterfaceDynamicFieldComponentSetReferenceState(DynamicFieldComponent, State);
    CCComponentSystemAddComponent(DynamicFieldComponent);
    
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertEqual(1, TestComponentGetValue(TargetComponent), @"should remain unchanged");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeUpdate);
    XCTAssertEqual(1, TestComponentGetValue(TargetComponent), @"should remain unchanged");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertEqual(2, TestComponentGetValue(TargetComponent), @"should have changed");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeUpdate);
    XCTAssertEqual(2, TestComponentGetValue(TargetComponent), @"should remain unchanged");
    
    CCExpressionSetState(State, CC_STRING(".value"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, 3), FALSE);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertEqual(2, TestComponentGetValue(TargetComponent), @"should remain unchanged");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeUpdate);
    XCTAssertEqual(2, TestComponentGetValue(TargetComponent), @"should remain unchanged");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertEqual(3, TestComponentGetValue(TargetComponent), @"should have changed");
    
    
    CCExpressionDestroy(State);
    CCComponentDestroy(DynamicFieldComponent);
    CCComponentDestroy(TargetComponent);
}

@end
