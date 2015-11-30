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
        _Bool fullscreen;
    } window;
    struct {
        CCCollection fonts;
        CCCollection levels;
        CCCollection rules;
        CCCollection textures;
        CCCollection shaders;
        CCCollection sounds;
        CCCollection layouts;
    } directory;
    /*
     Blob\ Assets/:
     blob.gamepkg
     /fonts/ (font sets)
     /levels/ (scenes)
     /audio/ (ui and game music/sounds)
     /graphics/ (ui and game art)
     /ui/ (ui layout files)
     /logs/ (uncommited, generated)
     /tmp/ (uncommited, generated)

     
     (game "title"
         (dir-fonts "font") ; (fontsets "font/debug.fnt" "font/style.fnt")
         (default-font "debug" 12)
         
         (dir-rules "logic/rules") ;so folders can be moved about and only gamepkg needs to be updated
         (dir-textures "art/gui" "art/game")
         ;(dir-levels)
         
         
         (default-window-size 720 1020)
         (default-fullscreen false)
         (config-file "")
     )
     */
} CCEngineConfig;


extern CCEngineConfig CCEngineConfiguration;
void CCConfigureOptions(int argc, const char *argv[]);

#endif
