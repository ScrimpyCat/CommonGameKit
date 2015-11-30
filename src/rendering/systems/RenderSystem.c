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

#include "RenderSystem.h"
#include "tinycthread.h"
#include <CommonC/Common.h>
#include "GLSetup.h"

static _Bool CCRenderSystemTryLock(void);
static void CCRenderSystemLock(void);
static void CCRenderSystemUnlock(void);
static _Bool CCRenderSystemHandlesComponent(CCComponentID id);
static void CCRenderSystemUpdate(double DeltaTime, CCCollection Components);
static void CCRenderSystemLoadResources(void);
static void CCRenderSystemUnloadResources(void);

static mtx_t Lock;
void CCRenderSystemRegister(void)
{
    int err;
    if ((err = mtx_init(&Lock, mtx_plain | mtx_recursive)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to create render system lock (%d)", err);
        exit(EXIT_FAILURE); //TODO: How should we handle this?
    }
    
    CCRenderSystemLoadResources();
    
    CCComponentSystemRegister(CC_RENDER_SYSTEM_ID, CCComponentSystemExecutionTypeRender, (CCComponentSystemUpdateCallback)CCRenderSystemUpdate, CCRenderSystemHandlesComponent, NULL, NULL, CCRenderSystemTryLock, CCRenderSystemLock, CCRenderSystemUnlock);
}

void CCRenderSystemDeregister(void)
{
    mtx_destroy(&Lock);
    
    CCComponentSystemDeregister(CC_RENDER_SYSTEM_ID, CCComponentSystemExecutionTypeRender);
    
    CCRenderSystemUnloadResources();
}

static _Bool CCRenderSystemTryLock(void)
{
    int err = mtx_trylock(&Lock);
    if ((err != thrd_success) && (err != thrd_busy))
    {
        CC_LOG_ERROR("Failed to lock render system (%d)", err);
    }
    
    return err == thrd_success;
}

static void CCRenderSystemLock(void)
{
    int err;
    if ((err = mtx_lock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to lock render system (%d)", err);
    }
}

static void CCRenderSystemUnlock(void)
{
    int err;
    if ((err = mtx_unlock(&Lock)) != thrd_success)
    {
        CC_LOG_ERROR("Failed to unlock render system (%d)", err);
    }
}

static _Bool CCRenderSystemHandlesComponent(CCComponentID id)
{
    return (id & 0x7f000000) == CC_RENDER_COMPONENT_FLAG;
}

#warning demo rendering behaviour (this will be changed later)
#include "RenderComponent.h"

typedef struct {
    CCVector2D position;
    CCColourRGB colour;
} DemoVertData;

#define DEMO_IBO_SIZE 20002
#define DEMO_QUAD_BATCH_SIZE ((DEMO_IBO_SIZE + 2) / 6)
#define DEMO_DEGENERATE_STRIPS(quads) (((quads) - 1) * 2)

static GLuint DemoShader;
static GLuint VertBuffer;
static GLuint VAO;
static GLuint IBO;
static void CCRenderSystemUpdate(double DeltaTime, CCCollection Components)
{
//    printf("draw: %f\n", DeltaTime);
    
    
    glUseProgram(DemoShader);
    glBindVertexArray(VAO);
    
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(CC_RENDER_SYSTEM_ID));
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(CC_RENDER_SYSTEM_ID));
    
    const GLsizei Count = (GLsizei)CCCollectionGetCount(Components);
    
    DemoVertData *Data;
    CC_SAFE_Malloc(Data, sizeof(DemoVertData) * 4 * Count);
    
    CCEnumerator Enumerator;
    CCCollectionGetEnumerator(Components, &Enumerator);
    
    size_t Index = 0;
    for (CCComponent *Component = CCCollectionEnumeratorGetCurrent(&Enumerator); Component; Component = CCCollectionEnumeratorNext(&Enumerator))
    {
        if (CCComponentGetID(*Component) == CC_RENDER_COMPONENT_ID)
        {
            CCColourRGB Colour = CCRenderComponentGetColour(*Component);
            CCRect Rect = CCRenderComponentGetRect(*Component);
            
            Data[Index * 4].position = Rect.position;
            Data[Index * 4].colour = Colour;
            Data[(Index * 4) + 1].position = CCVector2Add(Rect.position, CCVector2DMake(Rect.size.x, 0.0f));
            Data[(Index * 4) + 1].colour = Colour;
            Data[(Index * 4) + 2].position = CCVector2Add(Rect.position, CCVector2DMake(0.0f, Rect.size.y));
            Data[(Index * 4) + 2].colour = Colour;
            Data[(Index * 4) + 3].position = CCVector2Add(Rect.position, Rect.size);
            Data[(Index * 4) + 3].colour = Colour;
            
            Index++;
        }
    }
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(DemoVertData) * 4 * Count, Data, GL_DYNAMIC_DRAW);
    CC_SAFE_Free(Data);
    
    
    GLsizei Vertices = (Count * 4) + DEMO_DEGENERATE_STRIPS(Count);
    GLint Loop = 0;
    if (Count > DEMO_QUAD_BATCH_SIZE)
    {
        const GLsizei Batch = (DEMO_QUAD_BATCH_SIZE * 4) + DEMO_DEGENERATE_STRIPS(DEMO_QUAD_BATCH_SIZE);
        
        for (GLsizei C = DEMO_QUAD_BATCH_SIZE * (Count / DEMO_QUAD_BATCH_SIZE); Loop < C; Loop += DEMO_QUAD_BATCH_SIZE)
        {
            glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, Batch, GL_UNSIGNED_SHORT, NULL, Loop * 4);
        }
        
        Vertices = ((Count - Loop) * 4) + DEMO_DEGENERATE_STRIPS((Count - Loop));
    }
    
    if (Vertices)
    {
        glDrawElementsBaseVertex(GL_TRIANGLE_STRIP, Vertices, GL_UNSIGNED_SHORT, NULL, Loop);
    }
}

