//
//  Configuration.c
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#include "Configuration.h"
#include <stdlib.h>
#include <string.h>
#include "Project.h"


CCEngineConfig CCEngineConfiguration = {
    .launch = CCLaunchOptionGame,
    .project = NULL,
    .title = NULL,
    .window = {
        .width = 640,
        .height = 480,
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

void CCConfigureOptions(int argc, const char *argv[])
{
    for (int Loop = 0; Loop < argc; Loop++)
    {
        if (argv[Loop][0] == '-')
        {
            if (Loop + 1 < argc)
            {
                if (!strcmp(argv[Loop], "-game")) //-game [path]
                {
                    CCEngineConfiguration.launch = CCLaunchOptionGame;
                    CCEngineConfiguration.project = FSPathCreateFromSystemPath(argv[Loop + 1]);
                }
                
                else if (!strcmp(argv[Loop], "-edit")) //-edit [path]
                {
                    CCEngineConfiguration.launch = CCLaunchOptionEditor;
                    CCEngineConfiguration.project = FSPathCreateFromSystemPath(argv[Loop + 1]);
                }
                
                else if (!strcmp(argv[Loop], "-new")) //-new [path]
                {
                    CCEngineConfiguration.launch = CCLaunchOptionEditor;
                    CCEngineConfiguration.project = CCProjectCreate(argv[Loop + 1]);
                }
                
                else if (Loop + 2 < argc)
                {
                    if (!strcmp(argv[Loop], "-ship")) //-ship [path] [destination]
                    {
                        exit(EXIT_SUCCESS);
                    }
                }
            }
        }
    }
    
    
    if (CCEngineConfiguration.project)
    {
        CCProjectLoad(CCEngineConfiguration.project);
    }
    
    else
    {
        CC_LOG_ERROR("Failure to initialize engine due to no project file");
        exit(EXIT_FAILURE);
    }
}
