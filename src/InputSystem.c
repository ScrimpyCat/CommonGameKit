//
//  InputSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputSystem.h"
#include "Window.h"
#include "Keyboard.h"


static void CCWindowFocus(GLFWwindow *Window, int Focus);

void CCInputSystemRegister(void)
{
    glfwSetWindowFocusCallback(CCWindow, CCWindowFocus);
    glfwSetKeyCallback(CCWindow, CCKeyboardInput);
    glfwSetCharModsCallback(CCWindow, CCKeyboardCharInput);
}

void CCInputSystemDeregister(void)
{
    
}

static void CCWindowFocus(GLFWwindow *Window, int Focus)
{
    if (!Focus)
    {
        CCKeyboardStateReset();
    }
}
