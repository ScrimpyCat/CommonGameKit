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

#include "Text.h"
#include "AssetManager.h"

typedef struct CCTextInfo {
    CCAllocatorType allocator;
    CCOrderedCollection drawers;
    CCOrderedCollection strings;
    CCOrderedCollection selection;
    CCOrderedCollection lines;
    CCArray lineInfo;
    CCArray charInfo;
    CCTextAlignment alignment;
    CCRect frame;
    struct {
        struct {
            CCTextVisibility options;
            size_t offset;
            size_t length;
        } controls;
        size_t length; //cached visible length
    } visible;
    size_t length; //cached length
    enum {
        CCTextChangedString = (1 << 0),
        CCTextChangedAlignment = (1 << 1),
        CCTextChangedFrame = (1 << 2),
        CCTextChangedVisibility = (1 << 3),
        CCTextChangedVisibleOffset = (1 << 4),
        CCTextChangedVisibleLength = (1 << 5),
        
        CCTextChangedEverything = CCTextChangedString | CCTextChangedAlignment | CCTextChangedFrame | CCTextChangedVisibility | CCTextChangedVisibleOffset | CCTextChangedVisibleLength,
        CCTextChangedSelection = CCTextChangedString | CCTextChangedVisibleOffset | CCTextChangedVisibleLength,
        CCTextChangedLines = CCTextChangedVisibility | CCTextChangedFrame | CCTextChangedSelection
    } changed;
} CCTextInfo;

typedef struct {
    size_t length;
    float width;
    float height;
    float leading;
    float trailing;
} CCTextLineInfo;

typedef struct {
    CCVector2D cursor;
} CCTextCharInfo;

static void CCTextDestructor(CCText Text)
{
    if (Text->drawers) CCCollectionDestroy(Text->drawers);
    if (Text->strings) CCCollectionDestroy(Text->strings);
    if (Text->selection) CCCollectionDestroy(Text->selection);
    if (Text->lines) CCCollectionDestroy(Text->lines);
    if (Text->lineInfo) CCArrayDestroy(Text->lineInfo);
    if (Text->charInfo) CCArrayDestroy(Text->charInfo);
}

CCText CCTextCreate(CCAllocatorType Allocator)
{
    CCText Text = CCMalloc(Allocator, sizeof(CCTextInfo), NULL, CC_DEFAULT_ERROR_CALLBACK);
    if (Text)
    {
        *Text = (CCTextInfo){
            .allocator = Allocator,
            .drawers = NULL,
            .strings = NULL,
            .selection = NULL,
            .lines = NULL,
            .lineInfo = CCArrayCreate(Allocator, sizeof(CCTextLineInfo), 4),
            .charInfo = CCArrayCreate(Allocator, sizeof(CCTextCharInfo), 16),
            .alignment = 0,
            .frame = (CCRect){ CCVector2DFill(0.0f), CCVector2DFill(0.0f) },
            .visible = {
                .controls = {
                    .options = CCTextVisibilityMultiLine | CCTextVisibilityWord,
                    .offset = 0,
                    .length = SIZE_MAX
                },
                .length = 0
            },
            .length = 0,
            .changed = CCTextChangedEverything
        };
        
        CCMemorySetDestructor(Text, (CCMemoryDestructorCallback)CCTextDestructor);
    }
    
    else CC_LOG_ERROR("Failed to create text, due to allocation failure. Allocation size (%zu)", sizeof(CCTextInfo));
    
    return Text;
}

void CCTextDestroy(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    CC_SAFE_Free(Text);
}

typedef struct {
    CCVector2D position;
    CCColourRGBA colour;
    CCVector2D texCoord;
    //edge
    //width
} CCTextVertexData;

static void CCTextDrawableElementDestructor(CCCollection Collection, CCTextDrawable *Drawable)
{
    GFXDrawDestroy(Drawable->drawer);
}

