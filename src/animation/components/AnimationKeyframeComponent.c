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

#include "AnimationKeyframeComponent.h"
#include "ComponentExpressions.h"

const CCString CCAnimationKeyframeComponentName = CC_STRING("animation_keyframe");

static const CCComponentExpressionDescriptor CCAnimationKeyframeComponentDescriptor = {
    .id = CC_ANIMATION_KEYFRAME_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCAnimationKeyframeComponentDeserializer,
    .serialize = NULL
};

void CCAnimationKeyframeComponentRegister(void)
{
    CCComponentRegister(CC_ANIMATION_KEYFRAME_COMPONENT_ID, CCAnimationKeyframeComponentName, CC_STD_ALLOCATOR, sizeof(CCAnimationKeyframeComponentClass), CCAnimationKeyframeComponentInitialize, NULL, CCAnimationKeyframeComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("animation-keyframe"), &CCAnimationKeyframeComponentDescriptor, TRUE);
}

void CCAnimationKeyframeComponentDeregister(void)
{
    CCComponentDeregister(CC_ANIMATION_KEYFRAME_COMPONENT_ID);
}

static CCComponentExpressionArgumentDeserializer Arguments[] = {
    { .name = CC_STRING("frame:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat64, .setter = (CCComponentExpressionSetter)CCAnimationKeyframeComponentSetFrame },
    { .name = CC_STRING("frames:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeFloat64 | CCComponentExpressionArgumentTypeContainerOrderedCollection, .setter = (CCComponentExpressionSetter)CCAnimationKeyframeComponentSetFrames },
    { .name = CC_STRING("index:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeSize, .setter = (CCComponentExpressionSetter)CCAnimationKeyframeComponentSetIndex },
    { .name = CC_STRING("data:"), .serializedType = CCExpressionValueTypeUnspecified, .setterType = CCComponentExpressionArgumentTypeData | CCComponentExpressionArgumentTypeContainerOrderedCollection, .setter = (CCComponentExpressionSetter)CCAnimationKeyframeComponentSetData }
};

void CCAnimationKeyframeComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
{
    if (CCExpressionGetType(Arg) == CCExpressionValueTypeList)
    {
        const size_t ArgCount = CCCollectionGetCount(CCExpressionGetList(Arg));
        if (CCCollectionGetCount(CCExpressionGetList(Arg)) >= 2)
        {
            CCExpression NameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 0);
            if (CCExpressionGetType(NameExpr) == CCExpressionValueTypeAtom)
            {
                CCString Name = CCExpressionGetAtom(NameExpr);
                if (CCStringEqual(Name, CC_STRING("direction:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression DirectionExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(DirectionExpr) == CCExpressionValueTypeAtom)
                        {
                            CCString DirectionType = CCExpressionGetAtom(DirectionExpr);
                            if (CCStringEqual(DirectionType, CC_STRING(":forward"))) CCAnimationKeyframeComponentSetDirection(Component, CCAnimationKeyframeDirectionForward);
                            else if (CCStringEqual(DirectionType, CC_STRING(":backward"))) CCAnimationKeyframeComponentSetDirection(Component, CCAnimationKeyframeDirectionBackward);
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (direction:) is not a valid atom", DirectionType);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (direction:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (direction:) to be an atom");
                    
                    return;
                }
            }
        }
    }
    
    if (!CCComponentExpressionDeserializeArgument(Component, Arg, Arguments, sizeof(Arguments) / sizeof(typeof(*Arguments)), Deferred))
    {
        CCAnimationComponentDeserializer(Component, Arg, Deferred);
    }
}
