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

#include "Configuration.h"
#include <stdlib.h>
#include <string.h>
#include "Project.h"


CCEngineConfig CCEngineConfiguration = {
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

void CCConfigureOptions(int argc, const char *argv[])
{
    for (int Loop = 0; Loop < argc; Loop++)
    {
        if (argv[Loop][0] == '-')
        {
            if (!strcmp(argv[Loop], "-test"))
            {
                CCEngineConfiguration.launch = CCLaunchOptionTest;
                CCEngineConfiguration.title = "Test";
                CCEngineConfiguration.window.width = 640;
                CCEngineConfiguration.window.height = 480;
                return;
            }
            
            else if (Loop + 1 < argc)
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