CCOrderedCollection CCTextGetDrawables(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    if ((!Text->changed) && (Text->drawers)) return CCRetain(Text->drawers);
    /*
     TODO: Optimize!
     
     - The CCTextAttribute functions are not designed for optimal usage, later combine their operations.
     - The drawables should be ordered into groups that can be drawn together in a single call.
     */
    CCOrderedCollection Drawables = CCCollectionCreate(Text->allocator, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating, sizeof(CCTextDrawable), (CCCollectionElementDestructor)CCTextDrawableElementDestructor);
    
    Text->visible.length = 0;
    if (Text->strings)
    {
        CCOrderedCollection Selection = (Text->selection && !(Text->changed & CCTextChangedSelection) ? CCRetain(Text->selection) : CCTextAttributeGetSelection(Text->allocator, Text->strings, Text->visible.controls.offset, Text->visible.controls.length));
        CCOrderedCollection Lines = (Text->lines && !(Text->changed & CCTextChangedLines) ? CCRetain(Text->lines) : CCTextAttributeGetLines(Text->allocator, Selection, Text->visible.controls.options, Text->frame.size.x));
        
        if (Text->selection) CCCollectionDestroy(Text->selection);
        Text->selection = Selection;
        
        if (Text->changed & CCTextChangedLines)
        {
            CCArrayRemoveAllElements(Text->lineInfo);
            CCArrayRemoveAllElements(Text->charInfo);
        }
        
        size_t LineIndex = 0;
        float Height = 0.0f;
        CC_COLLECTION_FOREACH(CCOrderedCollection, Line, Lines)
        {
            if (LineIndex >= CCArrayGetCount(Text->lineInfo))
            {
                CCTextLineInfo Info = {
                    .height = CCTextAttributeGetLineHeight(Line, TRUE),
                    .length = CCTextAttributeGetLength(Line),
                    .width = CCTextAttributeGetLineWidth(Line, &Info.leading, &Info.trailing)
                };
                
                CCArrayAppendElement(Text->lineInfo, &Info);
            }
            
            const CCTextLineInfo *LineInfo = CCArrayGetElementAtIndex(Text->lineInfo, LineIndex++);
            
            Height += LineInfo->height;
            if (Height > Text->frame.size.y) break;
            
            Text->visible.length += LineInfo->length;
            
            CCVector2D Cursor = CCVector2Add(Text->frame.position, CCVector2DMake(0.0f, Text->frame.size.y - Height));
            if (Text->alignment != CCTextAlignmentLeft)
            {
                Cursor.x += (Text->frame.size.x - LineInfo->width) / (Text->alignment == CCTextAlignmentCenter ? 2.0f : 1.0f);
            }
            
            CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, Line)
            {
                const CCFontAttribute Options = CCTextAttributeGetFontAttribute(Attribute);
                
                CCVector2D CachedCursor = Cursor;
                size_t GlyphCount = 0;
                CC_STRING_FOREACH(Letter, Attribute->string)
                {
                    const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
                    if (Glyph)
                    {
                        GlyphCount++;
                        
                        CCArrayAppendElement(Text->charInfo, &(CCTextCharInfo){ .cursor = CachedCursor });
                        
                        CachedCursor = CCFontPositionGlyph(Attribute->font, Glyph, Options, CachedCursor, NULL, NULL);
                    }
                    
                    else
                    {
                        CCArrayAppendElement(Text->charInfo, &(CCTextCharInfo){ .cursor = CachedCursor });
                    }
                }
                
                if (GlyphCount)
                {
                    CCTextVertexData *Data = CCMalloc(Text->allocator, sizeof(CCTextVertexData) * GlyphCount * 4, NULL, CC_DEFAULT_ERROR_CALLBACK);
                    if (Data)
                    {
                        //TODO: This IBO should be an asset
#define IBO_SIZE 20002
#define QUAD_BATCH_SIZE ((DEMO_IBO_SIZE + 2) / 6)
#define DEGENERATE_STRIPS(quads) (((quads) - 1) * 2)
                        
                        static GFXBuffer IBO = NULL;
                        if (!IBO)
                        {
                            unsigned short *Indices;
                            CC_SAFE_Malloc(Indices, sizeof(unsigned short) * IBO_SIZE,
                                           CC_LOG_ERROR("Failed to allocate IBO");
                                           
                                           if (Text->lines) CCCollectionDestroy(Text->lines);
                                           Text->lines = Lines;
                                           
                                           return NULL;
                                           );
                            
                            
                            for (unsigned short Loop = 0, Loop2 = 0; Loop < IBO_SIZE; )
                            {
                                Indices[Loop++] = Loop2++;
                                Indices[Loop++] = Loop2++;
                                Indices[Loop++] = Loop2++;
                                Indices[Loop++] = Loop2;
                                
                                if (Loop < IBO_SIZE - 4)
                                {
                                    Indices[Loop++] = Loop2++;
                                    Indices[Loop++] = Loop2;
                                }
                            }
                            
                            IBO = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataIndex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(unsigned short) * IBO_SIZE, Indices);
                            CC_SAFE_Free(Indices);
                        }
                        
                        
                        size_t Index = 0;
                        CC_STRING_FOREACH(Letter, Attribute->string)
                        {
                            const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
                            if (Glyph)
                            {
                                CCRect Rect, TexCoord;
                                Cursor = CCFontPositionGlyph(Attribute->font, Glyph, Options, Cursor, &Rect, &TexCoord);
                                
                                Rect.position = CCVector2Add(Rect.position, Attribute->offset);
                                
                                const CCVector2D PosTilt = Attribute->tilt;
                                const CCVector2D NegTilt = CCVector2Neg(Attribute->tilt);
                                
                                Data[Index++] = (CCTextVertexData){
                                    .position = CCVector2Add(Rect.position, NegTilt),
                                    .colour = Attribute->colour,
                                    .texCoord = TexCoord.position
                                };
                                Data[Index++] = (CCTextVertexData){
                                    .position = CCVector2Add(CCVector2DMake(Rect.position.x + Rect.size.x, Rect.position.y), CCVector2DMake(NegTilt.x, PosTilt.y)),
                                    .colour = Attribute->colour,
                                    .texCoord = CCVector2DMake(TexCoord.position.x + TexCoord.size.x, TexCoord.position.y)
                                };
                                Data[Index++] = (CCTextVertexData){
                                    .position = CCVector2Add(CCVector2DMake(Rect.position.x, Rect.position.y + Rect.size.y), CCVector2DMake(PosTilt.x, NegTilt.y)),
                                    .colour = Attribute->colour,
                                    .texCoord = CCVector2DMake(TexCoord.position.x, TexCoord.position.y + TexCoord.size.y)
                                };
                                Data[Index++] = (CCTextVertexData){
                                    .position = CCVector2Add(CCVector2Add(Rect.position, Rect.size), PosTilt),
                                    .colour = Attribute->colour,
                                    .texCoord = CCVector2Add(TexCoord.position, TexCoord.size)
                                };
                            }
                        }
                        
                        
                        GFXBuffer VertBuffer = GFXBufferCreate(Text->allocator, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(CCTextVertexData) * GlyphCount * 4, Data);
                        CC_SAFE_Free(Data);
                        
                        GFXBuffer EdgeBuffer = GFXBufferCreate(Text->allocator, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(float), &(float){ Attribute->softness });
                        GFXBuffer WidthBuffer = GFXBufferCreate(Text->allocator, GFXBufferHintData | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(float), &(float){ Attribute->thickness });
                        
                        GFXShader Shader = CCAssetManagerCreateShader(CC_STRING("signed-distance-field"));
                        
                        GFXDraw Drawer = GFXDrawCreate(Text->allocator);
                        GFXDrawSetShader(Drawer, Shader);
                        GFXDrawSetTexture(Drawer, CC_STRING("tex"), CCFontGetTexture(Attribute->font));
                        GFXDrawSetVertexBuffer(Drawer, CC_STRING("vPosition"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCTextVertexData), offsetof(CCTextVertexData, position));
                        GFXDrawSetVertexBuffer(Drawer, CC_STRING("vColour"), VertBuffer, GFXBufferFormatFloat32x4, sizeof(CCTextVertexData), offsetof(CCTextVertexData, colour));
                        GFXDrawSetVertexBuffer(Drawer, CC_STRING("vTexCoord"), VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCTextVertexData), offsetof(CCTextVertexData, texCoord));
                        GFXDrawSetBuffer(Drawer, CC_STRING("edge"), EdgeBuffer);
                        GFXDrawSetBuffer(Drawer, CC_STRING("width"), WidthBuffer);
                        GFXDrawSetBlending(Drawer, GFXBlendTransparent);
                        GFXDrawSetIndexBuffer(Drawer, IBO, GFXBufferFormatUInt16);
                        
                        GFXBufferDestroy(EdgeBuffer);
                        GFXBufferDestroy(WidthBuffer);
                        GFXBufferDestroy(VertBuffer);
                        GFXShaderDestroy(Shader);
                        
                        CCOrderedCollectionAppendElement(Drawables, &(CCTextDrawable){
                            .drawer = Drawer,
                            .vertices = (GlyphCount * 4) + DEGENERATE_STRIPS(GlyphCount)
                        });
                    }
                }
            }
        }
        
        if (Text->lines) CCCollectionDestroy(Text->lines);
        Text->lines = Lines;
    }
    
    Text->changed = 0;
    
    if (Text->drawers) CCCollectionDestroy(Text->drawers);
    
    return CCRetain((Text->drawers = Drawables));
}

