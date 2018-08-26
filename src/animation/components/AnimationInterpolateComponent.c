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

#include "AnimationInterpolateComponent.h"
#include "ComponentExpressions.h"

const CCString CCAnimationInterpolateComponentName = CC_STRING("animation_interpolate");

static const CCComponentExpressionDescriptor CCAnimationInterpolateComponentDescriptor = {
    .id = CC_ANIMATION_INTERPOLATE_COMPONENT_ID,
    .initialize = NULL,
    .deserialize = CCAnimationInterpolateComponentDeserializer,
    .serialize = NULL
};

void CCAnimationInterpolateComponentRegister(void)
{
    CCComponentRegister(CC_ANIMATION_INTERPOLATE_COMPONENT_ID, CCAnimationInterpolateComponentName, CC_STD_ALLOCATOR, sizeof(CCAnimationInterpolateComponentClass), CCAnimationInterpolateComponentInitialize, NULL, CCAnimationInterpolateComponentDeallocate);
    
    CCComponentExpressionRegister(CC_STRING("animation-interpolate"), &CCAnimationInterpolateComponentDescriptor, TRUE);
}

void CCAnimationInterpolateComponentDeregister(void)
{
    CCComponentDeregister(CC_ANIMATION_INTERPOLATE_COMPONENT_ID);
}

static CCDictionary Interpolators;
void CCAnimationInterpolateComponentDeserializer(CCComponent Component, CCExpression Arg, _Bool Deferred)
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
                if (CCStringEqual(Name, CC_STRING("interpolator:")))
                {
                    if (ArgCount == 2)
                    {
                        CCExpression InterpolatorNameExpr = *(CCExpression*)CCOrderedCollectionGetElementAtIndex(CCExpressionGetList(Arg), 1);
                        if (CCExpressionGetType(InterpolatorNameExpr) == CCExpressionValueTypeAtom)
                        {
                            CCString InterpolatorName = CCExpressionGetAtom(InterpolatorNameExpr);
                            
                            CCAnimationInterpolator *Interpolator = Interpolators ? CCDictionaryGetValue(Interpolators, &InterpolatorName) : NULL;
                            if (Interpolator)
                            {
                                CCAnimationInterpolateComponentSetInterpolator(Component, *Interpolator);
                            }
                            
                            else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR_CUSTOM("Value (:%S) for argument (interpolator:) is not a valid atom", InterpolatorName);
                        }
                        
                        else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (interpolator:) to be an atom");
                    }
                    
                    else if (!CCComponentExpressionDeserializeDeferredArgument(Component, Arg, Deferred)) CC_LOG_ERROR("Expect value for argument (interpolator:) to be an atom");
                    
                    return;
                }
            }
        }
    }
    
    CCAnimationInterpolateComponentDeserializer(Component, Arg, Deferred);
}

void CCAnimationInterpolateComponentRegisterCallback(CCString Name, CCAnimationInterpolator Interpolator)
{
    if (!Interpolators)
    {
        Interpolators = CCDictionaryCreate(CC_STD_ALLOCATOR, CCDictionaryHintHeavyFinding | CCDictionaryHintConstantLength | CCDictionaryHintConstantElements, sizeof(CCString), sizeof(Interpolator), &(CCDictionaryCallbacks){
            .keyDestructor = CCStringDestructorForDictionary,
            .getHash = CCStringHasherForDictionary,
            .compareKeys = CCStringComparatorForDictionary
        });
    }
    
    CCDictionarySetValue(Interpolators, &(CCString){ CCStringCopy(Name) }, &Interpolator);
}
