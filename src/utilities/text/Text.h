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

#ifndef CommonGameKit_Text_h
#define CommonGameKit_Text_h

#include <CommonGameKit/TextAttribute.h>

typedef enum {
    CCTextAlignmentLeft,
    CCTextAlignmentCenter,
    CCTextAlignmentRight
} CCTextAlignment;

typedef struct {
    GFXDraw drawer;
    size_t vertices;
} CCTextDrawable;

/*!
 * @brief The renderable text.
 * @description Allows @b CCRetain.
 */
typedef struct CCTextInfo *CCText;


/*!
 * @brief Create the renderable text.
 * @description A convenient object to format renderable text. Re-setting fields
 *              is allowed with minimal impact on performance if it does not need
 *              to change much state internally.
 *
 * @param Allocator The allocator to be used for allocations.
 * @return The text. Must be destroyed.
 */
CC_NEW CCText CCTextCreate(CCAllocatorType Allocator);

/*!
 * @brief Destroy the text.
 * @param Text The text.
 */
void CCTextDestroy(CCText CC_DESTROY(Text));

/*!
 * @brief Get the draw operations to render the text.
 * @param Text The text.
 * @return The list of draw operation. Submit them or make changes to them.
 *         Note: Must destroy the collection.
 */
CC_NEW CCOrderedCollection CCTextGetDrawables(CCText Text);

/*!
 * @brief Set the string to be rendered.
 * @param Text The text.
 * @param AttributedStrings The collection of @b CCTextAttribute
 */
void CCTextSetString(CCText Text, CCOrderedCollection CC_RETAIN(AttributedStrings));

/*!
 * @brief Set alignment of the text.
 * @param Text The text.
 * @param Alignment The alignment to be used.
 */
void CCTextSetAlignment(CCText Text, CCTextAlignment Alignment);

/*!
 * @brief Set the visibility control of the text.
 * @param Text The text.
 * @param Visibility The visibility control to be used.
 */
void CCTextSetVisibility(CCText Text, CCTextVisibility Visibility);

/*!
 * @brief Set frame containing the text.
 * @param Text The text.
 * @param Frame The frame containing the text.
 */
void CCTextSetFrame(CCText Text, CCRect Frame);

/*!
 * @brief Get the full length of the text.
 * @param Text The text.
 * @return The length.
 */
size_t CCTextGetLength(CCText Text);

/*!
 * @brief Get the visible length of the text.
 * @description The number of characters visible in the frame.
 * @param Text The text.
 * @return The visible length.
 */
size_t CCTextGetVisibleLength(CCText Text);

/*!
 * @brief Set the visible length of the text.
 * @description The number of characters that should be visible in the frame, or
 *              as many that can fit.
 *
 * @param Text The text.
 * @param Length The maximum visible length. To allow as many as possible set to
 *        SIZE_MAX.
 */
void CCTextSetVisibleLength(CCText Text, size_t Length);

/*!
 * @brief Get the current offset of the text.
 * @description The offset to begin rendering from.
 * @param Text The text.
 * @return The offset.
 */
size_t CCTextGetOffset(CCText Text);

/*!
 * @brief Set the current offset of the text.
 * @description The offset to begin rendering from. Note: if word visibility is
 *              used, and the offset is set inside a word, then it'll cause 
 *              partial rendering of that word.
 *
 * @param Text The text.
 * @param Offset The offset.
 */
void CCTextSetOffset(CCText Text, size_t Offset);

/*!
 * @brief Get the cursor position in the text.
 * @param Text The text.
 * @param Offset The character offset to get the cursor of. To get the cursor at
 *        the end of the text for simplicity can pass SIZE_MAX.
 *
 * @return The position of the cursor.
 */
CCVector2D CCTextGetCursorPosition(CCText Text, size_t Offset);

/*!
 * @brief Get the cursor offset in the text.
 * @param Text The text.
 * @param Position The position of the character to get the curser offset of.
 * @return The offset of the cursor or SIZE_MAX if no character is at that position.
 */
size_t CCTextGetCursorOffset(CCText Text, CCVector2D Position);

/*!
 * @brief Get the full string.
 * @param Text The text.
 * @return The full string in the text object.
 */
CCString CCTextGetFullString(CCText Text);

#endif
