//
//  Configuration.h
//  Blob Game
//
//  Created by Stefan Johnson on 22/11/2015.
//  Copyright © 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Configuration_h
#define Blob_Game_Configuration_h

#include <CommonC/Common.h>

typedef enum {
    CCLaunchOptionGame,
    CCLaunchOptionEditor
} CCLaunchOption;

typedef struct {
    CCLaunchOption launch;
    FSPath project;
    char *title;
    struct {
        int width;
        int height;
        _Bool fullscreen; //TODO: Change to specifying monitors
    } window;
    struct {
        CCCollection fonts;
        CCCollection levels;
        CCCollection rules;
        CCCollection entities;
        CCCollection textures;
        CCCollection shaders;
        CCCollection sounds;
        CCCollection layouts;
        FSPath logs;
        FSPath tmp;
    } directory;
} CCEngineConfig;


extern CCEngineConfig CCEngineConfiguration;
void CCConfigureOptions(int argc, const char *argv[]);

#endif
