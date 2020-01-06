/*
 *  Copyright (c) 2020, Stefan Johnson
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

#define CC_QUICK_COMPILE
#include "TypeCallbacks.h"
#include "Entity.h"
#include "Component.h"
#include "GUIObject.h"
#include "PixelData.h"
#include "GFX.h"
#include "Expression.h"
#include "Font.h"
#include "Text.h"

#pragma mark Destructors

static void CCEntityContainerElementDestructor(void *Container, CCEntity *Element);
static void CCComponentContainerElementDestructor(void *Container, CCComponent *Element);
static void GUIObjectContainerElementDestructor(void *Container, GUIObject *Element);
static void CCPixelDataContainerElementDestructor(void *Container, CCPixelData *Element);
static void GFXBlitContainerElementDestructor(void *Container, GFXBlit *Element);
static void GFXBufferContainerElementDestructor(void *Container, GFXBuffer *Element);
static void GFXDrawContainerElementDestructor(void *Container, GFXDraw *Element);
static void GFXFramebufferContainerElementDestructor(void *Container, GFXFramebuffer *Element);
static void GFXShaderContainerElementDestructor(void *Container, GFXShader *Element);
static void GFXShaderLibraryContainerElementDestructor(void *Container, GFXShaderLibrary *Element);
static void GFXTextureStreamContainerElementDestructor(void *Container, GFXTextureStream *Element);
static void GFXTextureContainerElementDestructor(void *Container, GFXTexture *Element);
static void GFXCommandBufferContainerElementDestructor(void *Container, GFXCommandBuffer *Element);
static void CCExpressionContainerElementDestructor(void *Container, CCExpression *Element);
static void CCFontContainerElementDestructor(void *Container, CCFont *Element);
static void CCTextContainerElementDestructor(void *Container, CCText *Element);

#pragma mark - Collection Callbacks
#pragma mark Destructors

const CCCollectionElementDestructor CCEntityDestructorForCollection = (CCCollectionElementDestructor)CCEntityContainerElementDestructor;
const CCCollectionElementDestructor CCComponentDestructorForCollection = (CCCollectionElementDestructor)CCComponentContainerElementDestructor;
const CCCollectionElementDestructor GUIObjectDestructorForCollection = (CCCollectionElementDestructor)GUIObjectContainerElementDestructor;
const CCCollectionElementDestructor CCPixelDataDestructorForCollection = (CCCollectionElementDestructor)CCPixelDataContainerElementDestructor;
const CCCollectionElementDestructor GFXBlitDestructorForCollection = (CCCollectionElementDestructor)GFXBlitContainerElementDestructor;
const CCCollectionElementDestructor GFXBufferDestructorForCollection = (CCCollectionElementDestructor)GFXBufferContainerElementDestructor;
const CCCollectionElementDestructor GFXDrawDestructorForCollection = (CCCollectionElementDestructor)GFXDrawContainerElementDestructor;
const CCCollectionElementDestructor GFXFramebufferDestructorForCollection = (CCCollectionElementDestructor)GFXFramebufferContainerElementDestructor;
const CCCollectionElementDestructor GFXShaderDestructorForCollection = (CCCollectionElementDestructor)GFXShaderContainerElementDestructor;
const CCCollectionElementDestructor GFXShaderLibraryDestructorForCollection = (CCCollectionElementDestructor)GFXShaderLibraryContainerElementDestructor;
const CCCollectionElementDestructor GFXTextureStreamDestructorForCollection = (CCCollectionElementDestructor)GFXTextureStreamContainerElementDestructor;
const CCCollectionElementDestructor GFXTextureDestructorForCollection = (CCCollectionElementDestructor)GFXTextureContainerElementDestructor;
const CCCollectionElementDestructor GFXCommandBufferDestructorForCollection = (CCCollectionElementDestructor)GFXCommandBufferContainerElementDestructor;
const CCCollectionElementDestructor CCExpressionDestructorForCollection = (CCCollectionElementDestructor)CCExpressionContainerElementDestructor;
const CCCollectionElementDestructor CCFontDestructorForCollection = (CCCollectionElementDestructor)CCFontContainerElementDestructor;
const CCCollectionElementDestructor CCTextDestructorForCollection = (CCCollectionElementDestructor)CCTextContainerElementDestructor;

#pragma mark - Dictionary Callbacks
#pragma mark Destructors

const CCDictionaryElementDestructor CCEntityDestructorForDictionary = (CCDictionaryElementDestructor)CCEntityContainerElementDestructor;
const CCDictionaryElementDestructor CCComponentDestructorForDictionary = (CCDictionaryElementDestructor)CCComponentContainerElementDestructor;
const CCDictionaryElementDestructor GUIObjectDestructorForDictionary = (CCDictionaryElementDestructor)GUIObjectContainerElementDestructor;
const CCDictionaryElementDestructor CCPixelDataDestructorForDictionary = (CCDictionaryElementDestructor)CCPixelDataContainerElementDestructor;
const CCDictionaryElementDestructor GFXBlitDestructorForDictionary = (CCDictionaryElementDestructor)GFXBlitContainerElementDestructor;
const CCDictionaryElementDestructor GFXBufferDestructorForDictionary = (CCDictionaryElementDestructor)GFXBufferContainerElementDestructor;
const CCDictionaryElementDestructor GFXDrawDestructorForDictionary = (CCDictionaryElementDestructor)GFXDrawContainerElementDestructor;
const CCDictionaryElementDestructor GFXFramebufferDestructorForDictionary = (CCDictionaryElementDestructor)GFXFramebufferContainerElementDestructor;
const CCDictionaryElementDestructor GFXShaderDestructorForDictionary = (CCDictionaryElementDestructor)GFXShaderContainerElementDestructor;
const CCDictionaryElementDestructor GFXShaderLibraryDestructorForDictionary = (CCDictionaryElementDestructor)GFXShaderLibraryContainerElementDestructor;
const CCDictionaryElementDestructor GFXTextureStreamDestructorForDictionary = (CCDictionaryElementDestructor)GFXTextureStreamContainerElementDestructor;
const CCDictionaryElementDestructor GFXTextureDestructorForDictionary = (CCDictionaryElementDestructor)GFXTextureContainerElementDestructor;
const CCDictionaryElementDestructor GFXCommandBufferDestructorForDictionary = (CCDictionaryElementDestructor)GFXCommandBufferContainerElementDestructor;
const CCDictionaryElementDestructor CCExpressionDestructorForDictionary = (CCDictionaryElementDestructor)CCExpressionContainerElementDestructor;
const CCDictionaryElementDestructor CCFontDestructorForDictionary = (CCDictionaryElementDestructor)CCFontContainerElementDestructor;
const CCDictionaryElementDestructor CCTextDestructorForDictionary = (CCDictionaryElementDestructor)CCTextContainerElementDestructor;

#pragma mark -

static void CCEntityContainerElementDestructor(void *Container, CCEntity *Element)
{
    CCEntityDestroy(*Element);
}

static void CCComponentContainerElementDestructor(void *Container, CCComponent *Element)
{
    CCComponentDestroy(*Element);
}

static void GUIObjectContainerElementDestructor(void *Container, GUIObject *Element)
{
    GUIObjectDestroy(*Element);
}

static void CCPixelDataContainerElementDestructor(void *Container, CCPixelData *Element)
{
    CCPixelDataDestroy(*Element);
}

static void GFXBlitContainerElementDestructor(void *Container, GFXBlit *Element)
{
    GFXBlitDestroy(*Element);
}

static void GFXBufferContainerElementDestructor(void *Container, GFXBuffer *Element)
{
    GFXBufferDestroy(*Element);
}

static void GFXDrawContainerElementDestructor(void *Container, GFXDraw *Element)
{
    GFXDrawDestroy(*Element);
}

static void GFXFramebufferContainerElementDestructor(void *Container, GFXFramebuffer *Element)
{
    GFXFramebufferDestroy(*Element);
}

static void GFXShaderContainerElementDestructor(void *Container, GFXShader *Element)
{
    GFXShaderDestroy(*Element);
}

static void GFXShaderLibraryContainerElementDestructor(void *Container, GFXShaderLibrary *Element)
{
    GFXShaderLibraryDestroy(*Element);
}

static void GFXTextureStreamContainerElementDestructor(void *Container, GFXTextureStream *Element)
{
    GFXTextureStreamDestroy(*Element);
}

static void GFXTextureContainerElementDestructor(void *Container, GFXTexture *Element)
{
    GFXTextureDestroy(*Element);
}

static void GFXCommandBufferContainerElementDestructor(void *Container, GFXCommandBuffer *Element)
{
    GFXCommandBufferDestroy(*Element);
}

static void CCExpressionContainerElementDestructor(void *Container, CCExpression *Element)
{
    CCExpressionDestroy(*Element);
}

static void CCFontContainerElementDestructor(void *Container, CCFont *Element)
{
    CCFontDestroy(*Element);
}

static void CCTextContainerElementDestructor(void *Container, CCText *Element)
{
    CCTextDestroy(*Element);
}
