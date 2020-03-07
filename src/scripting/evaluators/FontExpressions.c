/*
 *  Copyright (c) 2020, Stefan Johnson
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

#include "FontExpressions.h"
#include "Font.h"
#include "AssetExpressions.h"
#include "ExpressionHelpers.h"
#include "AssetManager.h"
#include "GraphicsExpressions.h"

static CCFontAttribute CCFontExpressionLoadAttribute(CCFont *Font, CCEnumerator *Enumerator)
{
    CCFontAttribute Attribute = {
        .scale = CCGraphicsExpressionDefaultTextAttribute.scale,
        .space = CCGraphicsExpressionDefaultTextAttribute.space
    };
    
    for (CCExpression *Element = CCCollectionEnumeratorNext(Enumerator); Element; Element = CCCollectionEnumeratorNext(Enumerator))
    {
        CCExpression Arg = CCExpressionEvaluate(*Element);
        
        if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
        {
            size_t Count = CCCollectionGetCount(CCExpressionGetList(Arg));
            if (Count > 1)
            {
                CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
                if (CCExpressionGetType(Type) == CCExpressionValueTypeAtom)
                {
                    if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("scale:"))) Attribute.scale = CCExpressionGetNamedVector2(Arg);
                    else if (CCStringEqual(CCExpressionGetString(Type), CC_STRING("space:"))) Attribute.space = CCExpressionGetNamedFloat(Arg);
                }
            }
        }
        
        else if (CCExpressionGetType(Arg) == CCAssetExpressionValueTypeFont)
        {
            *Font = CCRetain(CCExpressionGetData(Arg));
        }
    }
    
    if (!*Font) *Font = CCGraphicsExpressionDefaultFont();
    
    return Attribute;
}

CCExpression CCFontExpressionGetLineHeight(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 1)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCFont Font;
        const CCFontAttribute Attribute = CCFontExpressionLoadAttribute(&Font, &Enumerator);
        const float Height = CCFontGetLineHeight(Font, Attribute);
        CCFontDestroy(Font);
        
        return CCExpressionCreateFloat(CC_STD_ALLOCATOR, Height);
    }
    
    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("font-line-height", "[font:font] [...]");
    
    return Expression;
}