void CCTextSetString(CCText Text, CCOrderedCollection AttributedStrings)
{
    CCAssertLog(Text, "Text must not be null");
    
    size_t Length = 0;
    CCOrderedCollection Strings = NULL;
    if (AttributedStrings)
    {
        Strings = CCTextAttributeMerge(Text->allocator, AttributedStrings);
        Length = CCTextAttributeGetLength(AttributedStrings);
    }
    
    if (Text->strings)
    {
        if ((Text->length == Length) && (CCCollectionGetCount(Text->strings) == CCCollectionGetCount(Strings)))
        {
            CCEnumerator Enumerator1, Enumerator2;
            
            CCCollectionGetEnumerator(Text->strings, &Enumerator1);
            CCCollectionGetEnumerator(Strings, &Enumerator2);
            
            _Bool Match = TRUE;
            for (CCTextAttribute *Attribute1 = CCCollectionEnumeratorGetCurrent(&Enumerator1), *Attribute2 = CCCollectionEnumeratorGetCurrent(&Enumerator2); (Attribute1) && (Attribute2); Attribute1 = CCCollectionEnumeratorNext(&Enumerator1), Attribute2 = CCCollectionEnumeratorNext(&Enumerator2))
            {
                Match = CCStringEqual(Attribute1->string, Attribute2->string) &&
                        (Attribute1->font == Attribute2->font) && //TODO: if false check internal values?
                        ((Attribute1->colour.r == Attribute2->colour.r) && (Attribute1->colour.g == Attribute2->colour.g) && (Attribute1->colour.b == Attribute2->colour.b) && (Attribute1->colour.a == Attribute2->colour.a)) &&
                        CCVector2Equal(Attribute1->scale, Attribute2->scale) &&
                        CCVector2Equal(Attribute1->offset, Attribute2->offset) &&
                        CCVector2Equal(Attribute1->tilt, Attribute2->tilt) &&
                        (Attribute1->space == Attribute2->space) &&
                        (Attribute1->softness == Attribute2->softness) &&
                        (Attribute1->thickness == Attribute2->thickness);
                
                if (!Match) break;
            }
            
            if (Match)
            {
                CCCollectionDestroy(Strings);
                return;
            }
        }
        
        CCCollectionDestroy(Text->strings);
    }
    
    Text->strings = Strings;
    Text->length = Length;
    
    Text->changed |= CCTextChangedString;
}

