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

@interface EntityManagerTests : XCTestCase

@end

@implementation EntityManagerTests

-(void) setUp
{
    [super setUp];
    
    CCComponentRegister(CC_COMPONENT_ID, "Base", CC_STD_ALLOCATOR, sizeof(CCComponentClass), CCComponentInitialize);
    CCEntityManagerCreate();
}

-(void) tearDown
{
    CCEntityManagerDestroy();
    CCComponentDeregister(CC_COMPONENT_ID);
    
    [super tearDown];
}

-(void) testAddingEntities
{
    
}

@end
