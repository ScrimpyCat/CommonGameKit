//
//  Keyboard.h
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#ifndef Blob_Game_Keyboard_h
#define Blob_Game_Keyboard_h

#include "GLSetup.h"
#include "Component.h"

typedef struct {
    double timestamp;
    _Bool down, repeat;
} CCKeyboardState;

typedef struct {
    int32_t keycode;
    uint32_t character;
    uint32_t flags;
    CCKeyboardState state;
} CCKeyboardMap;

void CCKeyboardInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods);
void CCKeyboardCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods);
void CCKeyboardStateReset(void);
CCKeyboardState CCKeyboardGetStateForComponent(CCComponent Component);

#endif
