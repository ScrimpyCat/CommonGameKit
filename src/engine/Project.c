//
//  Project.c
//  Blob Game
//
//  Created by Stefan Johnson on 23/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#include "Project.h"
#include "Configuration.h"
#include "Expression.h"

static const char CCProjectGamepkg[] =
"(game \"Game\" ; title\n"
"    (default-window-size 640 480) ; window size\n"
"    (default-fullscreen false) ; fullscreen mode\n"
"\n"
"    (dir-fonts \"font/\") ; font directories\n"
"    (dir-levels \"logic/levels/\") ; level directories\n"
"    (dir-rules \"logic/rules/\") ; rule directories\n"
"    (dir-textures \"graphics/textures/\") ; texture directories\n"
"    (dir-shaders \"graphics/shaders/\") ; shader directories\n"
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
    FSHandle Handle;
    if (FSHandleOpen(ProjectPath, FSHandleTypeRead, &Handle) != FSOperationSuccess)
    {
        CC_LOG_ERROR("Failed to open the project");
        return;
    }
    
    size_t Size = FSManagerGetSize(ProjectPath);
    char *Source;
    CC_SAFE_Malloc(Source, sizeof(char) * Size,
                   CC_LOG_ERROR("Failed to allocate memory for the project source file. Allocation size: %zu", sizeof(char) * Size);
                   );
    
    if (Source)
    {
        if (FSHandleRead(Handle, &Size, Source, FSBehaviourDefault) == FSOperationSuccess)
        {
            
        }
        
        else CC_LOG_ERROR("Failed to read the project source file.");
        
        CC_SAFE_Free(Source);
    }
    
    FSHandleClose(Handle);
}
