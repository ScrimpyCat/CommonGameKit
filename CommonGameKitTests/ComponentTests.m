/*
 *  Copyright (c) 2015 Stefan Johnson
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

@import Cocoa;
@import XCTest;
#import "Component.h"
#import "ComponentBase.h"


#define TEST_COMPONENT_ID 1

const char * const TestComponentName = "Test";

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    int value;
} TestComponentClass, *TestComponentPrivate;

static inline void TestComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    
    ((TestComponentPrivate)Component)->value = 15;
}

static inline int TestComponentGetValue(CCComponent Component)
{
    return ((TestComponentPrivate)Component)->value;
}


@interface ComponentTests : XCTestCase

@end

@implementation ComponentTests

-(void) setUp
{
    [super setUp];
}

-(void) tearDown
{
    [super tearDown];
}

-(void) testComponentRegistering
{
    XCTAssertEqual(CCComponentCreate(TEST_COMPONENT_ID), NULL, "Test component has not yet been registered and should not be available");
    XCTAssertEqual(CCComponentCreateForName(TestComponentName), NULL, "Test component has not yet been registered and should not be available");
    
    CCComponentRegister(TEST_COMPONENT_ID, TestComponentName, CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize, NULL);
    
    CCComponent a = CCComponentCreate(TEST_COMPONENT_ID), b = CCComponentCreateForName(TestComponentName);
    XCTAssertNotEqual(a, NULL, "Test component has been registered and should be available");
    XCTAssertNotEqual(b, NULL, "Test component has been registered and should be available");
    
    XCTAssertEqual(CCComponentGetID(a), TEST_COMPONENT_ID, @"Should be the correct component");
    XCTAssertEqual(CCComponentGetID(b), TEST_COMPONENT_ID, @"Should be the correct component");
    XCTAssertEqual(TestComponentGetValue(a), 15, @"Should be the correct component");
    XCTAssertEqual(TestComponentGetValue(b), 15, @"Should be the correct component");
    
    CCComponentDestroy(a);
    CCComponentDestroy(b);
    
    CCComponentDeregister(TEST_COMPONENT_ID);
}

-(void) testComponentCreationForID
{
    CCComponentRegister(CC_COMPONENT_ID, "Base", CC_STD_ALLOCATOR, sizeof(CCComponentClass), CCComponentInitialize, NULL);
    CCComponentRegister(TEST_COMPONENT_ID, TestComponentName, CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize, NULL);
    
    XCTAssertEqual(CCComponentCreate(2), NULL, "No component with id 2 has been registered and should not be available");
    
    CCComponent a = CCComponentCreate(CC_COMPONENT_ID), b = CCComponentCreate(TEST_COMPONENT_ID);
    XCTAssertNotEqual(a, NULL, "Component with id %u has been registered and should be available", CC_COMPONENT_ID);
    XCTAssertNotEqual(b, NULL, "Component with id %u has been registered and should be available", TEST_COMPONENT_ID);
    
    CCComponentDestroy(a);
    CCComponentDestroy(b);
    
    CCComponentDeregister(CC_COMPONENT_ID);
    CCComponentDeregister(TEST_COMPONENT_ID);
}

-(void) testComponentCreationForName
{
    CCComponentRegister(CC_COMPONENT_ID, "Base", CC_STD_ALLOCATOR, sizeof(CCComponentClass), CCComponentInitialize, NULL);
    CCComponentRegister(TEST_COMPONENT_ID, TestComponentName, CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize, NULL);
    
    XCTAssertEqual(CCComponentCreateForName("blah"), NULL, "No component with name \"blah\" has been registered and should not be available");
    
    CCComponent a = CCComponentCreateForName("Base"), b = CCComponentCreateForName(TestComponentName);
    XCTAssertNotEqual(a, NULL, "Component with name \"%s\" has been registered and should be available", "Base");
    XCTAssertNotEqual(b, NULL, "Component with name \"%s\" has been registered and should be available", TestComponentName);
    
    CCComponentDestroy(a);
    CCComponentDestroy(b);
    
    CCComponentDeregister(CC_COMPONENT_ID);
    CCComponentDeregister(TEST_COMPONENT_ID);
}

@end
