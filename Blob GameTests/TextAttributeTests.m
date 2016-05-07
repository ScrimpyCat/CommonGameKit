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

#import <XCTest/XCTest.h>
#import "TextAttribute.h"

@interface TextAttributeTests : XCTestCase

@end

@implementation TextAttributeTests

-(void) testLength
{
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aa bb cc"),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" abc "),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cba"),
        .font = NULL
    });
    
    
    XCTAssertEqual(CCTextAttributeGetLength(Strings), 16, @"Should have 16 characters");
    
    
    CCCollectionDestroy(Strings);
}

-(void) testLineHeight
{
    CCArray Glyphs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCFontGlyph), 3);
    
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(100.0f) },
        .offset = CCVector2DFill(0.0f),
        .advance = 5.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(5.0f) },
        .offset = CCVector2DMake(-1.0f, 10.0f),
        .advance = 6.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(8.0f, 0.0f), CCVector2DFill(5.0f) },
        .offset = CCVector2DMake(0.0f, -55.0f),
        .advance = 6.0f
    });
    
    CCArray Letters = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCChar), 4);
    
    CCArrayAppendElement(Letters, &(CCChar){ ' ' });
    CCArrayAppendElement(Letters, &(CCChar){ 'a' });
    CCArrayAppendElement(Letters, &(CCChar){ 'b' });
    
    CCFont Font = CCFontCreate(CC_STD_ALLOCATOR, CC_STRING("test"), 0, 30, 50, 0, FALSE, FALSE, (CCFontCharMap){ .letters = Letters }, Glyphs, NULL);
    CCArrayDestroy(Glyphs);
    CCArrayDestroy(Letters);
    
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a a"),
        .font = Font
    });
    
    
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, FALSE), 200.0f, @"Should have the correct line height");
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, TRUE), 50.0f, @"Should have the correct line height");
    
    
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b"),
        .font = Font
    });
    
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, FALSE), 200.0f, @"Should have the correct line height");
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, TRUE), 55.0f, @"Should have the correct line height");
    
    
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a a"),
        .font = Font,
        .scale = CCVector2DFill(1.0f)
    });
    
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, FALSE), 400.0f, @"Should have the correct line height");
    XCTAssertEqual(CCTextAttributeGetLineHeight(Strings, TRUE), 100.0f, @"Should have the correct line height");
    
    
    CCCollectionDestroy(Strings);
    CCFontDestroy(Font);
}

-(void) testMerging
{
    CCFont Font = CCFontCreate(CC_STD_ALLOCATOR, CC_STRING("test"), 0, 30, 82, 53, FALSE, FALSE, (CCFontCharMap){ .offset = 0 }, NULL, NULL);
    
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aa bb cc"),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" abc "),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cba"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("bac"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aaa"),
        .font = NULL,
        .space = 0.5f
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("bbb"),
        .font = NULL
    });
    
    
    CCOrderedCollection Merged = CCTextAttributeMerge(CC_STD_ALLOCATOR, Strings);
    XCTAssertEqual(CCCollectionGetCount(Merged), 4, @"Should select 4 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Merged, 0))->string, CC_STRING("aa bb cc abc ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Merged, 1))->string, CC_STRING("cbabac")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Merged, 2))->string, CC_STRING("aaa")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Merged, 3))->string, CC_STRING("bbb")), @"Should be the correct string");
    CCCollectionDestroy(Merged);
    
    
    CCCollectionDestroy(Strings);
    CCFontDestroy(Font);
}

