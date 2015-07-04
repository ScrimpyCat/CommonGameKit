//
//  Keyboard.h
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Keyboard_h
#define Blob_Game_Keyboard_h

#include "Window.h"

typedef struct {
    int32_t keycode;
    uint32_t character;
    uint32_t flags;
    double timestamp;
    _Bool down, repeat;
} CCKeyboardMap;

void CCKeyboardInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods);
void CCKeyboardCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods);
void CCKeyboardStateReset(void);

#endif
