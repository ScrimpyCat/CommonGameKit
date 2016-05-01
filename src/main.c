/*
 *  Copyright (c) 2015, Stefan Johnson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list
 *     of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice, this
 *     list of conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CC_LOG_OPTION CCLogOptionOutputAll

#include <stdio.h>
#include <stdlib.h>
#include <CommonC/Common.h>
#include <stdatomic.h>
#include <tinycthread.h>

#include "GLSetup.h"
#include "PlatformSetup.h"
#include "EngineSetup.h"
#include "ComponentSystem.h"
#include "Configuration.h"
#include "GUIManager.h"


GLFWwindow *CCWindow = NULL;


static void ErrorCallback(int Error, const char *Description)
{
    CC_LOG_ERROR("GLFW Error [%d]: %s", Error, Description);
}

static mtx_t RenderLock;
static int FramebufferWidth = 0, FramebufferHeight = 0;
void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    mtx_lock(&RenderLock);
    FramebufferWidth = Width;
    FramebufferHeight = Height;
    mtx_unlock(&RenderLock);
}

static int RenderLoop(GLFWwindow *Window)
{
    int err;
    if ((err = mtx_init(&RenderLock, mtx_plain)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create render thread lock (%d)", err);
    }
    
    glfwMakeContextCurrent(Window);
    
    while (!glfwWindowShouldClose(Window))
    {
        mtx_lock(&RenderLock);
        CC_GL_VIEWPORT(0, 0, FramebufferWidth, FramebufferHeight);
        CC_GL_ENABLE(GL_FRAMEBUFFER_SRGB);
        
        CC_GL_CLEAR_COLOR(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); CC_GL_CHECK();
        
        CCComponentSystemRun(CCComponentSystemExecutionTypeRender);
        
        GUIManagerLock();
        GUIManagerUpdate();
        GUIManagerRender(GFXFramebufferDefault(), 0);
        GUIManagerUnlock();
        
        glfwSwapBuffers(Window);
        mtx_unlock(&RenderLock);
    }
    
    return EXIT_SUCCESS;
}
#include "RenderSystem.h"
#include "RenderComponent.h"
#include "InputSystem.h"
#include "EntityManager.h"

int main(int argc, const char *argv[])
{
    CCEnginePreSetup();
    CCConfigureOptions(argc, argv);
    
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
    
    glfwWindowHint(GLFW_RED_BITS, 32);
    glfwWindowHint(GLFW_GREEN_BITS, 32);
    glfwWindowHint(GLFW_BLUE_BITS, 32);
    glfwWindowHint(GLFW_ALPHA_BITS, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);
    
    CCWindow = glfwCreateWindow(CCEngineConfiguration.window.width, CCEngineConfiguration.window.height, CCEngineConfiguration.title, CCEngineConfiguration.window.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (!CCWindow)
    {
        //TODO: Fallback to legacy GL profile? 
        CC_LOG_ERROR("Failed to create window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    
    glfwSetFramebufferSizeCallback(CCWindow, FramebufferSizeCallback);
    glfwGetFramebufferSize(CCWindow, &FramebufferWidth, &FramebufferHeight);
    
    glfwMakeContextCurrent(CCWindow);
    CCPlatformWindowSetup(CCWindow);
    
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
    
    CCEngineSetup();
    
    glfwMakeContextCurrent(NULL);
    
    int err;
    thrd_t RenderThread;
    if ((err = thrd_create(&RenderThread, (thrd_start_t)RenderLoop, CCWindow)) != thrd_success)
    {
        //Possibly fallback to single threaded implementation?
        CC_LOG_ERROR("Failed to create render thread (%d)", err);
        
        glfwDestroyWindow(CCWindow);
        glfwTerminate();
        
        return EXIT_FAILURE;
    }
    
    while (!glfwWindowShouldClose(CCWindow))
    {
        CCEntityManagerLock();
        CCEntityManagerUpdate();
        
        CCEnumerator Enumerator;
        CCCollectionGetEnumerator(CCEntityManagerGetEntities(), &Enumerator);
        
        for (CCEntity *Entity = CCCollectionEnumeratorGetCurrent(&Enumerator); Entity; Entity = CCCollectionEnumeratorNext(&Enumerator))
        {
//            CCVector2D Move = CCInputSystemGetPressure2ForAction(*Entity, "move");
//            CC_LOG_DEBUG("%f, %f", Move.x, Move.y);
            
            if (CCInputSystemGetStateForAction(*Entity, "recolour") == CCInputStateActive)
            {
                CCComponentSystemLock(CC_RENDER_SYSTEM_ID);
                CCEnumerator Enumerator;
                CCCollectionGetEnumerator(CCComponentSystemGetComponentsForSystem(CC_RENDER_SYSTEM_ID), &Enumerator);
                
                for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
                {
                    CCRenderComponentSetColour(*Component, CCVector3DMake(CCRandomRangef(0.0f, 1.0f), CCRandomRangef(0.0f, 1.0f), CCRandomRangef(0.0f, 1.0f)));
                }
                CCComponentSystemUnlock(CC_RENDER_SYSTEM_ID);
            }
        }
        
        CCEntityManagerUnlock();
        
        
        CCComponentSystemRun(CCComponentSystemExecutionTypeInput);
        glfwWaitEvents();
    }
    
    
    thrd_detach(RenderThread);
    
    glfwDestroyWindow(CCWindow);
    glfwTerminate();
    
    return EXIT_SUCCESS;
}
