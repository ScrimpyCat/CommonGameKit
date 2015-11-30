//
//  ProjectExpressions.h
//  Blob Game
//
//  Created by Stefan Johnson on 30/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_ProjectExpressions_h
#define Blob_Game_ProjectExpressions_h

#include "ExpressionEvaluator.h"
#include "Configuration.h"

/*
 (game "Game" ; title
    (default-window-size 640 480) ; window size
    (default-fullscreen false) ; fullscreen mode
 
    (dir-fonts "font/") ; font directories
    (dir-levels "logic/levels/") ; level directories
    (dir-rules "logic/rules/") ; rule directories
    (dir-textures "graphics/textures/") ; texture directories
    (dir-shaders "graphics/shaders/") ; shader directories
    (dir-sounds "audio/") ; sound directories
    (dir-layouts "ui/") ; layout directories
    (dir-entities "logic/entities/") ; entity directories
    (dir-logs "logs/") ; log directory
    (dir-tmp "tmp/") ; tmp directory
 )
 */

typedef enum {
    CCProjectExpressionValueTypeGameConfig = CCExpressionValueTypeReservedCount + 1,
    CCProjectExpressionValueTypeReservedCount
} CCProjectExpressionValueType;


static void CCProjectExpressionValueGameConfigDestructor(CCEngineConfig *Data)
{
    CC_SAFE_Free(Data->title);
    CC_SAFE_Free(Data);
}

static void CCProjectExpressionGameConfigDirectoryElementDestructor(CCCollection Collection, FSPath *Element)
{
    FSPathDestroy(*Element);
}

static CC_EXPRESSION_EVALUATOR(game) CCExpression CCProjectExpressionGame(CCExpression Expression)
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
            .layouts = NULL
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
                    
                    if (ArgCount == 1)
                    {
                        Expr = CCCollectionEnumeratorNext(&Enumerator);
                        if ((*Expr)->type == CCExpressionValueTypeString)
                        {
                            FSPath Path = FSPathCreate((*Expr)->string);
                            
                            if (!strcmp(Dir, "logs")) Config.directory.logs = Path;
                            else if (!strcmp(Dir, "tmp")) Config.directory.tmp = Path;
                            else FSPathDestroy(Path);
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
                        
                        if (!strcmp(Dir, "fonts")) Config.directory.fonts = Directories;
                        else if (!strcmp(Dir, "levels")) Config.directory.levels = Directories;
                        else if (!strcmp(Dir, "rules")) Config.directory.rules = Directories;
                        else if (!strcmp(Dir, "textures")) Config.directory.textures = Directories;
                        else if (!strcmp(Dir, "shaders")) Config.directory.shaders = Directories;
                        else if (!strcmp(Dir, "sounds")) Config.directory.sounds = Directories;
                        else if (!strcmp(Dir, "layouts")) Config.directory.layouts = Directories;
                        else if (!strcmp(Dir, "entities")) Config.directory.entities = Directories;
                        else if ((!strcmp(Dir, "logs")) || (!strcmp(Dir, "tmp"))) CC_EXPRESSION_EVALUATOR_LOG_FUNCTION_ERROR(Dir - 4, "path:string");
                        else CCCollectionDestroy(Directories);
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
        
    return Result;
}

#endif