void CCTextSetAlignment(CCText Text, CCTextAlignment Alignment)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->alignment != Alignment)
    {
        Text->alignment = Alignment;
        Text->changed |= CCTextChangedAlignment;
    }
}

void CCTextSetVisibility(CCText Text, CCTextVisibility Visibility)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.options != Visibility)
    {
        Text->visible.controls.options = Visibility;
        Text->changed |= CCTextChangedVisibility;
    }
}

void CCTextSetFrame(CCText Text, CCRect Frame)
{
    CCAssertLog(Text, "Text must not be null");
    
    if ((Text->frame.position.x != Frame.position.x) ||
        (Text->frame.position.y != Frame.position.y) ||
        (Text->frame.size.x != Frame.size.x) ||
        (Text->frame.size.y != Frame.size.y))
    {
        Text->frame = Frame;
        Text->changed |= CCTextChangedFrame;
    }
}

size_t CCTextGetLength(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    return Text->length;
}

size_t CCTextGetVisibleLength(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    //TODO: setup drawables and get visible length
    return Text->visible.length;
}

void CCTextSetVisibleLength(CCText Text, size_t Length)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.length != Length)
    {
        Text->visible.controls.length = Length;
        Text->changed |= CCTextChangedVisibleLength;
    }
}

size_t CCTextGetOffset(CCText Text)
{
    CCAssertLog(Text, "Text must not be null");
    
    return Text->visible.controls.offset;
}

