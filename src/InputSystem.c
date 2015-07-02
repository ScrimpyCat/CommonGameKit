//
//  InputSystem.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/07/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include "InputSystem.h"
#include "Window.h"


static void CCKeyInput(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods);

void CCInputSystemRegister(void)
{
    glfwSetKeyCallback(CCWindow, CCKeyInput);
}

void CCInputSystemDeregister(void)
{
    
}

static void CCKeyInput(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
{
    CC_LOG_DEBUG("%d %d %d %d", Key, Scancode, Action, Mods);
}
