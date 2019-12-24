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

static void TestComponentSystemUpdateCallback(CCComponentSystemHandle *System, void *Context, CCCollection Components)
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

static _Bool TestComponentSystemHandlesComponentCallback(CCComponentSystemHandle *System, CCComponentID id)
{
    return id == TEST_COMPONENT_ID;
};


@interface EntityManagerTests : XCTestCase

@end

@implementation EntityManagerTests

-(void) setUp
{
    [super setUp];
    
    CCComponentSystemRegister(TEST_SYSTEM_ID, CCComponentSystemExecutionTypeManual, TestComponentSystemUpdateCallback, NULL, TestComponentSystemHandlesComponentCallback, NULL, NULL, NULL, NULL, NULL);
    CCComponentRegister(TEST_COMPONENT_ID, CC_STRING("Test"), CC_STD_ALLOCATOR, sizeof(TestComponentClass), TestComponentInitialize, NULL, NULL);
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
    CCEntity Entity = CCEntityCreate(0, CC_STD_ALLOCATOR);
    CCComponent Component = CCComponentCreate(TEST_COMPONENT_ID);
    
    CCEntityAttachComponent(Entity, Component);
    CCComponentSystemAddComponent(Component);
    
    CCEntityManagerAddEntity(Entity);
    CCEntityManagerUpdate();
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity), CC_STRING("0x0000000000000000")), @"Should have assigned a unique ID");
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCEntityGetComponents(Entity), &Enumerator);
    
    XCTAssertEqual(TestComponentGetValue(*(CCComponent*)CCCollectionEnumeratorGetCurrent(&Enumerator)), 1, @"Should update the entity's component");
    
    
    CCEntityManagerRemoveEntity(Entity);
    CCEntityManagerUpdate();
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
    
    
    CCEntityManagerUpdate();
    CCComponentSystemRun(CCComponentSystemExecutionTypeManual);
}

-(void) testAssigningEntityID
{
    CCEntity Entity[5] = {
        CCEntityCreate(CC_STRING("a"), CC_STD_ALLOCATOR),
        CCEntityCreate(CC_STRING("b"), CC_STD_ALLOCATOR),
        CCEntityCreate(CC_STRING("c"), CC_STD_ALLOCATOR),
        CCEntityCreate(CC_STRING("d"), CC_STD_ALLOCATOR),
        CCEntityCreate(CC_STRING("e"), CC_STD_ALLOCATOR)
    };
    
    CCString NextID = CCEntityManagerGetNextID();
    XCTAssertTrue(CCStringEqual(NextID, CC_STRING("0x0000000000000000")), @"Should have the correct next ID");
    CCStringDestroy(NextID);
    
    CCEntityManagerAddEntity(Entity[0]);
    CCEntityManagerAddEntity(Entity[1]);
    CCEntityManagerAddEntityWithID(Entity[2], CC_STRING("0x10"));
    CCEntityManagerAddEntity(Entity[3]);
    
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[0]), CC_STRING("0x0000000000000000")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[1]), CC_STRING("0x0000000000000001")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[2]), CC_STRING("0x10")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[3]), CC_STRING("0x0000000000000011")), @"Should have assigned a unique ID");
    XCTAssertEqual(CCEntityGetID(Entity[4]), 0, @"Should not have assigned a unique ID");
    
    NextID = CCEntityManagerGetNextID();
    XCTAssertTrue(CCStringEqual(NextID, CC_STRING("0x0000000000000012")), @"Should have the correct next ID");
    CCStringDestroy(NextID);
    
    CCEntityManagerLock();
    XCTAssertEqual(CCEntityManagerGetEntity(CC_STRING("0x0000000000000000")), NULL, @"Should not find an active entity with this ID");
    CCEntityManagerUpdate();
    XCTAssertNotEqual(CCEntityManagerGetEntity(CC_STRING("0x0000000000000000")), NULL, @"Should find an active entity with this ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetName(CCEntityManagerGetEntity(CC_STRING("0x0000000000000000"))), CC_STRING("a")), @"Should find an active entity with this ID");
    CCEntityManagerUnlock();
    
    CCEntityManagerRemoveEntity(Entity[0]);
    CCEntityManagerAddEntity(Entity[4]);
    
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[1]), CC_STRING("0x0000000000000001")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[2]), CC_STRING("0x10")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[3]), CC_STRING("0x0000000000000011")), @"Should have assigned a unique ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetID(Entity[4]), CC_STRING("0x0000000000000012")), @"Should have assigned a unique ID");
    
    CCEntityManagerLock();
    XCTAssertNotEqual(CCEntityManagerGetEntity(CC_STRING("0x0000000000000000")), NULL, @"Should find an active entity with this ID");
    CCEntityManagerUpdate();
    XCTAssertEqual(CCEntityManagerGetEntity(CC_STRING("0x0000000000000000")), NULL, @"Should not find an active entity with this ID");
    XCTAssertNotEqual(CCEntityManagerGetEntity(CC_STRING("0x0000000000000012")), NULL, @"Should find an active entity with this ID");
    XCTAssertTrue(CCStringEqual(CCEntityGetName(CCEntityManagerGetEntity(CC_STRING("0x0000000000000012"))), CC_STRING("e")), @"Should find an active entity with this ID");
    CCEntityManagerUnlock();
    
    NextID = CCEntityManagerGetNextID();
    XCTAssertTrue(CCStringEqual(NextID, CC_STRING("0x0000000000000013")), @"Should have the correct next ID");
    CCStringDestroy(NextID);
}

@end
