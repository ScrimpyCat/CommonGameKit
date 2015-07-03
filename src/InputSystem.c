//
//  InputSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputSystem.h"
#include "Window.h"
#include "stdatomic.h"


typedef struct {
    int32_t keycode;
    uint32_t character;
    uint32_t flags;
    double timestamp;
    _Bool down, repeat;
} CCKeyMap;

static _Atomic(CCKeyMap) CCKeyList[GLFW_KEY_LAST + 1];

static void CCWindowFocus(GLFWwindow *Window, int Focus);
static void CCKeyInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods);
static void CCKeyCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods);


void CCInputSystemRegister(void)
{
    glfwSetWindowFocusCallback(CCWindow, CCWindowFocus);
    glfwSetKeyCallback(CCWindow, CCKeyInput);
    glfwSetCharModsCallback(CCWindow, CCKeyCharInput);
}

void CCInputSystemDeregister(void)
{
    
}

static void CCWindowFocus(GLFWwindow *Window, int Focus)
{
    if (!Focus)
    {
        for (size_t Loop = 0; Loop < sizeof(CCKeyList) / sizeof(typeof(*CCKeyList)); Loop++)
        {
            atomic_store(&CCKeyList[Loop], ((CCKeyMap){ .keycode = (uint32_t)Loop }));
        }
    }
}

/*
 TODO: Unsure if we can make this guarantee across all platforms or with future minor verson of GLFW.
 It looks like it should be ok on OS X (is), X11, Wayland, Mir; but not sure about Windows. Can we guarantee that the
 input sequence of events will always be keycode, followed by character (or at least character, followed by keycode).
 Must make sure it won't possibly send in another keycode inbetween.
 */
static CCKeyMap TempKey;
static void CCKeyInput(GLFWwindow *Window, int Keycode, int Scancode, int Action, int Mods)
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

static void CCKeyCharInput(GLFWwindow *Window, unsigned int Codepoint, int Mods)
{
    if (TempKey.keycode != GLFW_KEY_UNKNOWN)
    {
        TempKey.character = Codepoint;
        atomic_store(&CCKeyList[TempKey.keycode], TempKey);
    }
    
    //call callbacks, make sure to check modifiers
}
