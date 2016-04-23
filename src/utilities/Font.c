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
    GFXTextureDestroy(Font->charset.texture);
    CCArrayDestroy(Font->charset.glyphs);
    
    if (Font->charset.type & CCFontCharsetTypeOffsetMap) CCArrayDestroy(Font->charset.map.letters);
}

CCFont CCFontCreate(CCString Name, CCFontStyle Style, uint32_t Size, CCRect Padding, CCVector2D Spacing, int32_t LineHeight, int32_t Base, _Bool IsUnicode, _Bool SequentialMap, CCFontCharMap Map, CCArray Glyphs, GFXTexture Texture)
{
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
