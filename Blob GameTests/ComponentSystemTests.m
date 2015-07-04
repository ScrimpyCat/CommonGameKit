//
//  ComponentSystemTests.m
//  Blob Game
//
//  Created by Stefan Johnson on 23/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "ComponentSystem.h"
#import "ComponentBase.h"


#define TEST_SYSTEM_ID 1

@interface ComponentSystemTests : XCTestCase

@end

@implementation ComponentSystemTests

-(void) setUp
{
    [super setUp];
    
    CCComponentRegister(CC_COMPONENT_ID, "Base", CC_STD_ALLOCATOR, sizeof(CCComponentClass), CCComponentInitialize, NULL);
}

-(void) tearDown
{
    CCComponentDeregister(CC_COMPONENT_ID);
    
    [super tearDown];
}

static _Bool TestSystemRegisteringUpdateCalled = FALSE;
static void TestSystemRegisteringUpdate(void *Context, CCCollection Components)
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
static void TestSystemAddingComponentNonCallbackUpdate(void *Context, CCCollection Components)
{
    CCCollection Added = CCComponentSystemGetAddedComponentsForSystem(TEST_SYSTEM_ID), Removed = CCComponentSystemGetRemovedComponentsForSystem(TEST_SYSTEM_ID);
    TestSystemAddingComponentNonCallbackHasAddedComponents = CCCollectionGetCount(Added) != 0;
    TestSystemAddingComponentNonCallbackHasRemovedComponents = CCCollectionGetCount(Removed) != 0;
    TestSystemAddingComponentNonCallbackHasActiveComponents = CCCollectionGetCount(Components) != 0;
    
    CCCollectionDestroy(Added);
    CCCollectionDestroy(Removed);
}

static _Bool TestSystemAddingComponentNonCallbackHandlesComponent(CCComponentID id)
{
    return id == CC_COMPONENT_ID;
}

-(void) testSystemAddingComponentNonCallback
{
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestSystemAddingComponentNonCallbackUpdate, TestSystemAddingComponentNonCallbackHandlesComponent, NULL, NULL, NULL, NULL, NULL);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should not have any active components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should not have any added components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should not have any removed components");

    CCComponent Component = CCComponentCreate(CC_COMPONENT_ID);
    CCComponentSystemAddComponent(Component);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertTrue(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should have active components"); //as added gets added to active
    XCTAssertTrue(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should have added components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should not have any removed components");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertTrue(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should have active components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should not have any added components"); //as added has been cleared
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should not have any removed components");
    
    CCComponentSystemRemoveComponent(Component);
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should not have any active components"); //gets removed
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should not have any added components");
    XCTAssertTrue(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should have removed components");
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasActiveComponents, @"Should not have any active components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasAddedComponents, @"Should not have any added components");
    XCTAssertFalse(TestSystemAddingComponentNonCallbackHasRemovedComponents, @"Should not have any removed components"); //as removed has been cleared
    
    CCComponentDestroy(Component);
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);
}

static _Bool TestSystemAddingComponentCallbackHasActiveComponents;
static void TestSystemAddingComponentCallbackUpdate(void *Context, CCCollection Components)
{
    TestSystemAddingComponentCallbackHasActiveComponents = CCCollectionGetCount(Components) != 0;
}

static void TestSystemAddingComponentCallbackAddingComponent(CCComponent Component)
{
    
}

static void TestSystemAddingComponentCallbackRemovingComponent(CCComponent Component)
{
    
}

-(void) testSystemAddingComponentCallback
{
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestSystemAddingComponentCallbackUpdate, TestSystemAddingComponentNonCallbackHandlesComponent, TestSystemAddingComponentCallbackAddingComponent, TestSystemAddingComponentCallbackRemovingComponent, NULL, NULL, NULL);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentCallbackHasActiveComponents, @"Should not have any active components");
    
    CCComponent Component = CCComponentCreate(CC_COMPONENT_ID);
    CCComponentSystemAddComponent(Component);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertTrue(TestSystemAddingComponentCallbackHasActiveComponents, @"Should have active components");
        
    CCComponentSystemRemoveComponent(Component);
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    XCTAssertFalse(TestSystemAddingComponentCallbackHasActiveComponents, @"Should not have any active components");
    
    CCComponentDestroy(Component);
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);
}

@end
