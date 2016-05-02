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

#include "AssetExpressions.h"
#include "ExpressionHelpers.h"
#include "AssetManager.h"
#include "PixelDataFile.h"
#include "GFX.h"
#include "Font.h"

static CCExpression CCAssetExpressionRetainableValueCopy(CCExpression Value)
{
    return CCExpressionCreateCustomType(Value->allocator, Value->type, CCRetain(Value->data), Value->copy, Value->destructor);
}

CCExpression CCAssetExpressionShader(CCExpression Asset)
{
    CCExpression Ret = Asset;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Asset)) - 1;
    
    if (ArgCount >= 1)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            if (ArgCount == 1)
            {
                GFXShader Shader = CCAssetManagerCreateShader(CCExpressionGetString(Name));
                if (Shader)
                {
                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShader, Shader, CCAssetExpressionRetainableValueCopy, (CCExpressionValueDestructor)GFXShaderDestroy);
                }
            }
            
            else if (ArgCount == 3)
            {
                CCExpression Vertex = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 2));
                CCExpression Fragment = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 3));
                
                if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(Vertex) == CCExpressionValueTypeList) && (CCExpressionGetType(Fragment) == CCExpressionValueTypeList))
                {
                    if ((CCCollectionGetCount(CCExpressionGetList(Vertex)) == 2) && (CCCollectionGetCount(CCExpressionGetList(Fragment)) == 2))
                    {
                        CCExpression VertLib = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vertex), 0);
                        CCExpression VertSrc = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Vertex), 1);
                        
                        CCExpression FragLib = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Fragment), 0);
                        CCExpression FragSrc = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Fragment), 1);
                        
                        if ((CCExpressionGetType(VertLib) == CCExpressionValueTypeAtom) &&
                            (CCExpressionGetType(VertSrc) == CCExpressionValueTypeAtom) &&
                            (CCExpressionGetType(FragLib) == CCExpressionValueTypeAtom) &&
                            (CCExpressionGetType(FragSrc) == CCExpressionValueTypeAtom))
                        {
                            GFXShaderLibrary LibVert = CCAssetManagerCreateShaderLibrary(CCExpressionGetAtom(VertLib));
                            GFXShaderLibrary LibFrag = CCAssetManagerCreateShaderLibrary(CCExpressionGetAtom(FragLib));
                            
                            if ((LibVert) && (LibFrag))
                            {
                                GFXShaderSource Vert = NULL, Frag = NULL;
                                CC_STRING_TEMP_BUFFER(Buffer1, CCExpressionGetAtom(VertSrc)) Vert = GFXShaderLibraryGetSource(LibVert, Buffer1);
                                CC_STRING_TEMP_BUFFER(Buffer2, CCExpressionGetAtom(FragSrc)) Frag = GFXShaderLibraryGetSource(LibFrag, Buffer2);
                                
                                if ((Vert) && (Frag))
                                {
                                    GFXShader Shader = GFXShaderCreate(CC_STD_ALLOCATOR, Vert, Frag);
                                    CCAssetManagerRegisterShader(CCExpressionGetString(Name), Shader);
                                    
                                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShader, Shader, CCAssetExpressionRetainableValueCopy, (CCExpressionValueDestructor)GFXShaderDestroy);
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find the shader source (%S::%S) or (%S::%S)", CCExpressionGetAtom(VertLib), CCExpressionGetAtom(VertSrc), CCExpressionGetAtom(FragLib), CCExpressionGetAtom(FragSrc));
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find the shader library %S or %S", CCExpressionGetAtom(VertLib), CCExpressionGetAtom(FragLib));
                            
                            if (LibVert) GFXShaderLibraryDestroy(LibVert);
                            if (LibFrag) GFXShaderLibraryDestroy(LibFrag);
                        }
                        
                        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Shader source should be of type (lib:atom source:atom)");
                    }
                    
                    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Shader source should be of type (lib:atom source:atom)");
                }
            }
        }
    }
    
    if (Ret == Asset) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("shader", "name:string [vertex-source:list] [fragment-source:list]");
    
    return Ret;
}

