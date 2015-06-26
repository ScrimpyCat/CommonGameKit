//
//  EntityManagerTests.m
//  Blob Game
//
//  Created by Stefan Johnson on 27/06/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "EntityManager.h"
#import "Entity.h"
#import "ComponentBase.h"
#import "ComponentSystem.h"

#define TEST_SYSTEM_ID 1
#define TEST_COMPONENT_ID 1

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    int value;
} TestComponentClass, *TestComponentPrivate;

static inline void TestComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    
    ((TestComponentPrivate)Component)->value = 0;
}

static inline void TestComponentSetValue(CCComponent Component, int Value)
{
    ((TestComponentPrivate)Component)->value = Value;
}

static inline int TestComponentGetValue(CCComponent Component)
{
    return ((TestComponentPrivate)Component)->value;
}

static void TestComponentSystemUpdateCallback(void *Context, CCCollection Components)
{
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(TEST_SYSTEM_ID));
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(TEST_SYSTEM_ID));
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Components, &Enumerator);
    
    for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
    {
        TestComponentSetValue(*Component, TestComponentGetValue(*Component) + 1);
    }
}

static _Bool TestComponentSystemHandlesComponentCallback(CCComponentID id)
{
    return id == TEST_COMPONENT_ID;
};


@interface EntityManagerTests : XCTestCase

@end

@implementation EntityManagerTests

-(void) setUp
{
    [super setUp];
    
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestComponentSystemUpdateCallback, TestComponentSystemHandlesComponentCallback, NULL, NULL, NULL, NULL, NULL);
    CCComponentRegister(TEST_COMPONENT_ID, "Test", CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize);
    CCEntityManagerCreate();
}

-(void) tearDown
{
    CCEntityManagerDestroy();
    CCComponentDeregister(TEST_COMPONENT_ID);
    CCComponentSystemDeregister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual);

    
    [super tearDown];
}

-(void) testAddingEntities
{
    CCEntity Entity = CCEntityCreate(1, CC_STD_ALLOCATOR);
    CCComponent Component = CCComponentCreate(TEST_COMPONENT_ID);
    
    CCEntityAttachComponent(Entity, Component);
    CCComponentSystemAddComponent(Component);
    
    CCEntityManagerAddEntity(Entity);
    CCEntityManagerUpdate();
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCEntityGetComponents(Entity), &Enumerator);
    
    XCTAssertEqual(TestComponentGetValue(*(CCComponent*)CCCollectionEnumeratorGetCurrent(&Enumerator)), 1, @"Should update the entity's component");
    
    
    CCEntityManagerRemoveEntity(Entity);
    CCEntityManagerUpdate();
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
}

@end
