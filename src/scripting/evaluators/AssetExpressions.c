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
                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShader, Shader, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)GFXShaderDestroy);
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
                            CCString LibV = CCStringCreateWithoutRange(CCExpressionGetAtom(VertLib), 0, 1);
                            CCString LibF = CCStringCreateWithoutRange(CCExpressionGetAtom(FragLib), 0, 1);
                            
                            GFXShaderLibrary LibVert = CCAssetManagerCreateShaderLibrary(LibV);
                            GFXShaderLibrary LibFrag = CCAssetManagerCreateShaderLibrary(LibF);
                            
                            if ((LibVert) && (LibFrag))
                            {
                                CCString VertName = CCStringCopySubstring(CCExpressionGetAtom(VertSrc), 1, CCStringGetLength(CCExpressionGetAtom(VertSrc)) - 1);
                                CCString FragName = CCStringCopySubstring(CCExpressionGetAtom(FragSrc), 1, CCStringGetLength(CCExpressionGetAtom(FragSrc)) - 1);
                                
                                GFXShaderSource Vert = GFXShaderLibraryGetSource(LibVert, VertName), Frag = GFXShaderLibraryGetSource(LibFrag, FragName);
                                CCStringDestroy(VertName);
                                CCStringDestroy(FragName);
                                
                                if ((Vert) && (Frag))
                                {
                                    GFXShader Shader = GFXShaderCreate(CC_STD_ALLOCATOR, Vert, Frag);
                                    CCAssetManagerRegisterShader(CCExpressionGetString(Name), Shader);
                                    
                                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShader, Shader, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)GFXShaderDestroy);
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find the shader source (%S:%S) or (%S:%S)", LibV, CCExpressionGetAtom(VertSrc), LibF, CCExpressionGetAtom(FragSrc));
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_ERROR("Could not find the shader library %S or %S", LibV, LibF);
                            
                            if (LibVert) GFXShaderLibraryDestroy(LibVert);
                            if (LibFrag) GFXShaderLibraryDestroy(LibFrag);
                            
                            CCStringDestroy(LibV);
                            CCStringDestroy(LibF);
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
                    Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeTexture, Texture, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)GFXTextureDestroy);
                }
            }
            
            else if (ArgCount >= 3)
            {
                CCExpression Filter = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 2));
                CCExpression File = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 3));
                
                if ((CCExpressionGetType(Filter) == CCExpressionValueTypeAtom) && (CCExpressionGetType(File) == CCExpressionValueTypeList))
                {
                    const size_t FileArgCount = CCCollectionGetCount(CCExpressionGetList(File));
                    if (FileArgCount == 2)
                    {
                        CCExpression SourceType = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(File), 0);
                        CCExpression SourceArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(File), 1);
                        
                        if ((CCExpressionGetType(SourceType) == CCExpressionValueTypeAtom) && (CCExpressionGetType(SourceArg) == CCExpressionValueTypeString))
                        {
                            if (CCStringEqual(CCExpressionGetAtom(SourceType), CC_STRING("dir:")))
                            {
                                CCString GroupName = 0;
                                if (ArgCount == 4)
                                {
                                    CCExpression Group = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 4));
                                    
                                    if (CCExpressionGetType(Group) == CCExpressionValueTypeList)
                                    {
                                        const size_t GroupArgCount = CCCollectionGetCount(CCExpressionGetList(Group));
                                        if (GroupArgCount == 2)
                                        {
                                            CCExpression GroupType = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Group), 0);
                                            CCExpression GroupArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Group), 1);
                                            
                                            if ((CCExpressionGetType(GroupType) == CCExpressionValueTypeAtom) && (CCExpressionGetType(GroupArg) == CCExpressionValueTypeString))
                                            {
                                                if (CCStringEqual(CCExpressionGetAtom(GroupType), CC_STRING("stream:")))
                                                {
                                                    GroupName = CCExpressionGetString(GroupArg);
                                                }
                                                
                                                else
                                                {
                                                    CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("texture", "name:string [filter:atom] (dir: [path:string]) [(stream: group:string)]");
                                                    
                                                    return Asset;
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(SourceArg))
                                {
                                    FSPath Path = FSPathCreate(Buffer);
                                    CCPixelData Data = CCPixelDataFileCreate(CC_STD_ALLOCATOR, Path);
                                    FSPathDestroy(Path);
                                    
                                    if (Data)
                                    {
                                        GFXTextureHint Filtering = GFXTextureHintFilterModeLinear;
                                        if (CCStringEqual(CCExpressionGetAtom(Filter), CC_STRING(":nearest"))) Filtering = GFXTextureHintFilterModeNearest;
                                        else if (!CCStringEqual(CCExpressionGetAtom(Filter), CC_STRING(":linear"))) CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid filtering mode: %S", CCExpressionGetAtom(Filter));
                                        
                                        size_t Width, Height, Depth;
                                        CCPixelDataGetSize(Data, &Width, &Height, &Depth);
                                        
                                        GFXTexture Texture;
                                        if (GroupName)
                                        {
                                            /*
                                             TODO: Temporary solution for exposing streams to scripting language texture creation
                                             To allow more flexibility with creating streams, will probably expose another asset function to
                                             create the streams directly. And then pass in the stream to be used to the texture function.
                                             
                                             e.g.
                                             (texture-stream "my-stream" :linear :RGBA8Unorm_sRGB (size: 2048 2048)) #etc.
                                             (texture "name" (texture-stream "my-stream") (dir: "blah.png"))
                                             */
                                            GFXTextureStream Stream = CCAssetManagerCreateTextureStream(GroupName);
                                            if (!Stream)
                                            {
                                                Stream = GFXTextureStreamCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | (Filtering << GFXTextureHintFilterMin) | (Filtering << GFXTextureHintFilterMag), CCColourFormatRGBA8Unorm_sRGB, 2048, 2048, 1);
                                                CCAssetManagerRegisterTextureStream(GroupName, Stream);
                                            }
                                            
                                            Texture = GFXTextureCreateFromStream(CC_STD_ALLOCATOR, Stream, Width, Height, Depth, Data);
                                            
                                            GFXTextureStreamDestroy(Stream);
                                        }
                                        
                                        else Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | (Filtering << GFXTextureHintFilterMin) | (Filtering << GFXTextureHintFilterMag), Data->format, Width, Height, Depth, Data);
                                        
                                        CCAssetManagerRegisterTexture(CCExpressionGetString(Name), Texture);
                                        
                                        Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeTexture, Texture, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)GFXTextureDestroy);
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
    
    if (Ret == Asset) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("texture", "name:string [filter:atom] (dir: [path:string]) [(stream: group:string)]");
    
    return Ret;
}

