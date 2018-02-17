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

#ifndef CommonGameKit_InputMapControllerAxesComponent_h
#define CommonGameKit_InputMapControllerAxesComponent_h

#include <CommonGameKit/InputMapControllerComponent.h>

#define CC_INPUT_MAP_CONTROLLER_AXES_COMPONENT_ID (CCInputMapTypeControllerAxes | CC_INPUT_COMPONENT_FLAG)

extern const CCString CCInputMapControllerAxesComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCInputMapControllerComponentClass);
    float deadzone;
    uint8_t resolution;
    int32_t x, y, z;
    _Bool oneSidedX, oneSidedY, oneSidedZ;
    _Bool flipX, flipY, flipZ;
} CCInputMapControllerAxesComponentClass, *CCInputMapControllerAxesComponentPrivate;


void CCInputMapControllerAxesComponentRegister(void);
void CCInputMapControllerAxesComponentDeregister(void);
void CCInputMapControllerAxesComponentDeserializer(CCComponent Component, CCExpression Arg);


static inline void CCInputMapControllerAxesComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCInputMapControllerComponentInitialize(Component, id);
    ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone = 0.0f;
    ((CCInputMapControllerAxesComponentPrivate)Component)->resolution = 0;
    ((CCInputMapControllerAxesComponentPrivate)Component)->x = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->y = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->z = -1;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipX = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipY = FALSE;
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ = FALSE;
}

static inline void CCInputMapControllerAxesComponentDeallocate(CCComponent Component)
{
    CCInputMapControllerComponentDeallocate(Component);
}

static inline float CCInputMapControllerAxesComponentGetDeadzone(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone;
}

static inline void CCInputMapControllerAxesComponentSetDeadzone(CCComponent Component, float Deadzone)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->deadzone = Deadzone;
}

static inline uint8_t CCInputMapControllerAxesComponentGetResolution(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->resolution;
}

static inline void CCInputMapControllerAxesComponentSetResolution(CCComponent Component, uint8_t Resolution)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->resolution = Resolution;
}

static inline int32_t CCInputMapControllerAxesComponentGetX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->x;
}

static inline void CCInputMapControllerAxesComponentSetX(CCComponent Component, int32_t x)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->x = x;
}

static inline int32_t CCInputMapControllerAxesComponentGetY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->y;
}

static inline void CCInputMapControllerAxesComponentSetY(CCComponent Component, int32_t y)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->y = y;
}

static inline int32_t CCInputMapControllerAxesComponentGetZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->z;
}

static inline void CCInputMapControllerAxesComponentSetZ(CCComponent Component, int32_t z)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->z = z;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedX(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedX = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedY(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedY = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetOneSidedZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ;
}

static inline void CCInputMapControllerAxesComponentSetOneSidedZ(CCComponent Component, _Bool OneSided)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->oneSidedZ = OneSided;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipX(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipX;
}

static inline void CCInputMapControllerAxesComponentSetFlipX(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipX = Flip;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipY(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipY;
}

static inline void CCInputMapControllerAxesComponentSetFlipY(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipY = Flip;
}

static inline _Bool CCInputMapControllerAxesComponentGetFlipZ(CCComponent Component)
{
    return ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ;
}

static inline void CCInputMapControllerAxesComponentSetFlipZ(CCComponent Component, _Bool Flip)
{
    ((CCInputMapControllerAxesComponentPrivate)Component)->flipZ = Flip;
}

#endif
