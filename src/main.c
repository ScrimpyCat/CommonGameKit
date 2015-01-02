//
//  main.c
//  Blob Game
//
//  Created by Stefan Johnson on 3/01/2015.
//  Copyright (c) 2015 Stefan Johnson. All rights reserved.
//

#include <stdio.h>
#include <CommonC/Common.h>
#include <GLFW/glfw3.h>
#include <stdatomic.h>
#include <tinycthread.h>

static void ErrorCallback(int Error, const char *Description)
{
    CC_LOG_ERROR("GLFW Error [%d]: %s", Error, Description);
}

static int RenderLoop(GLFWwindow *Window)
{
    glfwMakeContextCurrent(Window);
    
    while (!glfwWindowShouldClose(Window))
    {
        glfwSwapBuffers(Window);
    }
    
    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[])
{
    glfwSetErrorCallback(ErrorCallback);
    
    if (!glfwInit())
    {
        CC_LOG_ERROR("Failed to initalized GLFW");
        return EXIT_FAILURE;
    }
    
    
    GLFWwindow *Window = glfwCreateWindow(640, 480, "Blob Game", NULL, NULL);
    if (!Window)
    {
        CC_LOG_ERROR("Failed to create window");
        glfwTerminate();
        return EXIT_FAILURE;
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
