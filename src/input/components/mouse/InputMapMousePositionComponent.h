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

#ifndef CommonGameKit_InputMapMousePositionComponent_h
#define CommonGameKit_InputMapMousePositionComponent_h

#include <CommonGameKit/InputMapComponent.h>

#define CC_INPUT_MAP_MOUSE_POSITION_COMPONENT_ID (CCInputMapTypeMousePosition | CC_INPUT_COMPONENT_FLAG)

extern const CCString CCInputMapMousePositionComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapComponentClass);
} CCInputMapMousePositionComponentClass, *CCInputMapMousePositionComponentPrivate;


void CCInputMapMousePositionComponentRegister(void);
void CCInputMapMousePositionComponentDeregister(void);

/*!
 * @brief Initialize the input map mouse position component.
 * @param Component The component to be initialized.
 * @param id The component ID.
 */
static inline void CCInputMapMousePositionComponentInitialize(CCComponent Component, CCComponentID id);

/*!
 * @brief Deallocate the input map mouse position component.
 * @param Component The component to be deallocated.
 */
static inline void CCInputMapMousePositionComponentDeallocate(CCComponent Component);


#pragma mark -

static inline void CCInputMapMousePositionComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapComponentInitialize(Component, id);
}

static inline void CCInputMapMousePositionComponentDeallocate(CCComponent Component)
{
    CCInputMapComponentDeallocate(Component);
}

#endif
