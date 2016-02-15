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
#include "GFX.h"

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
#include "AssetManager.h"

typedef struct {
    CCVector2D position;
    CCColourRGB colour;
} DemoVertData;

#define DEMO_IBO_SIZE 20002
#define DEMO_QUAD_BATCH_SIZE ((DEMO_IBO_SIZE + 2) / 6)
#define DEMO_DEGENERATE_STRIPS(quads) (((quads) - 1) * 2)

static GFXShader DemoShader;
static GFXBuffer VertBuffer;
static GFXDraw Drawer;
static GFXBuffer IBO;
static void CCRenderSystemUpdate(double DeltaTime, CCCollection Components)
{
//    printf("draw: %f\n", DeltaTime);
    
    CCCollectionDestroy(CCComponentSystemGetAddedComponentsForSystem(CC_RENDER_SYSTEM_ID));
    CCCollectionDestroy(CCComponentSystemGetRemovedComponentsForSystem(CC_RENDER_SYSTEM_ID));
    
    const size_t Count = CCCollectionGetCount(Components);
    
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
    
    GFXBufferSetSize(VertBuffer, sizeof(DemoVertData) * 4 * Count);
    GFXBufferWriteBuffer(VertBuffer, 0, sizeof(DemoVertData) * 4 * Count, Data);
    CC_SAFE_Free(Data);
    
    
    size_t Vertices = (Count * 4) + DEMO_DEGENERATE_STRIPS(Count);
    size_t Loop = 0;
    if (Count > DEMO_QUAD_BATCH_SIZE)
    {
        const size_t Batch = (DEMO_QUAD_BATCH_SIZE * 4) + DEMO_DEGENERATE_STRIPS(DEMO_QUAD_BATCH_SIZE);
        
        for (size_t C = DEMO_QUAD_BATCH_SIZE * (Count / DEMO_QUAD_BATCH_SIZE); Loop < C; Loop += DEMO_QUAD_BATCH_SIZE)
        {
            GFXDrawSubmitIndexed(Drawer, GFXPrimitiveTypeTriangleStrip, Loop * 4, Batch);
        }
        
        Vertices = ((Count - Loop) * 4) + DEMO_DEGENERATE_STRIPS((Count - Loop));
    }
    
    if (Vertices)
    {
        GFXDrawSubmitIndexed(Drawer, GFXPrimitiveTypeTriangleStrip, Loop * 4, Vertices);
    }
}

static void CCRenderSystemLoadResources(void)
{
    GFXShaderLibrary Lib = GFXShaderLibraryCreate(CC_STD_ALLOCATOR);
    const GFXShaderSource Vert = GFXShaderLibraryCompile(Lib, GFXShaderSourceTypeVertex, "vert",
                                                         "#version 330 core\n"
                                                         "layout (location = 0) in vec4 vPosition;\n"
                                                         "layout (location = 1) in vec4 vColour;\n"
                                                         "out vec4 colour;\n"
                                                         "uniform mat4 modelViewProjectionMatrix;\n"
                                                         "void main()\n"
                                                         "{\n"
                                                         "	colour = vColour;\n"
                                                         "	gl_Position = modelViewProjectionMatrix * vPosition;\n"
                                                         "}\n");
    
    const GFXShaderSource Frag = GFXShaderLibraryCompile(Lib, GFXShaderSourceTypeFragment, "frag",
                                                         "#version 330 core\n"
                                                         "in vec4 colour;\n"
                                                         "out vec4 fragColour;\n"
                                                         "void main()\n"
                                                         "{\n"
                                                         "	fragColour = colour;\n"
                                                         "}\n");
    
    DemoShader = GFXShaderCreate(CC_STD_ALLOCATOR, Vert, Frag);
    GFXShaderLibraryDestroy(Lib);
    
    CCAssetManagerRegisterShader(CC_STRING("vertex-colour"), DemoShader);
    
    
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
    
    IBO = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataIndex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadMany, sizeof(unsigned short) * DEMO_IBO_SIZE, Indices);
    CC_SAFE_Free(Indices);
    
    VertBuffer = GFXBufferCreate(CC_STD_ALLOCATOR, GFXBufferHintDataVertex | GFXBufferHintCPUWriteOnce | GFXBufferHintGPUReadOnce, 0, NULL);
    
    Drawer = GFXDrawCreate(CC_STD_ALLOCATOR);
    GFXDrawSetShader(Drawer, DemoShader);
    GFXDrawSetFramebuffer(Drawer, GFXFramebufferDefault(), 0);
    GFXDrawSetIndexBuffer(Drawer, IBO, GFXBufferFormatUInt16);
    GFXDrawSetVertexBuffer(Drawer, "vPosition", VertBuffer, GFXBufferFormatFloat32x2, sizeof(DemoVertData), offsetof(DemoVertData, position));
    GFXDrawSetVertexBuffer(Drawer, "vColour", VertBuffer, GFXBufferFormatFloat32x3, sizeof(DemoVertData), offsetof(DemoVertData, colour));
}

static void CCRenderSystemUnloadResources(void)
{
    //TODO: Probably make a secondary context for resource loading/unloading
}
