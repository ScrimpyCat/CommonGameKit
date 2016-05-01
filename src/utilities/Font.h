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

#ifndef Blob_Game_Font_h
#define Blob_Game_Font_h

#include <CommonC/Common.h>
#include "GFX.h"

typedef enum {
    CCFontStyleBold = (1 << 0),
    CCFontStyleItalic = (1 << 1)
} CCFontStyle;

typedef union {
    CCArray letters;
    size_t offset;
} CCFontCharMap;

typedef struct {
    CCRect coord;
    CCVector2D offset;
    float advance;
} CCFontGlyph;

/*!
 * @brief The font.
 * @description Allows @b CCRetain.
 */
typedef struct CCFontInfo *CCFont;


/*!
 * @brief Create the font.
 * @param Name The name of the font.
 * @param Style The style of the font.
 * @param Size The size of the font.
 * @param Padding The padding of the font.
 * @param Spacing The spacing of the font.
 * @param LineHeight The line height of the font.
 * @param Base The base of the font.
 * @param IsUnicode Whether or not the charset includes unicode characters.
 * @param SequentialMap Whether or not the charset map is sequential (offset based) or is an array of letters.
 * @param Glyphs The array of glyphs.
 * @param Texture The texture.
 * @return The font. Must be destroyed.
 */
CC_NEW CCFont CCFontCreate(CCString CC_COPY(Name), CCFontStyle Style, uint32_t Size, CCRect Padding, CCVector2D Spacing, int32_t LineHeight, int32_t Base, _Bool IsUnicode, _Bool SequentialMap, CCFontCharMap CC_RETAIN(Map), CCArray CC_RETAIN(Glyphs), GFXTexture CC_RETAIN(Texture));

/*!
 * @brief Destroy the font.
 * @param Font The font.
 */
void CCFontDestroy(CCFont CC_DESTROY(Font));

#endif