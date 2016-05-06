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

#include "TextAttribute.h"

static void CCTextAttributeElementDestructor(CCCollection Collection, CCTextAttribute *Element);

const CCCollectionElementDestructor CCTextAttributeDestructorForCollection = (CCCollectionElementDestructor)CCTextAttributeElementDestructor;

static void CCTextAttributeElementDestructor(CCCollection Collection, CCTextAttribute *Element)
{
    if (Element->string) CCStringDestroy(Element->string);
    if (Element->font) CCFontDestroy(Element->font);
}

size_t CCTextAttributeGetLength(CCOrderedCollection AttributedStrings)
{
    CCAssertLog(AttributedStrings, "AttributedStrings must not be null");
    
    size_t Length = 0;
    CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attr, AttributedStrings)
    {
        if (Attr->string) Length += CCStringGetLength(Attr->string);
    }
    
    return Length;
}

static _Bool CCTextAttributeMergeable(const CCTextAttribute *Attr1, const CCTextAttribute *Attr2)
{
    return !memcmp(&Attr1->font, &Attr2->font, sizeof(CCTextAttribute) - offsetof(CCTextAttribute, font));
}

CCOrderedCollection CCTextAttributeMerge(CCAllocatorType Allocator, CCOrderedCollection AttributedStrings)
{
    CCAssertLog(AttributedStrings, "AttributedStrings must not be null");
    
    CCOrderedCollection Strings = CCCollectionCreate(Allocator, CCCollectionHintHeavyEnumerating, sizeof(CCTextAttribute), CCTextAttributeDestructorForCollection);
    
    CCTextAttribute Attribute = { .string = 0 };
    CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attr, AttributedStrings)
    {
        if (Attr->string)
        {
            if (Attribute.string)
            {
                if (CCTextAttributeMergeable(&Attribute, Attr))
                {
                    CCString Temp = Attribute.string;
                    Attribute.string = CCStringCreateByInsertingString(Temp, CCStringGetLength(Attribute.string), Attr->string);
                    CCStringDestroy(Temp);
                }
                
                else
                {
                    CCOrderedCollectionAppendElement(Strings, &Attribute);
                    
                    Attribute = *Attr;
                    Attribute.string = CCStringCopy(Attribute.string);
                    CCRetain(Attribute.font);
                }
            }
            
            else
            {
                Attribute = *Attr;
                Attribute.string = CCStringCopy(Attribute.string);
                CCRetain(Attribute.font);
            }
        }
    }
    
    if (Attribute.string)
    {
        CCOrderedCollectionAppendElement(Strings, &Attribute);
    }
    
    return Strings;
}

CCOrderedCollection CCTextAttributeGetSelection(CCAllocatorType Allocator, CCOrderedCollection AttributedStrings, size_t ChrOffset, size_t ChrLength)
{
    CCAssertLog(AttributedStrings, "AttributedStrings must not be null");
    
    CCOrderedCollection Attributes = CCCollectionCreate(Allocator, CCCollectionHintHeavyEnumerating, sizeof(CCTextAttribute), CCTextAttributeDestructorForCollection);
    
    if (ChrLength)
    {
        size_t Offset = 0;
        CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, AttributedStrings)
        {
            const size_t Length = CCStringGetLength(Attribute->string);
            
            CCString String = 0;
            if (ChrOffset < (Offset + Length))
            {
                size_t AdjustedOffset;
                if (ChrOffset >= Offset)
                {
                    const size_t Index = ChrOffset - Offset;
                    String = CCStringCreateWithoutRange(Attribute->string, 0, Index);
                    
                    if (ChrLength < (Length - Index))
                    {
                        CCString Temp = String;
                        String = CCStringCreateWithoutRange(String, ChrLength, Length - (Index + ChrLength));
                        CCStringDestroy(Temp);
                    }
                }
                
                else if (ChrLength > (AdjustedOffset = (Offset - ChrOffset)))
                {
                    if (ChrLength < (AdjustedOffset + Length)) String = CCStringCreateWithoutRange(Attribute->string, ChrLength - AdjustedOffset, (AdjustedOffset + Length) - ChrLength);
                    else String = CCStringCopy(Attribute->string);
                }
                
                else break;
                
                CCTextAttribute Attr = *Attribute;
                Attr.string = String;
                CCRetain(Attr.font);
                CCOrderedCollectionAppendElement(Attributes, &Attr);
            }
            
            Offset += Length;
        }
    }
    
    return Attributes;
}

static void CCTextAttributeCollectionElementDestructor(CCCollection Collection, CCCollection *Element)
{
    CCCollectionDestroy(*Element);
}

static CCFontAttribute CCTextAttributeGetFontAttribute(CCTextAttribute *Attribute)
{
    return CCFontAttributeAdjustSpacing(CCFontAttributeAdjustScaling(CCFontAttributeDefault(), Attribute->scale), Attribute->space);
}

