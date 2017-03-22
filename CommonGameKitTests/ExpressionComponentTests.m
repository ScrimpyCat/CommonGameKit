/*
 *  Copyright (c) 2016, Stefan Johnson
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
#import "Expression.h"
#import "ExpressionEvaluator.h"
#import "ComponentExpressions.h"
#import "InputMapGroupComponent.h"
#import "InputMapKeyboardComponent.h"

@interface ExpressionComponentTests : CCTestCase

@end

@implementation ExpressionComponentTests

static void KeyboardCallback()
{
}

-(void) setUp
{
    [super setUp];
    
    CCInputMapGroupComponentRegister();
    CCInputMapKeyboardComponentRegister();
    
    CCInputMapComponentRegisterCallback(CC_STRING(":test"), CCInputMapTypeKeyboard, KeyboardCallback);
}

-(void) testInputMapGroup
{
    CCExpression Expr = CCExpressionCreateFromSource("(input-group (action: \"test\") (input: (input-keyboard (keycode: :left)) (input-keyboard (keycode: :right))) (all-active: #t))");
    CCExpression Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    CCComponent Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_GROUP_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapGroupComponentGetWantsAllActive(Component), @"Should initialize correctly");
    XCTAssertTrue(CCStringEqual(CCInputMapComponentGetAction(Component), CC_STRING("test")), @"Should initialize correctly");
    XCTAssertEqual(CCCollectionGetCount(CCInputMapGroupComponentGetInputMaps(Component)), 2, @"Should be correct component");
    
    CCKeyboardKeycode Keycode[2] = { CCKeyboardKeycodeLeft, CCKeyboardKeycodeRight };
    CC_COLLECTION_FOREACH(CCComponent, Key, CCInputMapGroupComponentGetInputMaps(Component))
    {
        XCTAssertEqual(CCComponentGetID(Key), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
        
        XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Key), @"Should initialize correctly");
        XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Key), Keycode[CCOrderedCollectionGetIndex(CCInputMapGroupComponentGetInputMaps(Component), CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR))], @"Should initialize correctly");
        XCTAssertTrue(CCInputMapKeyboardComponentGetIgnoreModifier(Key), @"Should initialize correctly");
        XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Key), @"Should initialize correctly");
        XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Key), 0.0f, @"Should initialize correctly");
    }
    
    CCExpressionDestroy(Expr);
}

-(void) testInputMapKeyboard
{
    CCExpression Expr = CCExpressionCreateFromSource("(input-keyboard (keycode: :any) (repeat: #t) (action: \"test\") (ramp: 0.5))");
    CCExpression Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    CCComponent Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Component), CCInputMapKeyboardComponentKeycodeAny, @"Should initialize correctly");
    XCTAssertTrue(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertTrue(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertTrue(CCStringEqual(CCInputMapComponentGetAction(Component), CC_STRING("test")), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.5f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
    
    
    Expr = CCExpressionCreateFromSource("(input-keyboard (char: \"C\") (callback: :test) (flags: :shift :alt))");
    Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertFalse(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetCharacter(Component), 'C', @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetFlags(Component), CCKeyboardModifierShift | CCKeyboardModifierAlt, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetCallback(Component), &KeyboardCallback, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.0f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
    
    
    Expr = CCExpressionCreateFromSource("(input-keyboard (keycode: :a) (flags: :none))");
    Result = CCExpressionEvaluate(Expr);
    
    XCTAssertEqual(CCExpressionGetType(Result), CCComponentExpressionValueTypeComponent, @"Should create the component");
    
    Component = CCExpressionGetData(Result);
    XCTAssertEqual(CCComponentGetID(Component), CC_INPUT_MAP_KEYBOARD_COMPONENT_ID, @"Should be correct component");
    
    XCTAssertTrue(CCInputMapKeyboardComponentGetIsKeycode(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetKeycode(Component), CCKeyboardKeycodeA, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetIgnoreModifier(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetFlags(Component), 0, @"Should initialize correctly");
    XCTAssertFalse(CCInputMapKeyboardComponentGetRepeats(Component), @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetAction(Component), 0, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapComponentGetCallback(Component), NULL, @"Should initialize correctly");
    XCTAssertEqual(CCInputMapKeyboardComponentGetRamp(Component), 0.0f, @"Should initialize correctly");
    
    CCExpressionDestroy(Expr);
}

@end
