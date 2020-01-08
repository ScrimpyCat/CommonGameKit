/*
 *  Copyright (c) 2020, Stefan Johnson
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
#import "RelationSystem.h"
#import "RelationParentComponent.h"
#import "Callbacks.h"

@interface RelationSystemTests : CCTestCase

@end

@implementation RelationSystemTests

static double Timestamp(void)
{
    return 0.0;
}

-(void) setUp
{
    [super setUp];
    
    CCTimestamp = Timestamp;
    
    CCRelationSystemRegister();
    CCRelationParentComponentRegister();
}

-(void) tearDown
{
    CCRelationParentComponentDeregister();
    CCRelationSystemDeregister();
    
    CCTimestamp = NULL;
    
    [super tearDown];
}

-(void) testChildren
{
    CCEntity EntityA = CCEntityCreate(CC_STRING("a"), CC_STD_ALLOCATOR);
    CCEntity EntityB = CCEntityCreate(CC_STRING("b"), CC_STD_ALLOCATOR);
    CCEntity EntityC = CCEntityCreate(CC_STRING("c"), CC_STD_ALLOCATOR);
    CCEntity EntityD = CCEntityCreate(CC_STRING("d"), CC_STD_ALLOCATOR);
    
    CCComponent ParentComponent = CCComponentCreate(CC_RELATION_PARENT_COMPONENT_ID);
    CCRelationParentComponentSetParent(ParentComponent, EntityA);
    CCEntityAttachComponent(EntityB, ParentComponent);
    CCComponentSystemAddComponent(ParentComponent);
    
    ParentComponent = CCComponentCreate(CC_RELATION_PARENT_COMPONENT_ID);
    CCRelationParentComponentSetParent(ParentComponent, EntityC);
    CCEntityAttachComponent(EntityD, ParentComponent);
    CCComponentSystemAddComponent(ParentComponent);
    
    ParentComponent = CCComponentCreate(CC_RELATION_PARENT_COMPONENT_ID);
    CCRelationParentComponentSetParent(ParentComponent, EntityA);
    CCEntityAttachComponent(EntityC, ParentComponent);
    CCComponentSystemAddComponent(ParentComponent);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeUpdate);
    
    CCCollection(Entity) Children = CCRelationSystemGetChildren(EntityA);
    XCTAssertEqual(2, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityB);
    XCTAssertEqual(0, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityC);
    XCTAssertEqual(1, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityD);
    XCTAssertEqual(0, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    CCEntityDetachComponent(EntityC, ParentComponent);
    CCComponentSystemRemoveComponent(ParentComponent);
    
    CCComponentSystemRun(CCComponentSystemExecutionTypeUpdate);
    
    Children = CCRelationSystemGetChildren(EntityA);
    XCTAssertEqual(1, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityB);
    XCTAssertEqual(0, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityC);
    XCTAssertEqual(1, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    Children = CCRelationSystemGetChildren(EntityD);
    XCTAssertEqual(0, CCCollectionGetCount(Children), @"should have the correct number of children");
    CCCollectionDestroy(Children);
    
    CCEntityDestroy(EntityA);
    CCEntityDestroy(EntityB);
    CCEntityDestroy(EntityC);
    CCEntityDestroy(EntityD);
}

@end
