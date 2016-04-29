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
#include "AssetManager.h"
#include "PixelDataFile.h"
#include "GFX.h"

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
            
            else if (ArgCount == 2)
            {
                CCExpression File = CCExpressionEvaluate(*(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Asset), 2));
                
                if (CCExpressionGetType(File) == CCExpressionValueTypeList)
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
                                        size_t Width, Height, Depth;
                                        CCPixelDataGetSize(Data, &Width, &Height, &Depth);
                                        
                                        GFXTexture Texture = GFXTextureCreate(CC_STD_ALLOCATOR, GFXTextureHintDimension2D | (GFXTextureHintFilterModeLinear << GFXTextureHintFilterMin) | (GFXTextureHintFilterModeLinear << GFXTextureHintFilterMag), Data->format, Width, Height, Depth, Data);
                                        
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
    
    if (Ret == Asset) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("texture", "name:string [path:string]");
    
    return Ret;
}

