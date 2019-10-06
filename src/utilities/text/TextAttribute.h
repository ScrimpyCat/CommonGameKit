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

#ifndef CommonGameKit_TextAttribute_h
#define CommonGameKit_TextAttribute_h

#include <CommonGameKit/Font.h>

typedef enum {
    CCTextVisibilityMultiLine = (0 << 0),
    CCTextVisibilitySingleLine = (1 << 0),
    
    CCTextVisibilityWord = (0 << 1),
    CCTextVisibilityCharacter = (1 << 1)
} CCTextVisibility;

typedef struct {
    /// The string the attribute applies to
    CCString string;
    /// The font to be used for the attribute
    CCFont font;
    /// The colour of the text
    CCColourRGBA colour;
    /// The size of the text
    CCVector2D scale;
    /// The positioning of the text relative to its current position
    CCVector2D offset;
    /// The tilt applied to the glyph. Positive values shift the top/right, negative values shit the bottom/left.
    CCVector2D tilt;
    /// The character distancing in the text
    float space;
    /// The character softness of the text (0.0 = hard, 1.0 = very soft)
    float softness;
    /// The thickness of the text (0.0 = very thin, 1.0 = very thick)
    float thickness;
} CCTextAttribute;

///Convenient collection element destructor for @b CCTextAttribute's
extern const CCCollectionElementDestructor CCTextAttributeDestructorForCollection;

/*!
 * @brief Get font attribute settings.
 * @param Attribute The text attribute to get the font attribute settings of.
 * @return The font attribute.
 */
CCFontAttribute CCTextAttributeGetFontAttribute(CCTextAttribute *Attribute);

/*!
 * @brief Get the length of the attributed strings.
 * @param Allocator The allocator to be used.
 * @param AttributedStrings The attributed strings to get the length of.
 * @return The length of the attributed strings.
 */
size_t CCTextAttributeGetLength(CCOrderedCollection(CCTextAttribute) AttributedStrings);

/*!
 * @brief Get the width of the line of attributed strings.
 * @param AttributedStrings The attributed strings to get the length of.
 * @param LeadingWidth Store the width of leading whitespace.
 * @param TrailingWidth Store the width of trailing whitespace.
 * @return The line width of the attributed strings.
 */
float CCTextAttributeGetLineWidth(CCOrderedCollection(CCTextAttribute) AttributedStrings, float *LeadingWidth, float *TrailingWidth);

/*!
 * @brief Get the height of the line of attributed strings.
 * @param AttributedStrings The attributed strings to get the length of.
 * @param IgnoreWhitespace Whether whitespace should be ignored or included in
 *        calculations.
 *
 * @return The line height of the attributed strings.
 */
float CCTextAttributeGetLineHeight(CCOrderedCollection(CCTextAttribute) AttributedStrings, _Bool IgnoreWhitespace);

/*!
 * @brief Merge any attributed strings that are mergeable.
 * @param Allocator The allocator to be used.
 * @param AttributedStrings The attributed strings to merge.
 * @return The merged attributed strings. Note: Must destroy the list.
 */
CC_NEW CCOrderedCollection(CCTextAttribute) CCTextAttributeMerge(CCAllocatorType Allocator, CCOrderedCollection(CCTextAttribute) AttributedStrings);

/*!
 * @brief Get the attributed strings in selection.
 * @description Modifies the strings of the attributes if necessary.
 * @param Allocator The allocator to be used.
 * @param AttributedStrings The attributed strings to select from.
 * @param Offset The offset of the first character.
 * @param Length The length of characters to select.
 * @return The selected attributed strings. Note: Must destroy the list.
 */
CC_NEW CCOrderedCollection(CCTextAttribute) CCTextAttributeGetSelection(CCAllocatorType Allocator, CCOrderedCollection(CCTextAttribute) AttributedStrings, size_t Offset, size_t Length);

/*!
 * @brief Get the lines of attributed strings that fit within the line width.
 * @description Depending on visibility options, either characters or words that just won't fit within
 *              the given line width will cause lines to be incomplete and return only up to them.
 *
 * @param Allocator The allocator to be used.
 * @param AttributedStrings The attributed strings to separate into lines.
 * @param Visibility The visibility options to define how lines should be structured.
 * @param LineWidth The maximum width of a line.
 * @return The lines of attributed strings. Note: Must destroy the list.
 */
CC_NEW CCOrderedCollection(CCOrderedCollection(CCTextAttribute)) CCTextAttributeGetLines(CCAllocatorType Allocator, CCOrderedCollection(CCTextAttribute) AttributedStrings, CCTextVisibility Visibility, float LineWidth);

#endif