CCExpression CCAssetExpressionFont(CCExpression Expression)
{
    CCExpression Ret = Expression;
    if (CCCollectionGetCount(CCExpressionGetList(Expression)) == 2)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        if (CCExpressionGetType(Name) == CCExpressionValueTypeString)
        {
            CCFont Font = CCAssetManagerCreateFont(CCExpressionGetString(Name));
            if (Font)
            {
                Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeFont, Font, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCFontDestroy);
            }
        }
    }
    
    else if (CCCollectionGetCount(CCExpressionGetList(Expression)) >= 3)
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
                            if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("style:")))
                            {
                                CCEnumerator StyleEnumerator;
                                CCCollectionGetEnumerator(CCExpressionGetList(Option), &StyleEnumerator);
                                
                                for (CCExpression *Style = CCCollectionEnumeratorNext(&StyleEnumerator); Style; Style = CCCollectionEnumeratorNext(&StyleEnumerator))
                                {
                                    if (CCExpressionGetType(*Style) == CCExpressionValueTypeAtom)
                                    {
                                        if (CCStringEqual(CCExpressionGetAtom(*Style), CC_STRING(":bold")))
                                        {
                                            FontStyle |= CCFontStyleBold;
                                        }
                                        
                                        else if (CCStringEqual(CCExpressionGetAtom(*Style), CC_STRING(":italic")))
                                        {
                                            FontStyle |= CCFontStyleItalic;
                                        }
                                    }
                                }
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("unicode:")))
                            {
                                IsUnicode = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("line-height:")))
                            {
                                LineHeight = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("base:")))
                            {
                                Base = CCExpressionGetNamedInteger(Option);
                            }
                            
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING("letter:")))
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
                                                if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("glyph:")))
                                                {
                                                    Glyph.coord = CCExpressionGetNamedRect(*LetterOption);
                                                }
                                                
                                                else if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("offset:")))
                                                {
                                                    Glyph.offset = CCExpressionGetNamedVector2(*LetterOption);
                                                }
                                                
                                                else if (CCStringEqual(CCExpressionGetAtom(Option), CC_STRING("advance:")))
                                                {
                                                    Glyph.advance = CCExpressionGetNamedFloat(*LetterOption);
                                                }
                                            }
                                        }
                                    }
                                    
                                    CCArrayAppendElement(Glyphs, &Glyph);
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_OPTION_ERROR("letter", "string [(glyph: number number number number) (offset: number number) (advance: number)]");
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
            
            CCFont Font = CCFontCreate(CC_STD_ALLOCATOR, CCExpressionGetString(Name), FontStyle, CCExpressionGetInteger(Size), LineHeight, Base, IsUnicode, IsSequential, Map, Glyphs, Texture);
            
            CCAssetManagerRegisterFont(CCExpressionGetString(Name), Font);
            
            if (Texture) GFXTextureDestroy(Texture);
            if (!IsSequential) CCArrayDestroy(Letters);
            CCArrayDestroy(Glyphs);
            
            Ret = CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeFont, Font, CCExpressionRetainedValueCopy, (CCExpressionValueDestructor)CCFontDestroy);
        }
    }
    
    if (Ret == Expression) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("font", "name:string [size:number [...]]");
    
    return Ret;
}

