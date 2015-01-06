//
//  PlatformSetup_win.c
//  Blob Game
//
//  Created by Stefan Johnson on 5/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#include "PlatformSetup.h"
#include <GLFW/glfw3native.h>
#include <stdio.h>

void PlatformSetup(void)
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
}

void PlatformWindowSetup(GLFWwindow *Window)
{
    
}
