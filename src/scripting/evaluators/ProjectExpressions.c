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
    CCCollectionGetEnumerator(Expression->list, &Enumerator);
    
    CCExpression *Expr = CCCollectionEnumeratorNext(&Enumerator);
    if ((Expr) && ((*Expr)->type == CCExpressionValueTypeString))
    {
        Config.title = (*Expr)->string;
        (*Expr)->destructor = NULL;
    }
    
    for (Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
    {
        if ((*Expr)->type == CCExpressionValueTypeExpression)
        {
            size_t ArgCount = CCCollectionGetCount((*Expr)->list) - 1;
            
            CCEnumerator Enumerator;
            CCCollectionGetEnumerator((*Expr)->list, &Enumerator);
            
            Expr = CCCollectionEnumeratorGetCurrent(&Enumerator);
            
            if ((Expr) && ((*Expr)->type == CCExpressionValueTypeAtom))
            {
                if (!strcmp((*Expr)->atom, "default-resolution"))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression *Width = CCCollectionEnumeratorNext(&Enumerator), *Height = CCCollectionEnumeratorNext(&Enumerator);
                        
                        if (((*Width)->type == CCExpressionValueTypeInteger) && ((*Height)->type == CCExpressionValueTypeInteger))
                        {
                            Config.window.width = (*Width)->integer;
                            Config.window.height = (*Height)->integer;
                        }
                        
                        else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-resolution", "width:integer height:integer");
                    }
                    
                    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-resolution", "width:integer height:integer");
                }
                
                else if (!strcmp((*Expr)->atom, "default-fullscreen"))
                {
                    CCExpression *Fullscreen = CCCollectionEnumeratorNext(&Enumerator);
                    if ((ArgCount == 1) && ((*Fullscreen)->type == CCExpressionValueTypeAtom)) //TODO: Later add atom evaluators, so this can instead be changed to an integer
                    {
                        if (!strcmp((*Fullscreen)->atom, "true"))
                        {
                            Config.window.fullscreen = TRUE;
                        }
                        
                        else if (!strcmp((*Fullscreen)->atom, "false"))
                        {
                            Config.window.fullscreen = FALSE;
                        }
                    }
                    
                    else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR("default-fullscreen", "fullscreen:atom");
                }
                
                else if (!strncmp((*Expr)->atom, "dir-", 4))
                {
                    const char *Dir = (*Expr)->atom + 4;
                    
                    struct {
                        const char *atom;
                        void *attribute;
                        _Bool path;
                    } Commands[] = {
                        { "fonts", &Config.directory.fonts, FALSE },
                        { "levels", &Config.directory.levels, FALSE },
                        { "rules", &Config.directory.rules, FALSE },
                        { "textures", &Config.directory.textures, FALSE },
                        { "shaders", &Config.directory.shaders, FALSE },
                        { "sounds", &Config.directory.sounds, FALSE },
                        { "layouts", &Config.directory.layouts, FALSE },
                        { "entities", &Config.directory.entities, FALSE },
                        { "logs", &Config.directory.logs, TRUE },
                        { "tmp", &Config.directory.tmp, TRUE }
                    };
                    
                    for (size_t Loop = 0; Loop < sizeof(Commands) / sizeof(typeof(*Commands)); Loop++)
                    {
                        if (!strcmp(Dir, Commands[Loop].atom))
                        {
                            if (Commands[Loop].path)
                            {
                                if (ArgCount == 1)
                                {
                                    Expr = CCCollectionEnumeratorNext(&Enumerator);
                                    if ((*Expr)->type == CCExpressionValueTypeString)
                                    {
                                        *(FSPath*)(Commands[Loop].attribute) = FSPathCreate((*Expr)->string);
                                    }
                                }
                                
                                else CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Dir - 4, "path:string");
                            }
                            
                            else
                            {
                                CCCollection Directories = CCCollectionCreate(CC_STD_ALLOCATOR, CCCollectionHintSizeSmall | CCCollectionHintHeavyEnumerating, sizeof(FSPath), (CCCollectionElementDestructor)CCProjectExpressionGameConfigDirectoryElementDestructor);
                                
                                //TODO: Make a directory expression
                                for (Expr = NULL; (Expr = CCCollectionEnumeratorNext(&Enumerator)); )
                                {
                                    if ((*Expr)->type == CCExpressionValueTypeString)
                                    {
                                        CCCollectionInsertElement(Directories, &(FSPath){ FSPathCreate((*Expr)->string) });
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
    
    CCExpression Result = CCExpressionCreate(CC_STD_ALLOCATOR, (CCExpressionValueType)CCProjectExpressionValueTypeGameConfig);
    CC_SAFE_Malloc(Result->data, sizeof(CCEngineConfig),
                   CC_LOG_ERROR("Failed to allocate CCEngineConfig for expression. Allocation size: %zu", sizeof(CCEngineConfig));
                   );
    
    if (Result->data)
    {
        *(CCEngineConfig*)Result->data = Config;
        Result->destructor = (CCExpressionValueDestructor)CCProjectExpressionValueGameConfigDestructor;
    }
    
    else CCProjectExpressionValueGameConfigDestructor(&Config);
    
    return Result;
}
