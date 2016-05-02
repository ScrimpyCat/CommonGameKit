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
#import "Font.h"

@interface Font : XCTestCase

@end

@implementation Font

-(void) testGettingGlyphs
{
    CCArray Glyphs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCFontGlyph), 3);
    
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-6.0f, 13.0f),
        .advance = 48.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(8.0f, 0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-4.0f, 3.0f),
        .advance = 48.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(16.0f, 0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-6.0f, 13.0f),
        .advance = 45.0f
    });
    
    CCFont Font = CCFontCreate(CC_STRING("test"), 0, 30, 82, 53, FALSE, TRUE, (CCFontCharMap){ .offset = 'a' }, Glyphs, NULL);
    CCArrayDestroy(Glyphs);
    
    
    CCFontGlyph *Glyph = CCFontGetGlyph(Font, 'a');
    XCTAssertEqual(Glyph->coord.position.x, 0.0f, @"Should get the correct glyph");
    
    Glyph = CCFontGetGlyph(Font, 'b');
    XCTAssertEqual(Glyph->coord.position.x, 8.0f, @"Should get the correct glyph");
    
    Glyph = CCFontGetGlyph(Font, 'c');
    XCTAssertEqual(Glyph->coord.position.x, 16.0f, @"Should get the correct glyph");
    
    Glyph = CCFontGetGlyph(Font, 'd');
    XCTAssertEqual(Glyph, NULL, @"Should not find the glyph");
    
    Glyph = CCFontGetGlyph(Font, 'A');
    XCTAssertEqual(Glyph, NULL, @"Should not find the glyph");
    
    
    CCFontDestroy(Font);
}

-(void) testPositioningGlyphs
{
    CCArray Glyphs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCFontGlyph), 3);
    
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-6.0f, 13.0f),
        .advance = 48.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(8.0f, 0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-4.0f, 3.0f),
        .advance = 48.0f
    });
    CCArrayAppendElement(Glyphs, &(CCFontGlyph){
        .coord = (CCRect){ CCVector2DMake(16.0f, 0.0f), CCVector2DFill(8.0f) },
        .offset = CCVector2DMake(-6.0f, 13.0f),
        .advance = 45.0f
    });
    
    int32_t Base = 50;
    CCFont Font = CCFontCreate(CC_STRING("test"), 0, 30, 82, Base, FALSE, TRUE, (CCFontCharMap){ .offset = 'a' }, Glyphs, NULL);
    CCArrayDestroy(Glyphs);
    
    
    CCRect Pos;
    CCVector2D Cursor = CCVector2DFill(0.0f);
    
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'a'), CCFontAttributeDefault(), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 48.0f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, -6.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, Base - (13.0f + 8.0f), @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 8.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 8.0f, @"Should correctly position glyph");
    
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'b'), CCFontAttributeDefault(), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 96.0f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, 44.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, Base - (3.0f + 8.0f), @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 8.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 8.0f, @"Should correctly position glyph");
    
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'c'), CCFontAttributeDefault(), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 141.0f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, 90.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, Base - (13.0f + 8.0f), @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 8.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 8.0f, @"Should correctly position glyph");
    
    Cursor = CCVector2DFill(0.0f);
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'a'), CCFontAttributeCreate(CCVector2DFill(0.5f), 1.0f), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 24.0f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, -3.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, (Base - (13.0f + 8.0f)) / 2.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 4.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 4.0f, @"Should correctly position glyph");
    
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'b'), CCFontAttributeCreate(CCVector2DFill(0.5f), 1.0f), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 48.0f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, 22.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, (Base - (3.0f + 8.0f)) / 2.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 4.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 4.0f, @"Should correctly position glyph");
    
    Cursor = CCFontPositionGlyph(Font, CCFontGetGlyph(Font, 'c'), CCFontAttributeCreate(CCVector2DFill(0.5f), 1.0f), Cursor, &Pos, NULL);
    XCTAssertEqual(Cursor.x, 70.5f, @"Should position cursor after glyph");
    XCTAssertEqual(Pos.position.x, 45.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.position.y, (Base - (13.0f + 8.0f)) / 2.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.x, 4.0f, @"Should correctly position glyph");
    XCTAssertEqual(Pos.size.y, 4.0f, @"Should correctly position glyph");
    
    
    CCFontDestroy(Font);
}

@end