#include "GLGFX.h"
#include "MTLGFX.h"

static CCExpression CCAssetExpressionValueShaderLibraryCopy(CCExpression Value)
{
    return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShaderLibrary, CCRetain(CCExpressionGetData(Value)), CCAssetExpressionValueShaderLibraryCopy, (CCExpressionValueDestructor)CCFree);
}

static void CCAssetExpressionValueShaderLibraryDestructor(CCAssetExpressionValueShaderLibrary *Data)
{
    CCStringDestroy(Data->name);
    GFXShaderLibraryDestroy(Data->library);
}

CCExpression CCAssetExpressionLibrary(CCExpression Expression)
{
    CCExpressionCreateState(Expression, CC_STRING("@source-list"), CCExpressionCreateList(CC_STD_ALLOCATOR), FALSE, NULL, FALSE);
    
    CCExpressionCreateState(Expression, CC_STRING("@opengl"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, GFXMain == GLGFX), FALSE, NULL, FALSE);
    CCExpressionCreateState(Expression, CC_STRING("@metal"), CCExpressionCreateInteger(CC_STD_ALLOCATOR, GFXMain == MTLGFX), FALSE, NULL, FALSE);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *LibName = CCCollectionEnumeratorNext(&Enumerator);
    if ((LibName) && (CCExpressionGetType(*LibName) == CCExpressionValueTypeString))
    {
        for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator)) CCExpressionEvaluate(*Expr);
        
        CCExpression SourceList = CCExpressionGetState(Expression, CC_STRING("@source-list"));
        if (CCExpressionGetType(SourceList) == CCExpressionValueTypeList)
        {
            GFXShaderLibrary Library = GFXShaderLibraryCreate(CC_STD_ALLOCATOR);
            CCAssetManagerRegisterShaderLibrary(CCExpressionGetString(*LibName), Library);
            
            CC_COLLECTION_FOREACH(CCExpression, SourceArg, CCExpressionGetList(SourceList))
            {
                if (CCExpressionGetType(SourceArg) == CCExpressionValueTypeList)
                {
                    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(SourceArg));
                    if (ArgCount == 3)
                    {
                        CCExpression Name = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(SourceArg), 0);
                        CCExpression Type = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(SourceArg), 1);
                        CCExpression Source = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(SourceArg), 2);
                        
                        if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(Type) == CCExpressionValueTypeAtom) && (CCExpressionGetType(Source) == CCExpressionValueTypeExpression))
                        {
                            GFXShaderSourceType ShaderType = GFXShaderSourceTypeVertex;
                            if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING(":vertex"))) ShaderType = GFXShaderSourceTypeVertex;
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING(":fragment"))) ShaderType = GFXShaderSourceTypeFragment;
                            else if (CCStringEqual(CCExpressionGetAtom(Type), CC_STRING(":header"))) ShaderType = GFXShaderSourceTypeHeader;
                            else
                            {
                                CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid shader type: %S", CCExpressionGetAtom(Type));
                                break;
                            }
                            
                            
                            const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Source));
                            if (ArgCount == 2)
                            {
                                CCExpression SourceType = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Source), 0);
                                CCExpression SourceArg = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Source), 1);
                                
                                if ((CCExpressionGetType(SourceType) == CCExpressionValueTypeAtom) && (CCExpressionGetType(SourceArg) == CCExpressionValueTypeString))
                                {
                                    if (CCStringEqual(CCExpressionGetAtom(SourceType), CC_STRING("dir:")))
                                    {
                                        CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(SourceArg))
                                        {
                                            FSPath Path = FSPathCreate(Buffer);
                                            
                                            FSHandle Handle;
                                            if (FSHandleOpen(Path, FSHandleTypeRead, &Handle) == FSOperationSuccess)
                                            {
                                                size_t Size = FSManagerGetSize(Path);
                                                char *Shader;
                                                CC_SAFE_Malloc(Shader, sizeof(char) * (Size + 1));
                                                
                                                FSHandleRead(Handle, &Size, Shader, FSBehaviourDefault);
                                                Shader[Size] = 0;
                                                
                                                FSHandleClose(Handle);
                                                
                                                GFXShaderLibraryCompile(Library, ShaderType, CCExpressionGetString(Name), Shader);
                                                
                                                CC_SAFE_Free(Shader);
                                            }
                                            
                                            FSPathDestroy(Path);
                                        }
                                    }
                                    
                                    else if (CCStringEqual(CCExpressionGetAtom(SourceType), CC_STRING("glsl:")))
                                    {
                                        CC_STRING_TEMP_BUFFER(Shader, CCExpressionGetString(SourceArg))
                                        {
                                            GFXShaderLibraryCompile(Library, ShaderType, CCExpressionGetString(Name), Shader);
                                        }
                                    }
                                    
                                    else
                                    {
                                        CC_EXPRESSION_EVALUATOR_LOG_ERROR("Invalid source type: %S", CCExpressionGetAtom(SourceType));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            
            CCAssetExpressionValueShaderLibrary *ShaderLibrary;
            CC_SAFE_Malloc(ShaderLibrary, sizeof(CCAssetExpressionValueShaderLibrary),
                           CCAssetManagerDeregisterShaderLibrary(CCExpressionGetString(*LibName));
                           GFXShaderLibraryDestroy(Library);
                           CC_LOG_ERROR("Failed to create CCAssetExpressionValueShaderLibrary due to allocation failure. Allocation size (%zu)", sizeof(CCAssetExpressionValueShaderLibrary));
                           return Expression;
                           );
            
            CCMemorySetDestructor(ShaderLibrary, (CCMemoryDestructorCallback)CCAssetExpressionValueShaderLibraryDestructor);
            
            *ShaderLibrary = (CCAssetExpressionValueShaderLibrary){
                .name = CCStringCopy(CCExpressionGetString(*LibName)),
                .library = Library
            };
            
            return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCAssetExpressionValueTypeShaderLibrary, ShaderLibrary, CCAssetExpressionValueShaderLibraryCopy, (CCExpressionValueDestructor)CCFree);
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_ERROR("@source-list state is not a list");
    }
    
    return Expression;
}