static void CCRenderSystemLoadResources(void)
{
    GLuint Vert = glCreateShader(GL_VERTEX_SHADER), Frag = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(Vert, 1, (const GLchar*[]){
        "#version 330 core\n"
        "layout (location = 0) in vec4 vPosition;\n"
        "layout (location = 1) in vec4 vColour;\n"
        "out vec4 colour;\n"
        "uniform mat4 modelViewProjectionMatrix;\n"
        "void main()\n"
        "{\n"
        "	colour = vColour;\n"
        "	gl_Position = /*modelViewProjectionMatrix * */vPosition;\n"
        "}\n"
    }, NULL);
    
    glShaderSource(Frag, 1, (const GLchar*[]){
        "#version 330 core\n"
        "in vec4 colour;\n"
        "out vec4 fragColour;\n"
        "void main()\n"
        "{\n"
        "	fragColour = colour;\n"
        "}\n"
    }, NULL);
    
    glCompileShader(Vert);
    glCompileShader(Frag);
    
    
    GLint Compiled;
    glGetShaderiv(Vert, GL_COMPILE_STATUS, &Compiled);
    
    if (!Compiled)
    {
        char ErrorLog[1024];
        glGetShaderInfoLog(Vert, sizeof(ErrorLog), NULL, ErrorLog);
        
        CC_LOG_ERROR("Shader compilation failed:\n%s", ErrorLog);
        exit(EXIT_FAILURE);
    }
    
    
    glGetShaderiv(Frag, GL_COMPILE_STATUS, &Compiled);
    
    if (!Compiled)
    {
        char ErrorLog[1024];
        glGetShaderInfoLog(Frag, sizeof(ErrorLog), NULL, ErrorLog);
        
        CC_LOG_ERROR("Shader compilation failed:\n%s", ErrorLog);
        exit(EXIT_FAILURE);
    }
    
    
    
    DemoShader = glCreateProgram();
    glAttachShader(DemoShader, Vert);
    glAttachShader(DemoShader, Frag);
    glBindFragDataLocation(DemoShader, 0, "fragColour");
    glLinkProgram(DemoShader);
    
    GLint Linked;
    glGetProgramiv(DemoShader, GL_LINK_STATUS, (GLint*)&Linked);
    
    if (!Linked)
    {
        char ErrorLog[1024];
        glGetProgramInfoLog(DemoShader, sizeof(ErrorLog), NULL, ErrorLog);
        
        CC_LOG_ERROR("Shader linking failed:\n%s", ErrorLog);
        exit(EXIT_FAILURE);
    }
    
    
    
    unsigned short *Indices;
    CC_SAFE_Malloc(Indices, sizeof(unsigned short) * DEMO_IBO_SIZE,
                   CC_LOG_ERROR("Failed to allocate IBO");
                   exit(EXIT_FAILURE);
                   );
    
    
    for (unsigned short Loop = 0, Loop2 = 0; Loop < DEMO_IBO_SIZE; )
    {
        Indices[Loop++] = Loop2++;
        Indices[Loop++] = Loop2++;
        Indices[Loop++] = Loop2++;
        Indices[Loop++] = Loop2;
        
        if (Loop < DEMO_IBO_SIZE - 4)
        {
            Indices[Loop++] = Loop2++;
            Indices[Loop++] = Loop2;
        }
    }
    
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * DEMO_IBO_SIZE, Indices, GL_STATIC_DRAW);
    CC_SAFE_Free(Indices);
    
    glGenBuffers(1, &VertBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, VertBuffer);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(DemoVertData), (void*)offsetof(DemoVertData, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(DemoVertData), (void*)offsetof(DemoVertData, colour));
    
    glBindVertexArray(0);
}

static void CCRenderSystemUnloadResources(void)
{
    //TODO: Probably make a secondary context for resource loading/unloading
}
