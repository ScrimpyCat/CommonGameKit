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
