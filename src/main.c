//
//  main.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#define CC_LOG_OPTION CCLogOptionOutputAll

#include <stdio.h>
#include <stdlib.h>
#include <CommonC/Common.h>
#include <stdatomic.h>
#include <tinycthread.h>

#include "GLSetup.h"
#include "PlatformSetup.h"


static void ErrorCallback(int Error, const char *Description)
{
    CC_LOG_ERROR("GLFW Error [%d]: %s", Error, Description);
}

static int RenderLoop(GLFWwindow *Window)
{
    glfwMakeContextCurrent(Window);
    
    while (!glfwWindowShouldClose(Window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(Window);
    }
    
    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[])
{
    CCPlatformSetup();
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit())
    {
        CC_LOG_ERROR("Failed to initalized GLFW");
        return EXIT_FAILURE;
    }
    
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
    
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    
    GLFWwindow *Window = glfwCreateWindow(640, 480, "Blob Game", NULL, NULL);
    if (!Window)
    {
        //TODO: Fallback to legacy GL profile? 
        CC_LOG_ERROR("Failed to create window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    
    CCPlatformWindowSetup(Window);
    
    CC_LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
    CC_LOG_INFO("Renderer: %s", glGetString(GL_RENDERER));
    CC_LOG_INFO("OpenGL Version: %s", glGetString(GL_VERSION));
    CC_LOG_INFO("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    GLint ExtCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ExtCount);
    for (GLint Loop = 0; Loop < ExtCount; Loop++)
    {
        CC_LOG_INFO("Extensions: %s", glGetStringi(GL_EXTENSIONS, Loop));
    }
    
    
    int err;
    thrd_t RenderThread;
    if ((err = thrd_create(&RenderThread, (thrd_start_t)RenderLoop, Window)) != thrd_success)
    {
        //Possibly fallback to single threaded implementation?
        CC_LOG_ERROR("Failed to create render thread (%d)", err);
        
        glfwDestroyWindow(Window);
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    while (!glfwWindowShouldClose(Window))
    {
        glfwWaitEvents();
    }
    
    
    thrd_detach(RenderThread);
    
    glfwDestroyWindow(Window);
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