-(void) testSelection
{
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aa bb cc"),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" abc "),
        .font = NULL
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cba"),
        .font = NULL
    });
    
    
    CCOrderedCollection Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 6, 8);
    XCTAssertEqual(CCCollectionGetCount(Selection), 3, @"Should select 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 0))->string, CC_STRING("cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 1))->string, CC_STRING(" abc ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 2))->string, CC_STRING("c")), @"Should be the correct string");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 6, 1);
    XCTAssertEqual(CCCollectionGetCount(Selection), 1, @"Should select 1 attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 0))->string, CC_STRING("c")), @"Should be the correct string");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 6, SIZE_MAX);
    XCTAssertEqual(CCCollectionGetCount(Selection), 3, @"Should select 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 0))->string, CC_STRING("cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 1))->string, CC_STRING(" abc ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 2))->string, CC_STRING("cba")), @"Should be the correct string");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 0, 16);
    XCTAssertEqual(CCCollectionGetCount(Selection), 3, @"Should select 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 0))->string, CC_STRING("aa bb cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 1))->string, CC_STRING(" abc ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 2))->string, CC_STRING("cba")), @"Should be the correct string");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 6, 0);
    XCTAssertEqual(CCCollectionGetCount(Selection), 0, @"Should select 0 attributes");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 60, SIZE_MAX);
    XCTAssertEqual(CCCollectionGetCount(Selection), 0, @"Should select 0 attributes");
    CCCollectionDestroy(Selection);
    
    
    Selection = CCTextAttributeGetSelection(CC_STD_ALLOCATOR, Strings, 0, SIZE_MAX);
    XCTAssertEqual(CCCollectionGetCount(Selection), 3, @"Should select 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 0))->string, CC_STRING("aa bb cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 1))->string, CC_STRING(" abc ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(Selection, 2))->string, CC_STRING("cba")), @"Should be the correct string");
    CCCollectionDestroy(Selection);
    
    
    CCCollectionDestroy(Strings);
}

