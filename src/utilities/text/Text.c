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
} CCTextInfo;

static void CCTextDestructor(CCText Text)
{
    if (Text->drawers) CCCollectionDestroy(Text->drawers);
    if (Text->strings) CCCollectionDestroy(Text->strings);
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
            .length = 0
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
    
    /*
     TODO: Optimize!
     
     - The CCTextAttribute functions are not designed for optimal usage, later combine their operations.
     - The drawables should be ordered into groups that can be drawn together in a single call.
     - Drawables should be cached and only recreated when necessary.
     */
    CCOrderedCollection Drawables = CCCollectionCreate(Text->allocator, CCCollectionHintOrdered | CCCollectionHintHeavyEnumerating, sizeof(CCTextDrawable), (CCCollectionElementDestructor)CCTextDrawableElementDestructor);
    
    Text->visible.length = 0;
    if (Text->strings)
    {
        CCOrderedCollection Selection = CCTextAttributeGetSelection(Text->allocator, Text->strings, Text->visible.controls.offset, Text->visible.controls.length);
        CCOrderedCollection Lines = CCTextAttributeGetLines(Text->allocator, Selection, Text->visible.controls.options, Text->frame.size.x);
        
        float Height = 0.0f;
        CC_COLLECTION_FOREACH(CCOrderedCollection, Line, Lines)
        {
            Height += CCTextAttributeGetLineHeight(Line, TRUE);
            if (Height > Text->frame.size.y) break;
            
            Text->visible.length += CCTextAttributeGetLength(Line);
            
            CCVector2D Cursor = CCVector2Add(Text->frame.position, CCVector2DMake(0.0f, Text->frame.size.y - Height));
            if (Text->alignment != CCTextAlignmentLeft)
            {
                float LeadingWhitespace, TrailingWhitespace;
                const float Width = CCTextAttributeGetLineWidth(Line, &LeadingWhitespace, &TrailingWhitespace);
                Cursor.x += (Text->frame.size.x - Width) / (Text->alignment == CCTextAlignmentCenter ? 2.0f : 1.0f);
            }
            
            CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, Line)
            {
                size_t GlyphCount = 0;
                CC_STRING_FOREACH(Letter, Attribute->string)
                {
                    const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
                    if (Glyph) GlyphCount++;
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
                        
                        
                        const CCFontAttribute Options = CCTextAttributeGetFontAttribute(Attribute);
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
                        GFXDrawSetTexture(Drawer, "tex", CCFontGetTexture(Attribute->font));
                        GFXDrawSetVertexBuffer(Drawer, "vPosition", VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCTextVertexData), offsetof(CCTextVertexData, position));
                        GFXDrawSetVertexBuffer(Drawer, "vColour", VertBuffer, GFXBufferFormatFloat32x4, sizeof(CCTextVertexData), offsetof(CCTextVertexData, colour));
                        GFXDrawSetVertexBuffer(Drawer, "vTexCoord", VertBuffer, GFXBufferFormatFloat32x2, sizeof(CCTextVertexData), offsetof(CCTextVertexData, texCoord));
                        GFXDrawSetBuffer(Drawer, "edge", EdgeBuffer);
                        GFXDrawSetBuffer(Drawer, "width", WidthBuffer);
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
    }
    
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
    
    //TODO: compare difference between new Strings and old Text->strings
    if (Text->strings) CCCollectionDestroy(Text->strings);
    Text->strings = Strings;
    Text->length = Length;
}

void CCTextSetAlignment(CCText Text, CCTextAlignment Alignment)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->alignment != Alignment)
    {
        //TODO: set change in alignment
        Text->alignment = Alignment;
    }
}

void CCTextSetVisibility(CCText Text, CCTextVisibility Visibility)
{
    CCAssertLog(Text, "Text must not be null");
    
    if (Text->visible.controls.options != Visibility)
    {
        //TODO: set change visibility
        Text->visible.controls.options = Visibility;
    }
}

void CCTextSetFrame(CCText Text, CCRect Frame)
{
    CCAssertLog(Text, "Text must not be null");
    
    if ((Text->frame.position.x != Frame.position.x) &&
        (Text->frame.position.y != Frame.position.y) &&
        (Text->frame.size.x != Frame.size.x) &&
        (Text->frame.size.y != Frame.size.y))
    {
        //TODO: set change frame
        Text->frame = Frame;
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
        //TODO: set change visible length
        Text->visible.controls.length = Length;
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
        //TODO: set change visible length
        Text->visible.controls.offset = Offset;
    }
}
