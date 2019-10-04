/*
 *  Copyright (c) 2017, Stefan Johnson
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

#ifndef CommonGameKit_SpatialTransformComponent_h
#define CommonGameKit_SpatialTransformComponent_h

#include <CommonGameKit/Base.h>
#include <CommonGameKit/ComponentBase.h>


#define CC_SPATIAL_TRANSFORM_COMPONENT_ID 1

extern const CCString CCSpatialTransformComponentName;

typedef struct {
    CC_COMPONENT_INHERIT(CCComponentClass);
    CCVector3D position;
    CCVector3D rotation;
    CCVector3D scale;
} CCSpatialTransformComponentClass, *CCSpatialTransformComponentPrivate;


void CCSpatialTransformComponentRegister(void);
void CCSpatialTransformComponentDeregister(void);


static inline void CCSpatialTransformComponentInitialize(CCComponent Component, CCComponentID id)
{
    CCComponentInitialize(Component, id);
    ((CCSpatialTransformComponentPrivate)Component)->position = CCVector3DZero;
    ((CCSpatialTransformComponentPrivate)Component)->rotation = CCVector3DZero;
    ((CCSpatialTransformComponentPrivate)Component)->scale = CCVector3DZero;
}

static inline void CCSpatialTransformComponentDeallocate(CCComponent Component)
{
    CCComponentDeallocate(Component);
}

static inline CCVector3D CCSpatialTransformComponentGetPosition(CCComponent Component)
{
    return ((CCSpatialTransformComponentPrivate)Component)->position;
}

static inline void CCSpatialTransformComponentSetPosition(CCComponent Component, CCVector3D Position)
{
    ((CCSpatialTransformComponentPrivate)Component)->position = Position;
}

static inline CCVector3D CCSpatialTransformComponentGetRotation(CCComponent Component)
{
    return ((CCSpatialTransformComponentPrivate)Component)->rotation;
}

static inline void CCSpatialTransformComponentSetRotation(CCComponent Component, CCVector3D Rotation)
{
    ((CCSpatialTransformComponentPrivate)Component)->rotation = Rotation;
}

static inline CCVector3D CCSpatialTransformComponentGetScale(CCComponent Component)
{
    return ((CCSpatialTransformComponentPrivate)Component)->scale;
}

static inline void CCSpatialTransformComponentSetScale(CCComponent Component, CCVector3D Scale)
{
    ((CCSpatialTransformComponentPrivate)Component)->scale = Scale;
}

static inline CCMatrix4 CCSpatialTransformComponentGetTransformationMatrix(CCComponent Component)
{
    CCMatrix4 TransformMatrix = CCMatrix4MakeScale(((CCSpatialTransformComponentPrivate)Component)->scale);
    TransformMatrix = CCMatrix4RotateX(TransformMatrix, ((CCSpatialTransformComponentPrivate)Component)->rotation.x);
    TransformMatrix = CCMatrix4RotateY(TransformMatrix, ((CCSpatialTransformComponentPrivate)Component)->rotation.y);
    TransformMatrix = CCMatrix4RotateZ(TransformMatrix, ((CCSpatialTransformComponentPrivate)Component)->rotation.z);
    
    return CCMatrix4Translate(TransformMatrix, ((CCSpatialTransformComponentPrivate)Component)->position);
}

#endif