-(void) testLines
{
    CCArray Glyphs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCFontGlyph), 3);
    
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(0.0f) },
        .offset = CCVector2DFill(0.0f),
        .advance = 5.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(5.0f) },
        .offset = CCVector2DMake(-1.0f, 10.0f),
        .advance = 6.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(8.0f, 0.0f), CCVector2DFill(5.0f) },
        .offset = CCVector2DMake(2.0f, 10.0f),
        .advance = 6.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(16.0f, 0.0f), CCVector2DFill(10.0f) },
        .offset = CCVector2DMake(0.0f, 10.0f),
        .advance = 11.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(16.0f, 0.0f), CCVector2DFill(100.0f) },
        .offset = CCVector2DMake(0.0f, 10.0f),
        .advance = 100.0f
    });
    
    CCArray Letters = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCChar), 4);
    
    CCArrayAppendElement(Letters, &(CCChar){ ' ' });
    CCArrayAppendElement(Letters, &(CCChar){ 'a' });
    CCArrayAppendElement(Letters, &(CCChar){ 'b' });
    CCArrayAppendElement(Letters, &(CCChar){ 'c' });
    CCArrayAppendElement(Letters, &(CCChar){ 'z' });
    
    CCFont Font = CCFontCreate(CC_STD_ALLOCATOR, CC_STRING("test"), 0, 30, 82, 53, FALSE, FALSE, (CCFontCharMap){ .letters = Letters }, Glyphs, NULL);
    CCArrayDestroy(Glyphs);
    CCArrayDestroy(Letters);
    
    CCOrderedCollection Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aa bb cc"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" abc "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cbaz"),
        .font = Font
    });
    
    
    CCOrderedCollection Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 0.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 0, @"Should produce 0 lines");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 10.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 20.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa ")), @"Should be the correct string"); //cursor positons: 6 12 17
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 30.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa bb ")), @"Should be the correct string"); //cursor positons: 0-1 6-1 12 17+2 23+2 29
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 29.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa b")), @"Should be the correct string"); //cursor positons: 0-1 6-1 12 17+2 23+2+5 29
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityCharacter, 100.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 3, @"Should contain 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa bb cc")), @"Should be the correct string"); //6+6+5+6+6+5+11+11 = 56
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING(" abc ")), @"Should be the correct string"); //5+6+6+11 = 28
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 2))->string, CC_STRING("c")), @"Should be the correct string"); //84 + 11 = 95 + 2+5 > 100
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityCharacter, 10.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 12, @"Should produce 12 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING(" ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("b")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 3)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 3), 0))->string, CC_STRING("b ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 4)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 4), 0))->string, CC_STRING("c")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 5)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 5), 0))->string, CC_STRING("c")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 6)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 6), 0))->string, CC_STRING(" a")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 7)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 7), 0))->string, CC_STRING("b")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 8)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 8), 0))->string, CC_STRING("c")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 9)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 9), 0))->string, CC_STRING(" ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 10)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 10), 0))->string, CC_STRING("c")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 11)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 11), 0))->string, CC_STRING("ba")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityCharacter, 100.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 3, @"Should produce 3 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 3, @"Should contain 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa bb cc")), @"Should be the correct string"); //6+6+5+6+6+5+11+11 = 56
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING(" abc ")), @"Should be the correct string"); //5+6+6+11 = 28
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 2))->string, CC_STRING("c")), @"Should be the correct string"); //84 + 11 = 95 + 2+5 > 100
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("ba")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("z")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 20.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 2, @"Should produce 2 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("bb ")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 25.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 4, @"Should produce 4 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("bb ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 1))->string, CC_STRING(" ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 3)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 3), 0))->string, CC_STRING("abc ")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilitySingleLine | CCTextVisibilityWord, 100.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 1, @"Should produce 1 line");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aa bb cc")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING(" abc ")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aaaa"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("bb cb"),
        .font = Font
    });
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 40.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 2, @"Should produce 2 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("aaaa")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING("bb ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("cb")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 20.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 0, @"Should produce 0 lines");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a aaa"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("bb cb"),
        .font = Font
    });
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 40.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 3, @"Should produce 2 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("a ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("aaa")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 1))->string, CC_STRING("bb ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("cb")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING(" "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("c"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b"),
        .font = Font
    });
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 40.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 3, @"Should produce 3 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("a")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING(" ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 6, @"Should contain 6 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("a")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 1))->string, CC_STRING("a")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 2))->string, CC_STRING("a")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 3))->string, CC_STRING("b")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 4))->string, CC_STRING("b")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 5))->string, CC_STRING(" ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("c")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 1))->string, CC_STRING("b")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("aa"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("ab"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cb"),
        .font = Font
    });
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 40.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 3, @"Should produce 3 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("a ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 3, @"Should contain 3 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("aa")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 1))->string, CC_STRING("ab")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 2))->string, CC_STRING("b ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("cb")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    Strings = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintOrdered, sizeof(CCTextAttribute), NULL);
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("a\n"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("ab"),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("b "),
        .font = Font
    });
    CCOrderedCollectionAppendElement(Strings, &(CCTextAttribute){
        .string = CC_STRING("cb"),
        .font = Font
    });
    
    Lines = CCTextAttributeGetLines(CC_STD_ALLOCATOR, Strings, CCTextVisibilityMultiLine | CCTextVisibilityWord, 40.0f);
    XCTAssertEqual(CCCollectionGetCount(Lines), 3, @"Should produce 3 lines");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 0))->string, CC_STRING("a ")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 0), 1))->string, CC_STRING("a\n")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1)), 2, @"Should contain 2 attributes");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 0))->string, CC_STRING("ab")), @"Should be the correct string");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 1), 1))->string, CC_STRING("b ")), @"Should be the correct string");
    XCTAssertEqual(CCCollectionGetCount(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2)), 1, @"Should contain one attribute");
    XCTAssertTrue(CCStringEqual(((CCTextAttribute*)CCOrderedCollectionGetElementAtIndex(*(CCOrderedCollection*)CCOrderedCollectionGetElementAtIndex(Lines, 2), 0))->string, CC_STRING("cb")), @"Should be the correct string");
    CCCollectionDestroy(Lines);
    
    
    CCCollectionDestroy(Strings);
    CCFontDestroy(Font);
}

@end