void CCTextSetOffset(CCText Text, size_t Offset)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.offset != Offset)
    {
        Text->visible.controls.offset = Offset;
        Text->changed |= CCTextChangedVisibleOffset;
    }
}

CCVector2D CCTextGetCursorPosition(CCText Text, size_t Offset)
{
    if (Offset == SIZE_MAX) Offset = CCTextGetLength(Text);
    
    size_t Length = Text->visible.controls.offset;
    
    if ((Text->strings) && (Length <= Offset))
    {
        if (Text->changed & CCTextChangedLines)
        {
            CCArrayRemoveAllElements(Text->lineInfo);
            CCArrayRemoveAllElements(Text->charInfo);
        }
        
        else if (Offset < CCArrayGetCount(Text->charInfo)) return ((CCTextCharInfo*)CCArrayGetElementAtIndex(Text->charInfo, Offset))->cursor;
        
        CCOrderedCollection Selection = (Text->selection && !(Text->changed & CCTextChangedSelection) ? CCRetain(Text->selection) : CCTextAttributeGetSelection(Text->allocator, Text->strings, Text->visible.controls.offset, Text->visible.controls.length));
        CCOrderedCollection Lines = (Text->lines && !(Text->changed & CCTextChangedLines) ? CCRetain(Text->lines) : CCTextAttributeGetLines(Text->allocator, Selection, Text->visible.controls.options, Text->frame.size.x));
        
        if (Text->selection) CCCollectionDestroy(Text->selection);
        Text->selection = Selection;
        
        size_t LineIndex = 0;
        float Height = 0.0f;
        CC_COLLECTION_FOREACH(CCOrderedCollection, Line, Lines)
        {
            if (LineIndex >= CCArrayGetCount(Text->lineInfo))
            {
                CCTextLineInfo Info = {
                    .height = CCTextAttributeGetLineHeight(Line, TRUE),
                    .length = CCTextAttributeGetLength(Line),
                    .width = CCTextAttributeGetLineWidth(Line, &Info.leading, &Info.trailing)
                };
                
                CCArrayAppendElement(Text->lineInfo, &Info);
            }
            
            const CCTextLineInfo *LineInfo = CCArrayGetElementAtIndex(Text->lineInfo, LineIndex++);
            
            Height += LineInfo->height;
            if (Height > Text->frame.size.y) break;
            
            size_t PrevLength = Length;
            Length += LineInfo->length;
            
            CCVector2D Cursor = CCVector2Add(Text->frame.position, CCVector2DMake(0.0f, Text->frame.size.y - Height));
            if (Text->alignment != CCTextAlignmentLeft)
            {
                Cursor.x += (Text->frame.size.x - LineInfo->width) / (Text->alignment == CCTextAlignmentCenter ? 2.0f : 1.0f);
            }
            
            if (Length >= Offset)
            {
                CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, Line)
                {
                    const CCFontAttribute Options = CCTextAttributeGetFontAttribute(Attribute);
                    CC_STRING_FOREACH(Letter, Attribute->string)
                    {
                        if (PrevLength++ == Offset)
                        {
                            if (Text->lines) CCCollectionDestroy(Text->lines);
                            Text->lines = Lines;
                            
                            return Cursor;
                        }
                        
                        const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
                        if (Glyph)
                        {
                            Cursor = CCFontPositionGlyph(Attribute->font, Glyph, Options, Cursor, NULL, NULL);
                        }
                    }
                }
                
                if (Text->lines) CCCollectionDestroy(Text->lines);
                Text->lines = Lines;
                
                return Cursor;
            }
        }
        
        if (Text->lines) CCCollectionDestroy(Text->lines);
        Text->lines = Lines;
    }
    
    return CCVector2DMake(NAN, NAN);
}

