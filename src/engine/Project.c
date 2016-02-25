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

#include "Project.h"
#include "Configuration.h"
#include "Expression.h"
#include "ProjectExpressions.h"

static const char CCProjectGamepkg[] =
"(game \"Game\" ; title\n"
"    (default-resolution 640 480) ; resolution or window size\n"
"    (default-fullscreen false) ; fullscreen mode\n"
"\n"
"    (dir-fonts \"font/\") ; font directories\n"
"    (dir-levels \"logic/levels/\") ; level directories\n"
"    (dir-rules \"logic/rules/\") ; rule directories\n"
"    (dir-textures \"graphics/textures/\") ; texture directories\n"
"    (dir-shaders \"graphics/shaders/\" (engine-path \"assets/shaders/\")) ; shader directories\n"
"    (dir-sounds \"audio/\") ; sound directories\n"
"    (dir-layouts \"ui/\") ; layout directories\n"
"    (dir-entities \"logic/entities/\") ; entity directories\n"
"    (dir-logs \"logs/\") ; log directory\n"
"    (dir-tmp \"tmp/\") ; tmp directory\n"
")\n";

FSPath CCProjectCreate(const char *Directory)
{
    FSPath Path = FSPathCreateFromSystemPath(Directory);
    
    FSPathComponent Component = FSPathGetComponentAtIndex(Path, FSPathGetComponentCount(Path) - 1);
    if ((FSPathComponentGetType(Component) != FSPathComponentTypeExtension) || (FSPathComponentGetString(Component) == NULL) || (strcmp(FSPathComponentGetString(Component), "gamepkg")))
    {
        CC_LOG_ERROR("Failed to create project, project file should be of type .gamepkg");
        FSPathDestroy(Path);
        return NULL;
    }
    
    if (FSManagerCreate(Path, FALSE) == FSOperationSuccess)
    {
        FSHandle Handle;
        if (FSHandleOpen(Path, FSHandleTypeWrite, &Handle) != FSOperationSuccess)
        {
            CC_LOG_ERROR("Failed to open the project");
            FSPathDestroy(Path);
            return NULL;
        }
        
        FSHandleRemove(Handle, SIZE_MAX, FSBehaviourDefault);
        FSHandleWrite(Handle, sizeof(CCProjectGamepkg) - 1, CCProjectGamepkg, FSBehaviourDefault);
        FSHandleClose(Handle);
        
        
        FSPath ProjectDir = FSPathCopy(Path);
        for (size_t Loop = FSPathGetComponentCount(ProjectDir) - 1; Loop > 0; Loop--)
        {
            FSPathComponentType Type = FSPathComponentGetType(FSPathGetComponentAtIndex(ProjectDir, Loop));
            if ((Type != FSPathComponentTypeExtension) && (Type != FSPathComponentTypeFile))  break;
            
            FSPathRemoveComponentIndex(ProjectDir, Loop);
        }
        
        //font
        FSPathAppendComponent(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "font"));
        FSManagerCreate(ProjectDir, FALSE);
        
        //tmp
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "tmp"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //logs
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "logs"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //ui
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "ui"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //audio
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "audio"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //graphics/textures
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "graphics"), FSPathGetComponentCount(ProjectDir) - 1);
        FSPathAppendComponent(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "textures"));
        FSManagerCreate(ProjectDir, TRUE);
        
        //graphics/shaders
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "shaders"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //logic/levels
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "logic"), FSPathGetComponentCount(ProjectDir) - 2);
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "levels"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, TRUE);
        
        //logic/rules
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "rules"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        //logic/entities
        FSPathSetComponentAtIndex(ProjectDir, FSPathComponentCreate(FSPathComponentTypeDirectory, "entities"), FSPathGetComponentCount(ProjectDir) - 1);
        FSManagerCreate(ProjectDir, FALSE);
        
        FSPathDestroy(ProjectDir);
    }
    
    else
    {
        CC_LOG_ERROR("Failed to create project file at path: %s", FSPathGetFullPathString(Path));
        FSPathDestroy(Path);
        return NULL;
    }
    
    return Path;
}

void CCProjectLoad(FSPath ProjectPath)
{
    CCExpression Expression = CCExpressionCreateFromSourceFile(ProjectPath);
    if (Expression)
    {
        FSPath EnginePath = FSPathCreateFromSystemPath(__FILE__);
        FSPathRemoveComponentLast(EnginePath); // src/engine/Project.c
        FSPathRemoveComponentLast(EnginePath); // src/engine/Project
        FSPathRemoveComponentLast(EnginePath); // src/engine/
        FSPathRemoveComponentLast(EnginePath); // src/
        CCExpressionCreateState(Expression, CC_STRING("engine-path"), CCExpressionCreateString(CC_STD_ALLOCATOR, CCStringCreate(CC_STD_ALLOCATOR, CCStringEncodingUTF8 | CCStringHintCopy, FSPathGetPathString(EnginePath)), FALSE), FALSE);
        FSPathDestroy(EnginePath);
        
        
        CCExpression Result = CCExpressionEvaluate(Expression);
        
        if (CCExpressionGetType(Result) == CCProjectExpressionValueTypeGameConfig)
        {
            ((CCEngineConfig*)CCExpressionGetData(Result))->launch = CCEngineConfiguration.launch;
            CCEngineConfiguration = *(CCEngineConfig*)CCExpressionGetData(Result);
            CCEngineConfiguration.project = ProjectPath;
            
            CCExpressionChangeOwnership(Result, NULL, CCFree);
        }
        
        else CC_LOG_ERROR("Failed to evaluate the project source file.");
        
        CCExpressionDestroy(Expression);
    }
    
    else CC_LOG_ERROR("Failed to read the project source file.");
}
