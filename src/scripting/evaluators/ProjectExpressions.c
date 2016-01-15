/*
 *  Copyright (c) 2015, Stefan Johnson
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

#include "ProjectExpressions.h"
#include "Configuration.h"

#warning Need to add a copy function
static void CCProjectExpressionValueGameConfigDestructor(CCEngineConfig *Data)
{
    CC_SAFE_Free(Data->title);
    if (Data->directory.fonts) CCCollectionDestroy(Data->directory.fonts);
    if (Data->directory.levels) CCCollectionDestroy(Data->directory.levels);
    if (Data->directory.rules) CCCollectionDestroy(Data->directory.rules);
    if (Data->directory.textures) CCCollectionDestroy(Data->directory.textures);
    if (Data->directory.shaders) CCCollectionDestroy(Data->directory.shaders);
    if (Data->directory.sounds) CCCollectionDestroy(Data->directory.sounds);
    if (Data->directory.layouts) CCCollectionDestroy(Data->directory.layouts);
    if (Data->directory.logs) FSPathDestroy(Data->directory.logs);
    if (Data->directory.tmp) FSPathDestroy(Data->directory.tmp);
    CC_SAFE_Free(Data);
}

static void CCProjectExpressionGameConfigDirectoryElementDestructor(CCCollection Collection, FSPath *Element)
{
    FSPathDestroy(*Element);
}

CCExpression CCProjectExpressionGame(CCExpression Expression)
{
    CCEngineConfig Config = {
        .launch = CCLaunchOptionGame,
        .project = NULL,
        .title = NULL,
        .window = {
            .width = 0,
            .height = 0,
            .fullscreen = FALSE
        },
        .directory = {
            .fonts = NULL,
            .levels = NULL,
            .rules = NULL,
            .textures = NULL,
            .shaders = NULL,
            .sounds = NULL,
            .layouts = NULL,
            .logs = NULL,
            .tmp = NULL
        }
    };
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(CCExpressionGetList(Expression), &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator);
    if (Expr)
    {
        CCExpression Result = CCExpressionEvaluate(*Expr);
        if (CCExpressionGetType(Result) == CCExpressionValueTypeString)
        {
            Config.title = (char*)CCExpressionGetString(Result);
            CCExpressionChangeOwnership(Result, NULL, NULL);
        }
        
        for (Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
        {
            Result = CCExpressionEvaluate(*Expr);
            if (CCExpressionGetType(Result) == CCExpressionValueTypeExpression)
            {
                size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Result)) - 1;
                
                CCEnumerator Enumerator;
                CCCollectionGetEnumerator(CCExpressionGetList(Result), &Enumerator);
                
                Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
                
                if (Expr)
                {
                    Result = CCExpressionEvaluate(*Expr);
                    if (CCExpressionGetType(Result) == CCExpressionValueTypeAtom)
                    {
                        if (CCStringEqual(CCExpressionGetAtom(Result), CC_STRING("default-resolution")))
                        {
                            if (ArgCount == 2)
                            {
                                CCExpression Width = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator)), Height = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
                                
                                if ((CCExpressionGetType(Width) == CCExpressionValueTypeInteger) && (CCExpressionGetType(Height) == CCExpressionValueTypeInteger))
                                {
                                    Config.window.width = CCExpressionGetInteger(Width);
                                    Config.window.height = CCExpressionGetInteger(Height);
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-resolution", "width:integer height:integer");
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-resolution", "width:integer height:integer");
                        }
                        
                        else if (CCStringEqual(CCExpressionGetAtom(Result), CC_STRING("default-fullscreen")))
                        {
                            if (ArgCount == 1)
                            {
                                CCExpression Fullscreen = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
                                
                                if (CCExpressionGetType(Fullscreen) == CCExpressionValueTypeAtom) //TODO: Later add atom evaluators, so this can instead be changed to an integer
                                {
                                    if (CCStringEqual(CCExpressionGetAtom(Fullscreen), CC_STRING("true")))
                                    {
                                        Config.window.fullscreen = TRUE;
                                    }
                                    
                                    else if (CCStringEqual(CCExpressionGetAtom(Fullscreen), CC_STRING("false")))
                                    {
                                        Config.window.fullscreen = FALSE;
                                    }
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-fullscreen", "fullscreen:atom");
                            }
                            
                            else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-fullscreen", "fullscreen:atom");
                        }
                        
                        else if (CCStringHasPrefix(CCExpressionGetAtom(Result), CC_STRING("dir-")))
                        {
                            CCString Dir = CCExpressionGetAtom(Result);
                            
                            struct {
                                CCString atom;
                                void *attribute;
                                _Bool path;
                            } Commands[] = {
                                { CC_STRING("fonts"), &Config.directory.fonts, FALSE },
                                { CC_STRING("levels"), &Config.directory.levels, FALSE },
                                { CC_STRING("rules"), &Config.directory.rules, FALSE },
                                { CC_STRING("textures"), &Config.directory.textures, FALSE },
                                { CC_STRING("shaders"), &Config.directory.shaders, FALSE },
                                { CC_STRING("sounds"), &Config.directory.sounds, FALSE },
                                { CC_STRING("layouts"), &Config.directory.layouts, FALSE },
                                { CC_STRING("entities"), &Config.directory.entities, FALSE },
                                { CC_STRING("logs"), &Config.directory.logs, TRUE },
                                { CC_STRING("tmp"), &Config.directory.tmp, TRUE }
                            };
                            
                            for (size_t Loop = 0; Loop < sizeof(Commands) / sizeof(typeof(*Commands)); Loop++)
                            {
                                if (CCStringHasSuffix(Dir, Commands[Loop].atom))
                                {
                                    if (Commands[Loop].path)
                                    {
                                        if (ArgCount == 1)
                                        {
                                            Result = CCExpressionEvaluate(*(CCExpression*)CCCollectionEnumeratorNext(&Enumerator));
                                            if (CCExpressionGetType(Result) == CCExpressionValueTypeString)
                                            {
                                                CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Result)) *(FSPath*)(Commands[Loop].attribute) = FSPathCreate(Buffer);
                                            }
                                        }
                                        
                                        else
                                        {
                                            CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Result)) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Buffer, "path:string");
                                        }
                                    }
                                    
                                    else
                                    {
                                        CCCollection Directories = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyEnumerating, sizeof(FSPath), (CCCollectionElementDestructor)CCProjectExpressionGameConfigDirectoryElementDestructor);
                                        
                                        //TODO: Make a directory expression
                                        for (Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
                                        {
                                            Result = CCExpressionEvaluate(*Expr);
                                            if (CCExpressionGetType(Result) == CCExpressionValueTypeString)
                                            {
                                                CC_STRING_TEMP_BUFFER(Buffer, CCExpressionGetString(Result)) CCCollectionInsertElement(Directories, &(FSPath){ FSPathCreate(Buffer) });
                                            }
                                        }
                                        
                                        *(CCCollection*)(Commands[Loop].attribute) = Directories;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CCEngineConfig *Result;
    CC_SAFE_Malloc(Result, sizeof(CCEngineConfig),
                   CC_LOG_ERROR("Failed to allocate CCEngineConfig for expression. Allocation size: %zu", sizeof(CCEngineConfig));
                   );
    
    if (Result)
    {
        *(CCEngineConfig*)Result = Config;
        
        return CCExpressionCreateCustomType(CC_STD_ALLOCATOR, (CCExpressionValueType)CCProjectExpressionValueTypeGameConfig, Result, NULL, (CCExpressionValueDestructor)CCProjectExpressionValueGameConfigDestructor);
    }
    
    else CCProjectExpressionValueGameConfigDestructor(&Config);
    
    return Expression;
}
