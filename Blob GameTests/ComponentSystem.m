//
//  ComponentSystem.m
//  Blob Game
//
//  Created by Stefan Johnson on 23/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "ComponentSystem.h"


#define TEST_SYSTEM_ID 0

@interface ComponentSystem : XCTestCase

@end

@implementation ComponentSystem

-(void) setUp
{
    [super setUp];
    
    CCComponentRegister(CC_COMPONENT_ID, "Base", CC_STD_ALLOCATOR, sizeof(CCComponentClass), CCComponentInitialize);
}

-(void) tearDown
{
    CCComponentDeregister(CC_COMPONENT_ID);
    
    [super tearDown];
}

static _Bool TestSystemRegisteringUpdateCalled = FALSE;
static void TestSystemRegisteringUpdate(void *Context, CCComponentList *Components)
{
    TestSystemRegisteringUpdateCalled = TRUE;
}

-(void) testSystemRegistering
{
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestSystemRegisteringUpdate, NULL, NULL, NULL, NULL, NULL, NULL);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertTrue(TestSystemRegisteringUpdateCalled, @"Update callback should be called");
    
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);
}

static _Bool TestSystemAddingComponentNonCallbackHasActiveComponents, TestSystemAddingComponentNonCallbackHasAddedComponents, TestSystemAddingComponentNonCallbackHasRemovedComponents;
static void TestSystemAddingComponentNonCallbackUpdate(void *Context, CCComponentList *Components)
{
    TestSystemAddingComponentNonCallbackHasAddedComponents = CCComponentSystemGetAddedComponentsForSystem(TEST_SYSTEM_ID) != NULL;
    TestSystemAddingComponentNonCallbackHasRemovedComponents = CCComponentSystemGetRemovedComponentsForSystem(TEST_SYSTEM_ID) != NULL;
    TestSystemAddingComponentNonCallbackHasActiveComponents = *Components != NULL;
}

-(void) testSystemAddingComponentNonCallback
{
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestSystemAddingComponentNonCallbackUpdate, NULL, NULL, NULL, NULL, NULL, NULL);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should not have any active components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should not have any added components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should not have any removed components");

    
    
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);
}

@end
