//
//  EntityTests.m
//  Blob Game
//
//  Created by Stefan Johnson on 25/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "Entity.h"
#import "ComponentBase.h"

@interface EntityTests : XCTestCase

@end

@implementation EntityTests

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

-(void) testAttachingComponents
{
    CCEntity Entity = CCEntityCreate(1, CC_STD_ALLOCATOR);
    CCComponent Component = CCComponentCreate(CC_COMPONENT_ID);
    
    XCTAssertEqual(CCCollectionGetCount(CCEntityGetComponents(Entity)), 0, "Entity should have the correct number of components attached to it");
    
    CCEntityAttachComponent(Entity, Component);
    XCTAssertEqual(CCCollectionGetCount(CCEntityGetComponents(Entity)), 1, "Entity should have the correct number of components attached to it");
    
    CCEntityDetachComponent(Entity, Component);
    XCTAssertEqual(CCCollectionGetCount(CCEntityGetComponents(Entity)), 0, "Entity should have the correct number of components attached to it");
    
    CCComponentDestroy(Component);
    CCEntityDestroy(Entity);
}

@end
