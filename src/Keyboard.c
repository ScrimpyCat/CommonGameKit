//
//  Keyboard.c
//  Blob Game
//
//  Created by Stefan Johnson on 4/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "Keyboard.h"
#include "stdatomic.h"


typedef struct {
    int32_t keycode;
    uint32_t character;
    uint32_t flags;
    double timestamp;
    _Bool down, repeat;
} CCKeyboardMap;

static _Atomic(CCKeyboardMap) KeyList[GLFW_KEY_LAST + 1];

/*
 TODO: Unsure if we can make this guarantee across all platforms or with future minor verson of GLFW.
 It looks like it should be ok on OS X (is), X11, Wayland, Mir; but not sure about Windows. Can we guarantee that the
 input sequence of events will always be keycode, followed by character (or at least character, followed by keycode).
 Must make sure it won't possibly send in another keycode inbetween.
 */
static CCKeyboardMap TempKey;
void CCKeyboardInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods)
{
    if ((TempKey.keycode = Keycode) != GLFW_KEY_UNKNOWN)
    {
        CCAssertLog(Keycode >= 0 && Keycode <= GLFW_KEY_LAST, "Keycode is not within bounds");
        
        TempKey.flags = Mods;
        TempKey.timestamp = glfwGetTime();
        
        if (Action == GLFW_REPEAT)
        {
            TempKey.down = TRUE;
            TempKey.repeat = TRUE;
        }
        
        else
        {
            TempKey.down = Action == GLFW_PRESS;
            TempKey.repeat = FALSE;
        }
    }
    
    //call callbacks
}

void CCKeyboardCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods)
{
    if (TempKey.keycode != GLFW_KEY_UNKNOWN)
    {
        TempKey.character = Codepoint;
        atomic_store(&KeyList[TempKey.keycode], TempKey);
    }
    
    //call callbacks, make sure to check modifiers
}

void CCKeyboardStateReset(void)
{
    for (size_t Loop = 0; Loop < sizeof(KeyList) / sizeof(typeof(*KeyList)); Loop++)
    {
        atomic_store(&KeyList[Loop], ((CCKeyboardMap){ .keycode = (uint32_t)Loop }));
    }
}