CCExpression CCAssetExpressionLibrarySource(CCExpression Expression)
{
    const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Expression)) - 1;
    if (ArgCount == 3)
    {
        CCExpression Name = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 1));
        CCExpression Type = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 2));
        CCExpression Source = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Expression), 3));
        
        if ((CCExpressionGetType(Name) == CCExpressionValueTypeString) && (CCExpressionGetType(Type) == CCExpressionValueTypeAtom) && (CCExpressionGetType(Source) == CCExpressionValueTypeExpression))
        {
            CCExpression List = CCExpressionCopy(CCExpressionGetState(Expression, CC_STRING("@source-list")));
            
            if ((List) && (CCExpressionGetType(List) == CCExpressionValueTypeList))
            {
                CCExpression Value = CCExpressionCreateList(CC_STD_ALLOCATOR);
                CCOrderedCollectionAppendElement(CCExpressionGetList(Value), &(CCExpression){ CCExpressionRetain(Name) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Value), &(CCExpression){ CCExpressionRetain(Type) });
                CCOrderedCollectionAppendElement(CCExpressionGetList(Value), &(CCExpression){ CCExpressionRetain(Source) });
                
                CCOrderedCollectionAppendElement(CCExpressionGetList(List), &Value);
                
                CCExpressionSetState(Expression, CC_STRING("@source-list"), List, FALSE);
            }
            
            else CC_EXPRESSION_EVALUATOR_LOG_ERROR("@source-list state is not a list");
        }
        
        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("source", "name:string type:atom source:expr");
    }
    
    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("source", "name:string type:atom source:expr");
    
    return Expression;
}

CCExpression CCAssetExpressionAsset(CCExpression Expression)
{
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    for (CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator); Expr; Expr = CCCollectionEnumeratorNext(&Enumerator))
    {
        CCExpressionEvaluate(*Expr);
    }
    
    return Expression;
}