size_t CCTextGetCursorOffset(CCText Text, CCVector2D Position)
{
    size_t Length = Text->visible.controls.offset;
    
    if ((Text->strings) && (CCVector2LessThanEqual(Text->frame.position, Position)) && (CCVector2LessThanEqual(Position, CCVector2Add(Text->frame.position, Text->frame.size))))
    {
        if (Text->changed & CCTextChangedLines)
        {
            CCArrayRemoveAllElements(Text->lineInfo);
            CCArrayRemoveAllElements(Text->charInfo);
        }
        
        CCOrderedCollection Selection = (Text->selection && !(Text->changed & CCTextChangedSelection) ? CCRetain(Text->selection) : CCTextAttributeGetSelection(Text->allocator, Text->strings, Text->visible.controls.offset, Text->visible.controls.length));
        CCOrderedCollection Lines = (Text->lines && !(Text->changed & CCTextChangedLines) ? CCRetain(Text->lines) : CCTextAttributeGetLines(Text->allocator, Selection, Text->visible.controls.options, Text->frame.size.x));
        
        if (Text->selection) CCCollectionDestroy(Text->selection);
        Text->selection = Selection;
        
        size_t LineIndex = 0;
        float Height = 0.0f;
        
        CC_COLLECTION_FOREACH(CCOrderedCollection, Line, Lines)
        {
            if (LineIndex >= CCArrayGetCount(Text->lineInfo))
            {
                CCTextLineInfo Info = {
                    .height = CCTextAttributeGetLineHeight(Line, TRUE),
                    .length = CCTextAttributeGetLength(Line),
                    .width = CCTextAttributeGetLineWidth(Line, &Info.leading, &Info.trailing)
                };
                
                CCArrayAppendElement(Text->lineInfo, &Info);
            }
            
            const CCTextLineInfo *LineInfo = CCArrayGetElementAtIndex(Text->lineInfo, LineIndex++);
            
            Height += LineInfo->height;
            if (Height > Text->frame.size.y) break;
            
            size_t Offset = Length;
            Length += LineInfo->length;
            
            CCVector2D Cursor = CCVector2Add(Text->frame.position, CCVector2DMake(0.0f, Text->frame.size.y - Height));
            if ((Cursor.y <= Position.y) && (Position.y <= (Cursor.y + Height)))
            {
                if (Text->alignment != CCTextAlignmentLeft)
                {
                    Cursor.x += (Text->frame.size.x - LineInfo->width) / (Text->alignment == CCTextAlignmentCenter ? 2.0f : 1.0f);
                }
                
                if ((Cursor.x <= Position.x) && (Position.x <= (Cursor.x + LineInfo->width)))
                {
                    CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, Line)
                    {
                        const CCFontAttribute Options = CCTextAttributeGetFontAttribute(Attribute);
                        CC_STRING_FOREACH(Letter, Attribute->string)
                        {
                            const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
                            if (Glyph)
                            {
                                CCRect Rect;
                                Cursor = CCFontPositionGlyph(Attribute->font, Glyph, Options, Cursor, &Rect, NULL);
                                
                                Rect.position = CCVector2Add(Rect.position, Attribute->offset);
                                
                                //TODO: Option to match tilts?
                                
                                if ((CCVector2LessThanEqual(Rect.position, Position)) && (CCVector2LessThanEqual(Position, CCVector2Add(Rect.position, Rect.size))))
                                {
                                    if (Text->lines) CCCollectionDestroy(Text->lines);
                                    Text->lines = Lines;
                                    
                                    return Offset;
                                }
                            }
                            
                            Offset++;
                        }
                    }
                }
                
                else break;
            }
        }
        
        if (Text->lines) CCCollectionDestroy(Text->lines);
        Text->lines = Lines;
    }
    
    return SIZE_MAX;
}