CCOrderedCollection CCTextAttributeGetLines(CCAllocatorType Allocator, CCOrderedCollection AttributedStrings, CCTextVisibility Visibility, float LineWidth)
{
    CCAssertLog(AttributedStrings, "AttributedStrings must not be null");
    
    CCOrderedCollection Lines = CCCollectionCreate(Allocator, CCCollectionHintHeavyEnumerating, sizeof(CCOrderedCollection), (CCCollectionElementDestructor)CCTextAttributeCollectionElementDestructor);
    
    size_t Offset = 0, Length = 0, WordStart = 0, WordAttributeCount = 0, Skip = 0;
    CCVector2D Cursor = CCVector2DFill(0.0f);
    _Bool PrevAttr = FALSE;
    CC_COLLECTION_FOREACH_PTR(CCTextAttribute, Attribute, AttributedStrings)
    {
        if (CC_UNLIKELY(PrevAttr))
        {
            PrevAttr = FALSE;
            Attribute = CCCollectionEnumeratorPrevious(&CC_COLLECTION_CURRENT_ENUMERATOR);
        }
        
        WordAttributeCount++;
        
        size_t AttrLength = 0;
        _Bool Prev = FALSE;
        CC_STRING_FOREACH(Letter, Attribute->string)
        {
            if (CC_UNLIKELY(Prev))
            {
                Prev = FALSE;
                Letter = CCStringEnumeratorPrevious(&CC_STRING_CURRENT_ENUMERATOR);
            }
            
            if (Skip)
            {
                Skip--;
                continue;
            }
            
            AttrLength++;
            Length++;
            
            if (isspace(Letter))
            {
                WordStart = Length;
                WordAttributeCount = 1;
                
                if (Letter == '\n')
                {
                    WordStart = 0;
                    
                    CCOrderedCollectionAppendElement(Lines, &(CCOrderedCollection){ CCTextAttributeGetSelection(Allocator, AttributedStrings, Offset, Length) });
                    
                    Offset += Length;
                    Length = 0;
                    Cursor = CCVector2DFill(0.0f);
                    continue;
                }
            }
            
            const CCFontGlyph *Glyph = CCFontGetGlyph(Attribute->font, Letter);
            if (Glyph)
            {
                CCRect Rect;
                Cursor = CCFontPositionGlyph(Attribute->font, Glyph, CCTextAttributeGetFontAttribute(Attribute), Cursor, &Rect, NULL);
                
                if ((Cursor.x >= LineWidth) || ((Rect.position.x + Rect.size.x) > LineWidth))
                {
                    _Bool StartAgain = FALSE;
                    if (Visibility & CCTextVisibilityCharacter)
                    {
                        if ((Rect.position.x + Rect.size.x) > LineWidth)
                        {
                            if (Length == 1) return Lines;
                            
                            if (CCStringEnumeratorGetIndex(&CC_STRING_CURRENT_ENUMERATOR)) CCStringEnumeratorPrevious(&CC_STRING_CURRENT_ENUMERATOR);
                            else Prev = TRUE;
                            
                            Length--;
                        }
                    }
                    
                    else
                    {
                        if (!WordStart)
                        {
                            if ((Rect.position.x + Rect.size.x) > LineWidth) return Lines;
                            
                            CCChar Next = CCStringEnumeratorNext(&CC_STRING_CURRENT_ENUMERATOR);
                            if (!Next)
                            {
                                size_t Loop = 1;
                                for (CCTextAttribute *Attr; (Attr = CCCollectionEnumeratorNext(&CC_COLLECTION_CURRENT_ENUMERATOR)); Loop++)
                                {
                                    if (CCStringGetLength(Attr->string))
                                    {
                                        Next = CCStringGetCharacterAtIndex(Attr->string, 0);
                                        break;
                                    }
                                }
                                
                                while (Loop--) CCCollectionEnumeratorPrevious(&CC_COLLECTION_CURRENT_ENUMERATOR);
                            }
                            
                            CCStringEnumeratorPrevious(&CC_STRING_CURRENT_ENUMERATOR);
                            
                            if (!isspace(Next)) return Lines;
                        }
                        
                        else if (WordStart != Length)
                        {
                            const size_t WordLength = Length - WordStart;
                            Length -= WordLength;
                            
                            if ((WordAttributeCount > 1) && (WordLength >= AttrLength))
                            {
                                size_t Chrs = AttrLength;
                                for (CCTextAttribute *Attr; (Attr = CCCollectionEnumeratorPrevious(&CC_COLLECTION_CURRENT_ENUMERATOR)); )
                                {
                                    const size_t Len = CCStringGetLength(Attr->string);
                                    if (Len > (WordLength - Chrs))
                                    {
                                        Skip = (Len + Chrs) - WordLength;
                                        break;
                                    }
                                    
                                    Chrs += Len;
                                }
                                
                                if (!CCOrderedCollectionGetIndex(AttributedStrings, CCCollectionEnumeratorGetEntry(&CC_COLLECTION_CURRENT_ENUMERATOR))) PrevAttr = TRUE;
                                else CCCollectionEnumeratorPrevious(&CC_COLLECTION_CURRENT_ENUMERATOR);
                                
                                WordAttributeCount = 0;
                                StartAgain = TRUE;
                            }
                            
                            else
                            {
                                for (size_t Loop = 0, Count = CCStringEnumeratorGetIndex(&CC_STRING_CURRENT_ENUMERATOR); Loop < WordLength; Loop++, Count--)
                                {
                                    if (Count) CCStringEnumeratorPrevious(&CC_STRING_CURRENT_ENUMERATOR);
                                    else Prev = TRUE;
                                }
                            }
                            
                            WordStart = 0;
                        }
                        
                        else if ((Rect.position.x + Rect.size.x) > LineWidth) return Lines;
                    }
                    
                    CCOrderedCollectionAppendElement(Lines, &(CCOrderedCollection){ CCTextAttributeGetSelection(Allocator, AttributedStrings, Offset, Length) });
                    
                    Offset += Length;
                    Length = 0;
                    Cursor = CCVector2DFill(0.0f);
                    
                    if (Visibility & CCTextVisibilitySingleLine) return Lines;
                    if (StartAgain) break;
                }
            }
        }
    }
    
    if (Length) CCOrderedCollectionAppendElement(Lines, &(CCOrderedCollection){ CCTextAttributeGetSelection(Allocator, AttributedStrings, Offset, Length) });
    
    return Lines;
}
