/*
 *  Copyright (c) 2018, Stefan Johnson
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

#ifndef CommonGameKit_RenderCameraComponent_h
#define CommonGameKit_RenderCameraComponent_h

#include <CommonGameKit/RenderSystem.h>
#include <CommonGameKit/ComponentBase.h>
#include <CommonGameKit/GFXViewport.h>
#include <CommonC/Common.h>


#define CC_RENDER_CAMERA_COMPONENT_ID (CCRenderTypeCamera | CC_RENDER_COMPONENT_FLAG)

extern const CCString CCRenderCameraComponentName;

typedef _Bool (*CCRenderCameraFilter)(CCComponentID id);

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCMatrix4 projection;
    CCColourRGBA background;
    GFXViewport viewport;
    CCRenderCameraFilter filter;
} CCRenderCameraComponentClass, *CCRenderCameraComponentPrivate;


void CCRenderCameraComponentRegister(void);
void CCRenderCameraComponentDeregister(void);

/*!
 * @brief Initialize the camera component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCRenderCameraComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the camera component.
 * @param Component The component to be deallocated.
 */
static inline void CCRenderCameraComponentDeallocate(CCComponent Component);


#pragma mark -

static inline void CCRenderCameraComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCRenderCameraComponentPrivate)Component)->projection = CCMatrix4Identity;
    ((CCRenderCameraComponentPrivate)Component)->background = CCVector4DFill(0.0f);
    ((CCRenderCameraComponentPrivate)Component)->viewport = GFXViewport3D(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    ((CCRenderCameraComponentPrivate)Component)->filter = NULL;
}

static inline void CCRenderCameraComponentDeallocate(CCComponent Component)
{
    CCComponentDeallocate(Component);
}

#endif