CCExpression CCAssetExpressionTexture(CCExpression Asset)
{
    CCExpression Ret = Asset;
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Asset)) - 1;
    
    if (ArgCount >= 1)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            if (ArgCount == 1)
            {
                GFXTexture Texture = CCAssetManagerCreateTexture(CCExpressionGetString(Name));
                if (Texture)
                {
                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeTexture, Texture, CCAssetExpressionRetainableValueCopy, (CCExpressionValueDestructor)GFXTextureDestroy);
                }
            }
            
            else if (ArgCount == 3)
            {
                CCExpression Filter = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 2));
                CCExpression File = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 3));
                
                if ((CCExpressionGetType(Filter) == CCExpressionValueTypeAtom) && (CCExpressionGetType(File) == CCExpressionValueTypeList))
                {
                    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(File));
                    if (ArgCount == 2)
                    {
                        CCExpression SourceType = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(File), 0);
                        CCExpression SourceArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(File), 1);
                        
                        if ((CCExpressionGetType(SourceType) == CCExpressionValueTypeAtom) && (CCExpressionGetType(SourceArg) == CCExpressionValueTypeString))
                        {
                            if (CCStringEqual(CCExpressionGetAtom(SourceType), CC_STRING("dir")))
                            {
                                CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(SourceArg))
                                {
                                    /*
                                     TODO: Change it so it combines textures
                                     Or alternatively maybe make texture combining explicit functionality in the scripts.
                                     */
                                    
                                    FSPath Path = FSPathCreate(Buffer);
                                    CCPixelData Data = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
                                    FSPathDestroy(Path);
                                    
                                    if (Data)
                                    {
                                        GFXTextureHint Filtering = GFXTextureHintFilterModeLinear;
                                        if (CCStringEqual(CCExpressionGetAtom(Filter), CC_STRING("nearest"))) Filtering = GFXTextureHintFilterModeNearest;
                                        else if (!CCStringEqual(CCExpressionGetAtom(Filter), CC_STRING("linear"))) CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid filtering mode: %S", CCExpressionGetAtom(Filter));
                                        
                                        size_t Width, Height, Depth;
                                        CCPixelDataGetSize(Data, &Width, &Height, &Depth);
                                        
                                        GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | (Filtering << GFXTextureHintFilterMin) | (Filtering << GFXTextureHintFilterMag), Data->format, Width, Height, Depth, Data);
                                        
                                        CCAssetManagerRegisterTexture(CCExpressionGetString(Name), Texture);
                                        
                                        Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeTexture, Texture, CCAssetExpressionRetainableValueCopy, (CCExpressionValueDestructor)GFXTextureDestroy);
                                    }
                                    
                                    else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid pixel source: %s", Buffer);
                                }
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid pixel source type: %S", CCExpressionGetAtom(SourceType));
                        }
                    }
                }
            }
        }
    }
    
    if (Ret == Asset) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("texture", "name:string [filter:atom] [path:string]");
    
    return Ret;
}

