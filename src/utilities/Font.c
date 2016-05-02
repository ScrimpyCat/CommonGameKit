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

#include "Font.h"

typedef enum {
    CCFontCharsetTypeUnicode = (1 << 0),
    CCFontCharsetTypeOffsetMap = (1 << 1)
} CCFontCharsetType;

typedef struct {
    CCFontCharsetType type;
    CCRect padding;
    CCVector2D spacing;
    int32_t lineHeight;
    int32_t base;
    GFXTexture texture;
    CCFontCharMap map;
    CCArray glyphs;
} CCFontCharset;

typedef struct CCFontInfo {
    CCString name;
    CCFontStyle style;
    uint32_t size;
    CCFontCharset charset;
} CCFontInfo;

static void CCFontDestructor(CCFont Font)
{
    CCStringDestroy(Font->name);
    if (Font->charset.texture) GFXTextureDestroy(Font->charset.texture);
    if (Font->charset.glyphs) CCArrayDestroy(Font->charset.glyphs);
    
    if ((!(Font->charset.type & CCFontCharsetTypeOffsetMap)) && (Font->charset.map.letters)) CCArrayDestroy(Font->charset.map.letters);
}

CCFont CCFontCreate(CCString Name, CCFontStyle Style, uint32_t Size, CCRect Padding, CCVector2D Spacing, int32_t LineHeight, int32_t Base, _Bool IsUnicode, _Bool SequentialMap, CCFontCharMap Map, CCArray Glyphs, GFXTexture Texture)
{
    CCAssertLog(Name, "Must have a name");
    
    CCFont Font;
    CC_SAFE_Malloc(Font, sizeof(CCFontInfo),
                   CC_LOG_ERROR("Failed to create font, due to allocation failure. Allocation size (%zu)", sizeof(CCFontInfo));
                   return NULL;
                   );
    
    *Font = (CCFontInfo){
        .name = CCStringCopy(Name),
        .style = Style,
        .size = Size,
        .charset = {
            .type = (IsUnicode ? CCFontCharsetTypeUnicode : 0) | (SequentialMap ? CCFontCharsetTypeOffsetMap : 0),
            .padding = Padding,
            .spacing = Spacing,
            .lineHeight = LineHeight,
            .base = Base,
            .texture = CCRetain(Texture),
            .map = SequentialMap ? Map : (CCFontCharMap){ .letters = CCRetain(Map.letters) },
            .glyphs = CCRetain(Glyphs)
        }
    };
    
    CCMemorySetDestructor(Font, (CCMemoryDestructorCallback)CCFontDestructor);
    
    return Font;
}

void CCFontDestroy(CCFont Font)
{
    CCAssertLog(Font, "Font must not be null");
    CC_SAFE_Free(Font);
}

CCFontGlyph *CCFontGetGlyph(CCFont Font, CCChar Letter)
{
    CCAssertLog(Font, "Font must not be null");
    
    if (Font->charset.type & CCFontCharsetTypeOffsetMap)
    {
        const size_t Index = Letter - Font->charset.map.offset;
        if (Index < CCArrayGetCount(Font->charset.glyphs)) return CCArrayGetElementAtIndex(Font->charset.glyphs, Letter - Font->charset.map.offset);
    }
    
    else
    {
        for (size_t Loop = 0, Count = CCArrayGetCount(Font->charset.map.letters); Loop < Count; Loop++)
        {
            if (*(CCChar*)CCArrayGetElementAtIndex(Font->charset.map.letters, Loop) == Letter)
            {
                return CCArrayGetElementAtIndex(Font->charset.glyphs, Loop);
            }
        }
    }
    
    CC_LOG_WARNING_CUSTOM("Unable to find glyph for '%C' in font map: %S:%p", Letter, Font->name, Font);
    
    return NULL;
}

GFXTexture CCFontGetTexture(CCFont Font)
{
    return Font->charset.texture;
}

CCVector2D CCFontPositionGlyph(CCFont Font, const CCFontGlyph *Glyph, CCFontAttribute Attribute, CCVector2D Cursor, CCRect *Position, CCRect *TexCoord)
{
    CCAssertLog(Font, "Font must not be null");
    CCAssertLog(Glyph, "Glyph must not be null");
    
    if (Position)
    {
        *Position = (CCRect){
            .position = CCVector2Add(Cursor, CCVector2Mul(Attribute.scale, CCVector2DMake(Glyph->offset.x * Attribute.space, Font->charset.base - (Glyph->coord.size.y + Glyph->offset.y)))),
            .size = CCVector2Mul(Attribute.scale, Glyph->coord.size)
        };
    }
    
    if (TexCoord)
    {
        if (Font->charset.texture)
        {
            size_t Width, Height;
            GFXTextureGetSize(Font->charset.texture, &Width, &Height, NULL);
            
            *TexCoord = (CCRect){
                .position = {
                    Glyph->coord.position.x / Width,
                    Glyph->coord.position.y / Height
                },
                .size = {
                    Glyph->coord.size.x / Width,
                    Glyph->coord.size.y / Height
                }
            };
        }
        
        else
        {
            *TexCoord = (CCRect){ 0.0f, 0.0f, 0.0f, 0.0f };
        }
    }
    
    return CCVector2Add(Cursor, CCVector2Mul(Attribute.scale, CCVector2DMake(Glyph->advance * Attribute.space, 0.0f)));
}