CCExpression CCAssetExpressionFont(CCExpression Expression)
{
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 3)
    {
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
        
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        CCExpression Size = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
        
        _Bool IsUnicode = FALSE, IsSequential = TRUE;
        int32_t LineHeight = 0, Base = 0;
        GFXTexture Texture = NULL;
        CCFontStyle FontStyle = 0;
        CCArray Glyphs = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCFontGlyph), 1), Letters = CCArrayCreate(CC_STD_ALLOCATOR, sizeof(CCChar), 1);
        size_t Offset = 0;
        CCChar PrevChr = 0;
        
        if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(Size) == CCExpressionValueTypeInteger))
        {
            for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
            {
                CCExpression Option = CCExpressionEvaluate(*Expr);
                if (CCExpressionGetType(Option) == CCExpressionValueTypeList)
                {
                    size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Option));
                    if (ArgCount > 1)
                    {
                        CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Option), 0);
                        if (CCExpressionGetType(Type) == CCExpressionValueTypeAtom)
                        {
                            if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("style")))
                            {
                                CCEnumerator StyleEnumerator;
                                CCCollectionGetEnumerator(CCExpressionGetList(Option), &StyleEnumerator);
                                
                                for (CCExpression *Style = CCCollectionEnumeratorNext(&StyleEnumerator); Style; Style = CCCollectionEnumeratorNext(&StyleEnumerator))
                                {
                                    if (CCExpressionGetType(*Style) == CCExpressionValueTypeAtom)
                                    {
                                        if (CCStringEqual(CCExpressionGetAtom(*Style), CC_STRING("bold")))
                                        {
                                            FontStyle |= CCFontStyleBold;
                                        }
                                        
                                        else if (CCStringEqual(CCExpressionGetAtom(*Style), CC_STRING("italic")))
                                        {
                                            FontStyle |= CCFontStyleItalic;
                                        }
                                    }
                                }
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("unicode")))
                            {
                                IsUnicode = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("line-height")))
                            {
                                LineHeight = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("base")))
                            {
                                Base = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("letter")))
                            {
                                CCEnumerator LetterEnumerator;
                                CCCollectionGetEnumerator(CCExpressionGetList(Option), &LetterEnumerator);
                                
                                CCExpression *Letter = CCCollectionEnumeratorNext(&LetterEnumerator);
                                if ((Letter) && (CCExpressionGetType(*Letter) == CCExpressionValueTypeString))
                                {
                                    CCChar Chr = CCStringGetCharacterAtIndex(CCExpressionGetString(*Letter), 0);
                                    CCArrayAppendElement(Letters, &Chr);
                                    
                                    if (IsSequential)
                                    {
                                        if (!PrevChr) Offset = PrevChr = Chr;
                                        else if ((Chr - PrevChr) == 1) PrevChr = Chr;
                                        else IsSequential = FALSE;
                                    }
                                    
                                    CCFontGlyph Glyph = {
                                        .coord = { .position = CCVector2DFill(0.0f), .size = CCVector2DFill(0.0f) },
                                        .offset = CCVector2DFill(0.0f),
                                        .advance = 0.0f
                                    };
                                    
                                    for (CCExpression *LetterOption = CCCollectionEnumeratorNext(&LetterEnumerator); LetterOption; LetterOption = CCCollectionEnumeratorNext(&LetterEnumerator))
                                    {
                                        if ((CCExpressionGetType(*LetterOption) == CCExpressionValueTypeList) && (CCCollectionGetCount(CCExpressionGetList(*LetterOption)) >= 1))
                                        {
                                            CCExpression Option = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(*LetterOption), 0);
                                            
                                            if (CCExpressionGetType(Option) == CCExpressionValueTypeAtom)
                                            {
                                                if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("glyph")))
                                                {
                                                    Glyph.coord = CCExpressionGetNamedRect(*LetterOption);
                                                }
                                                
                                                else if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("offset")))
                                                {
                                                    Glyph.offset = CCExpressionGetNamedVector2(*LetterOption);
                                                }
                                                
                                                else if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("advance")))
                                                {
                                                    Glyph.advance = CCExpressionGetNamedFloat(*LetterOption);
                                                }
                                            }
                                        }
                                    }
                                    
                                    CCArrayAppendElement(Glyphs, &Glyph);
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("letter", "string [(glyph number number number number) (offset number number) (advance number)]");
                            }
                        }
                    }
                }
                
                else if (CCExpressionGetType(Option) == CCAssetExpressionValueTypeTexture)
                {
                    Texture = CCExpressionGetData(Option);
                }
            }
            
            CCFontCharMap Map = { .letters = Letters };
            if (IsSequential)
            {
                Map.offset = Offset;
                CCArrayDestroy(Letters);
            }
            
            CCFont Font = CCFontCreate(CCExpressionGetString(Name), FontStyle, CCExpressionGetInteger(Size), LineHeight, Base, IsUnicode, IsSequential, Map, Glyphs, Texture);
            
            CCAssetManagerRegisterFont(CCExpressionGetString(Name), Font);
            
            if (Texture) GFXTextureDestroy(Texture);
            if (!IsSequential) CCArrayDestroy(Letters);
            CCArrayDestroy(Glyphs);
        }
    }
    
    return Expression;
}

